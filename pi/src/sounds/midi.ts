// Turns a standard MIDI file into a Playtune score for the Mega's three
// square-wave voices (the format SoundPlayer plays).
//
// Every track and channel is merged, except drums (MIDI channel 10), which
// a square wave can't play. When more than three notes sound at once, the
// highest and lowest are kept (melody and bass) and the oldest note in the
// middle gives up its voice. Velocity, instruments and pedals are ignored.
//
// A marker named "end" (written by tools/arrange.ts for endAt) sets where the
// score ends, so a loop can include the rest after its last note.

import { SOUND_CHANNELS } from '../protocol/index.js';

export interface ConvertedMidi {
    score: Uint8Array;
    notes: number; // notes played
    dropped: number; // notes cut short or skipped for lack of a voice
    ms: number; // length
    bpm: number; // tempo at the start
}

export const END_MARKER = 'end';

const DRUM_CHANNEL = 9;
const DEFAULT_TEMPO = 500_000; // µs per quarter note, i.e. 120 bpm
const MAX_WAIT = 0x7fff; // the longest wait one score command holds

interface NoteEvent {
    tick: number;
    on: boolean;
    track: number;
    channel: number;
    note: number;
    order: number; // position in the file, keeps simultaneous notes in order
}

export interface MidiNote {
    track: number;
    channel: number; // 0-15, drums are 9
    note: number;
    start: number; // ticks
    end: number;
    order: number; // position of its note-on in the file
}

export interface MidiFile {
    ticksPerBeat: number;
    tempos: { tick: number; usPerBeat: number }[];
    notes: MidiNote[];
    programs: Map<string, number>; // "track:channel" -> General MIDI instrument
    trackNames: string[];
    toMs: (tick: number) => number;
}

// Every note in the file with its start and end, for tools that rearrange songs.
export function readMidi(data: Uint8Array): MidiFile {
    const { ticksPerBeat, events, tempos, programs, trackNames } = parse(data, true);
    events.sort((a, b) => a.tick - b.tick || Number(a.on) - Number(b.on) || a.order - b.order);
    const open = new Map<string, NoteEvent[]>();
    const notes: MidiNote[] = [];
    for (const ev of events) {
        const key = `${ev.track}:${ev.channel}:${ev.note}`;
        if (ev.on) open.set(key, [...(open.get(key) ?? []), ev]);
        else {
            const start = open.get(key)?.shift();
            if (start)
                notes.push({
                    track: ev.track,
                    channel: ev.channel,
                    note: ev.note,
                    start: start.tick,
                    end: ev.tick,
                    order: start.order,
                });
        }
    }
    return { ticksPerBeat, tempos, notes, programs, trackNames, toMs: tempoMap(ticksPerBeat, tempos) };
}

export function midiToScore(data: Uint8Array, voices = SOUND_CHANNELS): ConvertedMidi {
    const { ticksPerBeat, events, tempos, endTick } = parse(data);
    const toMs = tempoMap(ticksPerBeat, tempos);
    // Like playback: of several tempos at the very start, the last one counts.
    const startTempo = tempos.filter((t) => t.tick === 0).at(-1)?.usPerBeat ?? DEFAULT_TEMPO;

    // Same time: notes stop before new ones start, so a voice frees up.
    events.sort((a, b) => a.tick - b.tick || Number(a.on) - Number(b.on) || a.order - b.order);

    const voice: ({ key: string; note: number; since: number } | null)[] = new Array(voices).fill(null);
    const out: number[] = [];
    let at = 0; // ms written so far
    let notes = 0;
    let dropped = 0;

    const waitUntil = (ms: number) => {
        let wait = Math.round(ms) - at;
        at += Math.max(wait, 0);
        while (wait > 0) {
            const w = Math.min(wait, MAX_WAIT);
            out.push(w >> 8, w & 0xff);
            wait -= w;
        }
    };

    for (const ev of events) {
        const key = `${ev.channel}:${ev.note}`;
        if (!ev.on) {
            const v = voice.findIndex((x) => x?.key === key);
            if (v < 0) continue; // already lost its voice
            waitUntil(toMs(ev.tick));
            out.push(0x80 | v);
            voice[v] = null;
            continue;
        }

        let v = voice.indexOf(null);
        if (v < 0) {
            // All voices busy: keep the highest and lowest, steal the oldest in between.
            const pitches = [...voice.map((x) => x!.note), ev.note];
            const top = Math.max(...pitches);
            const bottom = Math.min(...pitches);
            const middle = voice
                .map((x, i) => ({ ...x!, i }))
                .filter((x) => x.note !== top && x.note !== bottom)
                .sort((a, b) => a.since - b.since);
            dropped++;
            if (middle.length === 0) continue; // the new note is the one in the middle
            v = middle[0].i;
        }
        waitUntil(toMs(ev.tick));
        out.push(0x90 | v, ev.note);
        voice[v] = { key, note: ev.note, since: ev.order };
        notes++;
    }

    if (endTick !== null) waitUntil(toMs(endTick));
    out.push(0xf0);
    return { score: Uint8Array.from(out), notes, dropped, ms: at, bpm: 60_000_000 / startTempo };
}

// --------------------------------------------------------------- Parsing

function parse(d: Uint8Array, withDrums = false) {
    let p = 0;
    const u32 = () => ((d[p++] << 24) | (d[p++] << 16) | (d[p++] << 8) | d[p++]) >>> 0;
    const u16 = () => (d[p++] << 8) | d[p++];
    const varLen = () => {
        let v = 0;
        for (;;) {
            const b = d[p++];
            v = (v << 7) | (b & 0x7f);
            if (b < 0x80 || p >= d.length) return v;
        }
    };
    const tag = () => String.fromCharCode(d[p++], d[p++], d[p++], d[p++]);

    if (tag() !== 'MThd') throw new Error('not a MIDI file');
    const headerEnd = u32() + p;
    u16(); // format: 0 and 1 both work, every track is merged
    const trackCount = u16();
    const ticksPerBeat = u16();
    if (ticksPerBeat & 0x8000) throw new Error('SMPTE timing is not supported');
    p = headerEnd;

    const events: NoteEvent[] = [];
    const tempos: { tick: number; usPerBeat: number }[] = [];
    const programs = new Map<string, number>();
    const trackNames: string[] = [];
    let endTick: number | null = null;
    let order = 0;

    for (let t = 0; t < trackCount && p < d.length; t++) {
        const name = tag();
        const end = u32() + p;
        if (name !== 'MTrk') {
            p = end;
            continue;
        }
        let tick = 0;
        let status = 0;
        while (p < end) {
            tick += varLen();
            if (d[p] & 0x80) status = d[p++];
            if (status === 0xff) {
                const type = d[p++];
                const len = varLen();
                if (type === 0x51 && len === 3) tempos.push({ tick, usPerBeat: (d[p] << 16) | (d[p + 1] << 8) | d[p + 2] });
                if (type === 0x03) trackNames[t] = String.fromCharCode(...d.subarray(p, p + len));
                if (type === 0x06 && String.fromCharCode(...d.subarray(p, p + len)) === END_MARKER) endTick = tick;
                p += len;
            } else if (status === 0xf0 || status === 0xf7) {
                const len = varLen(); // not `p += varLen()`: that adds to p from before the length was read
                p += len;
            } else {
                const kind = status & 0xf0;
                const channel = status & 0x0f;
                const a = d[p++];
                const b = kind === 0xc0 || kind === 0xd0 ? 0 : d[p++];
                if ((kind === 0x90 || kind === 0x80) && (withDrums || channel !== DRUM_CHANNEL))
                    events.push({ tick, on: kind === 0x90 && b > 0, track: t, channel, note: a, order: order++ });
                if (kind === 0xc0) programs.set(`${t}:${channel}`, a);
            }
        }
        p = end;
    }
    return { ticksPerBeat, events, tempos, programs, trackNames, endTick };
}

// Ticks to milliseconds, following tempo changes.
function tempoMap(ticksPerBeat: number, tempos: { tick: number; usPerBeat: number }[]) {
    const changes = [{ tick: 0, usPerBeat: DEFAULT_TEMPO }, ...tempos].sort((a, b) => a.tick - b.tick);
    const startMs: number[] = [0];
    for (let i = 1; i < changes.length; i++)
        startMs[i] =
            startMs[i - 1] + ((changes[i].tick - changes[i - 1].tick) * changes[i - 1].usPerBeat) / ticksPerBeat / 1000;
    return (tick: number) => {
        let i = changes.length - 1;
        while (changes[i].tick > tick) i--;
        return startMs[i] + ((tick - changes[i].tick) * changes[i].usPerBeat) / ticksPerBeat / 1000;
    };
}
