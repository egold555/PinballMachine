// Turns the source MIDI files listed in Midis/arrangements.yaml into
// sounds/<name>.mid, fitted to the machine's three square-wave voices.
// See Midis/README.md for the config.
//
//   make songs                                every sound in the config
//   npx tsx tools/arrange.ts ending-champ     just one
//   npx tsx tools/arrange.ts --list <file>    show a file's tracks, to write a recipe
//
// "lines" style: each line follows a list of parts in order of preference:
// whenever the first part is silent, the next one that is playing fills in.
// A part with chords contributes its top note (or bottom note, for bass).
// Drums and anything not listed are left out. The silence before the first note is
// cut, and any silence longer than 3 seconds inside the song (a drums-only
// break, say) is shortened to one beat. Output goes to sounds/, then
// `make deploy`.

import { readFileSync, writeFileSync } from 'node:fs';
import { basename, join } from 'node:path';
import { type MidiFile, type MidiNote, readMidi } from '../src/sounds/midi.js';
import {
    type Arrangement,
    type LinesArrangement,
    loadArrangements,
    MIDIS_DIR,
    type Part,
    type PluckArrangement,
} from './arrangements.js';

const OUT_DIR = join(import.meta.dirname, '..', 'sounds');
const NAMES = 'C C# D D# E F F# G G# A A# B'.split(' ');
// General MIDI instrument families, 8 instruments each.
const FAMILIES = [
    'piano', 'chromatic percussion', 'organ', 'guitar', 'bass', 'strings', 'ensemble/choir', 'brass',
    'reed/sax', 'pipe/flute', 'synth lead', 'synth pad', 'synth effects', 'ethnic', 'percussive', 'sound effects',
];
const noteName = (n: number) => `${NAMES[n % 12]}${Math.floor(n / 12) - 1}`;

// Bass is moved up whole octaves until most of it is at least this high
// (E2, 82 Hz). Lower than that is mostly a click on a small speaker.
const BASS_FLOOR = 40;
// Anything else is moved down if most of it is above C7.
const TOP_CEILING = 96;
// Silences longer than this are shortened to one beat.
const MAX_SILENCE_MS = 3000;
// Pluck style: how often new notes are counted (Musicotron's 48 per second),
// and the range notes are folded into by octaves.
const PLUCK_SLOT_MS = 1000 / 48;
const PLUCK_LOW = 33; // A1
const PLUCK_HIGH = 96; // C7

interface Segment {
    note: number;
    start: number;
    end: number;
}

// One line: at every moment, the first part that has a note sounding wins.
function buildLine(midi: MidiFile, parts: Part[], pick: 'top' | 'bottom', fromTick: number): Segment[] {
    const sources = parts.map((part) =>
        midi.notes.filter(
            (n) =>
                n.start >= fromTick &&
                (part.track === undefined || n.track === part.track) &&
                (part.channel === undefined || n.channel === part.channel - 1) &&
                n.channel !== 9,
        ),
    );
    const times = [...new Set(sources.flat().flatMap((n) => [n.start, n.end]))].sort((a, b) => a - b);

    const segments: Segment[] = [];
    let current: MidiNote | null = null;
    for (let i = 0; i < times.length - 1; i++) {
        const t = times[i];
        let chosen: MidiNote | null = null;
        for (const src of sources) {
            const sounding = src.filter((n) => n.start <= t && n.end > t);
            if (sounding.length === 0) continue;
            chosen = sounding.reduce((a, b) => ((pick === 'top' ? b.note > a.note : b.note < a.note) ? b : a));
            break;
        }
        if (chosen === current) {
            if (chosen) segments[segments.length - 1].end = times[i + 1];
            continue;
        }
        current = chosen;
        if (chosen) segments.push({ note: chosen.note, start: t, end: times[i + 1] });
    }
    return segments;
}

// Pluck style (see arrangements.ts): returns one list of notes per voice.
function pluckVoices(midi: MidiFile, a: PluckArrangement, fromTick: number, voices = 3): Segment[][] {
    const channels = a.channels?.map((c) => c - 1);
    const notes = midi.notes
        .filter((n) => n.start >= fromTick && n.channel !== 9 && (!channels || channels.includes(n.channel)))
        .sort((x, y) => x.start - y.start || x.order - y.order);

    const lines: Segment[][] = Array.from({ length: voices }, () => []);
    const playing: { segment: Segment; since: number }[] = [];
    let slot = -1;
    let inSlot = 0;
    for (const n of notes) {
        const startMs = midi.toMs(n.start);
        const s = Math.floor(startMs / PLUCK_SLOT_MS);
        if (s !== slot) [slot, inSlot] = [s, 0];
        if (inSlot >= voices) continue; // chord has more notes than voices: first ones win
        inSlot++;

        let note = n.note;
        while (note < PLUCK_LOW) note += 12;
        while (note > PLUCK_HIGH) note -= 12;
        const end = Math.min(n.end, tickAt(midi, startMs + (a.pluckMs ?? 250)));
        if (end <= n.start) continue;
        const segment = { note, start: n.start, end };

        // A free voice, or else the one that started longest ago.
        let v = lines.findIndex((line) => (line.at(-1)?.end ?? -1) <= n.start);
        if (v < 0) {
            v = playing.reduce((best, p, i) => (p.since < playing[best].since ? i : best), 0);
            lines[v].at(-1)!.end = n.start; // cut short
        }
        lines[v].push(segment);
        playing[v] = { segment, since: n.start };
    }
    return lines;
}

function median(xs: number[]): number {
    const s = [...xs].sort((a, b) => a - b);
    return s[Math.floor(s.length / 2)] ?? 0;
}

function octaveShift(segments: Segment[], bass: boolean): number {
    const m = median(segments.map((s) => s.note));
    let shift = 0;
    if (bass) while (m + shift < BASS_FLOOR) shift += 12;
    else while (m + shift > TOP_CEILING) shift -= 12;
    return shift;
}

// Where the song is silent for too long (and at the start), returns cuts
// that remove that time. Every tick is then moved by `shift`.
function silenceCuts(midi: MidiFile, segments: Segment[]) {
    const spans = [...segments].sort((a, b) => a.start - b.start);
    const cuts: { from: number; to: number }[] = [{ from: 0, to: spans[0].start }];
    let end = spans[0].end;
    for (const s of spans) {
        if (s.start > end && midi.toMs(s.start) - midi.toMs(end) > MAX_SILENCE_MS)
            cuts.push({ from: end + midi.ticksPerBeat, to: s.start });
        end = Math.max(end, s.end);
    }
    // A tick inside a cut lands on the cut's start.
    const shift = (tick: number) => {
        let removed = 0;
        for (const c of cuts) {
            if (tick >= c.to) removed += c.to - c.from;
            else if (tick > c.from) removed += tick - c.from;
        }
        return tick - removed;
    };
    return { cuts, shift };
}

// ------------------------------------------------------------- Writing

function vlq(v: number): number[] {
    const out = [v & 0x7f];
    for (v >>= 7; v; v >>= 7) out.unshift((v & 0x7f) | 0x80);
    return out;
}

function track(events: { tick: number; bytes: number[] }[]): number[] {
    events.sort((a, b) => a.tick - b.tick);
    const body: number[] = [];
    let last = 0;
    for (const e of events) {
        body.push(...vlq(e.tick - last), ...e.bytes);
        last = e.tick;
    }
    body.push(0, 0xff, 0x2f, 0);
    const len = body.length;
    return [0x4d, 0x54, 0x72, 0x6b, (len >>> 24) & 0xff, (len >> 16) & 0xff, (len >> 8) & 0xff, len & 0xff, ...body];
}

function text(type: number, s: string): number[] {
    return [0xff, type, ...vlq(s.length), ...Buffer.from(s, 'latin1')];
}

function writeMidi(
    midi: MidiFile,
    lines: { name: string; segments: Segment[] }[],
    shift: (tick: number) => number,
): Uint8Array {
    // Tempo changes inside cut silence land where the cut is, so the last
    // one still applies afterwards.
    const tempos = [{ tick: 0, usPerBeat: 500_000 }, ...midi.tempos].sort((a, b) => a.tick - b.tick);
    const tempoEvents = tempos.map((t) => ({
        tick: shift(t.tick),
        bytes: [0xff, 0x51, 3, (t.usPerBeat >> 16) & 0xff, (t.usPerBeat >> 8) & 0xff, t.usPerBeat & 0xff],
    }));

    const tracks = [track(tempoEvents)];
    lines.forEach((line, ch) => {
        const events = [
            { tick: 0, bytes: text(0x03, line.name) },
            { tick: 0, bytes: [0xc0 | ch, 80] }, // square lead, so other players sound close too
        ];
        for (const s of line.segments) {
            events.push({ tick: shift(s.start), bytes: [0x90 | ch, s.note, 100] });
            events.push({ tick: shift(s.end), bytes: [0x80 | ch, s.note, 0] });
        }
        // Note-offs sort before note-ons at the same tick (stable sort keeps
        // a segment's own on/off order, and ends come from earlier segments).
        events.sort((a, b) => a.tick - b.tick || Number(a.bytes[0] >= 0x90) - Number(b.bytes[0] >= 0x90));
        tracks.push(track(events));
    });

    const div = midi.ticksPerBeat;
    return Uint8Array.from([
        0x4d, 0x54, 0x68, 0x64, 0, 0, 0, 6, 0, 1, 0, tracks.length, (div >> 8) & 0xff, div & 0xff,
        ...tracks.flat(),
    ]);
}

// ---------------------------------------------------------------- Main

// The first tick at or after this many milliseconds.
function tickAt(midi: MidiFile, ms: number): number {
    let lo = 0;
    let hi = Math.max(...midi.notes.map((n) => n.end));
    while (lo < hi) {
        const mid = (lo + hi) >> 1;
        if (midi.toMs(mid) < ms) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}

function arrangeLines(midi: MidiFile, a: LinesArrangement, fromTick: number) {
    return [
        { name: 'melody', parts: a.melody, pick: 'top' as const, bass: false },
        { name: 'middle', parts: a.middle, pick: 'top' as const, bass: false },
        { name: 'bass', parts: a.bass, pick: 'bottom' as const, bass: true },
    ].map((l) => {
        const segments = buildLine(midi, l.parts, l.pick, fromTick);
        const shift = octaveShift(segments, l.bass) + (a.transpose?.[l.name as 'melody'] ?? 0);
        return { name: l.name, segments: segments.map((s) => ({ ...s, note: s.note + shift })), shift };
    });
}

function arrange(name: string, a: Arrangement): void {
    const midi = readMidi(readFileSync(join(MIDIS_DIR, a.source)));
    const fromTick = tickAt(midi, (a.startAt ?? 0) * 1000);
    const lines =
        a.style === 'pluck'
            ? pluckVoices(midi, a, fromTick).map((segments, i) => ({ name: `voice ${i + 1}`, segments, shift: 0 }))
            : arrangeLines(midi, a, fromTick);

    const all = lines.flatMap((l) => l.segments);
    const { cuts, shift } = silenceCuts(midi, all);
    const data = writeMidi(midi, lines, shift);
    writeFileSync(join(OUT_DIR, `${name}.mid`), data);

    const secs = (ms: number) => (ms / 1000).toFixed(1);
    const cutText = cuts
        .filter((c) => c.to > c.from)
        .map((c, i) => `${secs(midi.toMs(c.to) - midi.toMs(c.from))} s ${i === 0 && c.from === 0 ? 'at the start' : `at ${secs(midi.toMs(c.from))} s`}`);
    console.log(`${name}.mid  from ${a.source} (${a.style})`);
    console.log(`  cut silence: ${cutText.join(', ') || 'none'}. Now ${secs(readMidi(data).toMs(Math.max(...readMidi(data).notes.map((n) => n.end))))} s long`);
    for (const l of lines) {
        const notes = l.segments.map((s) => s.note);
        const range = notes.length ? `${noteName(Math.min(...notes))}-${noteName(Math.max(...notes))}` : 'empty';
        const moved = l.shift ? `, moved ${l.shift > 0 ? 'up' : 'down'} ${Math.abs(l.shift / 12)} octave` : '';
        console.log(`  ${l.name.padEnd(7)} ${String(l.segments.length).padStart(5)} notes  ${range}${moved}`);
    }
}

function list(file: string): void {
    const midi = readMidi(readFileSync(file));
    console.log(basename(file));
    console.log('  Busy = notes per second while playing. Over ~10 usually means strumming or fast');
    console.log('  chords, which sound like noise as a line. At once = most notes held together.\n');
    const groups = new Map<string, MidiNote[]>();
    for (const n of midi.notes) {
        const key = `${n.track}:${n.channel}`;
        groups.set(key, [...(groups.get(key) ?? []), n]);
    }
    for (const [key, ns] of [...groups].sort((a, b) => a[1][0].channel - b[1][0].channel)) {
        const [t, c] = key.split(':').map(Number);
        const program = midi.programs.get(key) ?? 0;
        const what = c === 9 ? 'DRUMS (always left out)' : `${FAMILIES[program >> 3]} (#${program + 1})`;
        const notes = ns.map((n) => n.note);
        const first = midi.toMs(Math.min(...ns.map((n) => n.start))) / 1000;
        const last = midi.toMs(Math.max(...ns.map((n) => n.end))) / 1000;
        const edges = ns.flatMap((n) => [[n.start, 1], [n.end, -1]]).sort((a, b) => a[0] - b[0] || a[1] - b[1]);
        let held = 0;
        let atOnce = 0;
        for (const [, d] of edges) atOnce = Math.max(atOnce, (held += d));
        console.log(
            `  channel ${String(c + 1).padEnd(3)} ${what.padEnd(26)} ${(midi.trackNames[t] ?? '').slice(0, 16).padEnd(17)}` +
                `${String(ns.length).padStart(5)} notes  ${(noteName(Math.min(...notes)) + '-' + noteName(Math.max(...notes))).padEnd(8)}` +
                ` busy ${(ns.length / Math.max(last - first, 1)).toFixed(1).padStart(4)}  at once ${String(atOnce).padStart(2)}` +
                `  plays ${first.toFixed(0)}-${last.toFixed(0)} s`,
        );
    }
}

const args = process.argv.slice(2);
if (args[0] === '--list') for (const f of args.slice(1)) list(f);
else
    for (const [name, a] of Object.entries(loadArrangements()))
        if (args.length === 0 || args.includes(name)) arrange(name, a);
