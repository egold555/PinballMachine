// Writes the original 8-bit songs made for the machine's three voices: lead,
// arpeggio and bass. Each song is a list of 8-bar sections. Edit them below
// and rerun:
//
//   make compose                         every song (npx tsx tools/compose.ts)
//   npx tsx tools/compose.ts frenzy      just one
//   make songs && make preview           then hear them
//
// Lead notation: "E5:4" is E5 for 4 sixteenths, "-:4" a rest. 16 per bar.

import { writeFileSync } from 'node:fs';
import { join } from 'node:path';

const MIDIS = join(import.meta.dirname, '..', '..', 'Midis');
const TICKS_PER_16TH = 120; // 480 per beat

// ----------------------------------------------------------- Harmony

type Chord = { root: number; minor: boolean }; // root as a MIDI note in octave 2-3
const ch = (name: string): Chord => {
    const roots: Record<string, number> = { C: 48, D: 50, E: 40, F: 41, G: 43, A: 45, Bb: 46 };
    const minor = name.endsWith('m');
    return { root: roots[minor ? name.slice(0, -1) : name], minor };
};
const tones = (c: Chord) => [0, c.minor ? 3 : 4, 7].map((i) => c.root + i);

interface Section {
    chords: string[]; // one per bar
    lead: string; // 16 sixteenths per bar
    arp: 'sixteenths' | 'eighths';
    bass: 'drive' | 'long';
}

interface Song {
    file: string; // relative to Midis/
    bpm: number;
    sections: [Section, number][]; // played in order, each with a key change in semitones
}

// ------------------------------------------------------- Frenzy theme
// 150 bpm, 56 bars (89.6 s), roughly one play through a 90-second frenzy.

const A: Section = {
    chords: ['Am', 'Am', 'F', 'F', 'C', 'C', 'G', 'G'],
    lead: `E5:4 E5:2 D5:2 C5:4 D5:2 E5:2 | A4:6 C5:2 E5:4 -:4 |
           F5:4 E5:2 D5:2 C5:4 A4:4 | C5:2 D5:2 F5:4 E5:4 -:4 |
           G5:4 E5:2 C5:2 E5:4 G5:4 | A5:6 G5:2 E5:4 -:4 |
           B4:2 D5:2 G5:4 F5:2 E5:2 D5:4 | B4:4 D5:4 E5:8`,
    arp: 'sixteenths',
    bass: 'drive',
};

const A2: Section = {
    chords: A.chords,
    lead: `A5:2 G5:2 E5:2 G5:2 A5:4 C6:4 | B5:2 A5:2 G5:2 E5:2 A5:8 |
           A5:2 F5:2 C5:2 F5:2 A5:4 C6:4 | B5:2 A5:2 G5:2 F5:2 E5:8 |
           G5:2 E5:2 C5:2 E5:2 G5:4 C6:4 | D6:4 C6:2 B5:2 C6:4 G5:4 |
           B5:2 G5:2 D5:2 G5:2 B5:4 D6:4 | C6:2 B5:2 A5:2 G5:2 E5:4 G5:4`,
    arp: 'sixteenths',
    bass: 'drive',
};

const B: Section = {
    chords: ['Dm', 'Dm', 'Am', 'Am', 'Bb', 'Bb', 'E', 'E'],
    lead: `D5:6 F5:2 A5:4 F5:4 | E5:4 D5:4 C5:4 D5:4 |
           E5:6 A5:2 C6:4 A5:4 | B5:4 A5:4 G5:4 E5:4 |
           F5:6 D5:2 F5:4 A#5:4 | A5:4 G5:4 F5:4 D5:4 |
           E5:4 G#5:4 B5:4 E6:4 | D6:4 B5:4 G#5:4 E5:4`,
    arp: 'sixteenths',
    bass: 'drive',
};

const BREAKDOWN: Section = {
    chords: ['Am', 'Am', 'E', 'E', 'F', 'F', 'G', 'G'],
    lead: `E5:8 C5:8 | A4:16 | G#5:8 E5:8 | B4:16 |
           A5:8 F5:8 | C5:16 | B5:8 D6:8 | G5:4 A5:4 B5:4 D6:4`,
    arp: 'eighths',
    bass: 'long',
};

// ------------------------------------------------- Sudden Death songs
// The music is one tense 8-bar loop (140 bpm, 13.7 s): each time round is a
// level, and the game plays it a key higher and faster every time. The
// ending is slow and sad, for the score screen.

const SUDDEN_DEATH: Section = {
    chords: ['Am', 'Am', 'F', 'E', 'Am', 'Am', 'Dm', 'E'],
    lead: `A5:2 E5:2 A5:2 B5:2 C6:2 B5:2 A5:2 E5:2 | A5:2 E5:2 A5:2 B5:2 C6:4 E6:4 |
           F5:2 C5:2 F5:2 G5:2 A5:2 G5:2 F5:2 C5:2 | E5:2 G#5:2 B5:2 E6:2 D6:4 B5:4 |
           A5:2 E5:2 A5:2 B5:2 C6:2 B5:2 A5:2 E5:2 | A5:2 C6:2 E6:4 D6:2 C6:2 B5:4 |
           D6:2 A5:2 F5:2 A5:2 D6:4 F6:4 | E6:4 D6:2 C6:2 B5:4 G#5:4`,
    arp: 'sixteenths',
    bass: 'drive',
};

const SUDDEN_DEATH_ENDING: Section = {
    chords: ['Am', 'Am', 'F', 'F', 'C', 'C', 'E', 'E'],
    lead: `E5:8 D5:4 C5:4 | A4:16 | F5:8 E5:4 D5:4 | C5:16 |
           E5:8 G5:4 E5:4 | C5:8 B4:8 | G#4:8 B4:4 D5:4 | E5:16`,
    arp: 'eighths',
    bass: 'long',
};

const SONGS: Record<string, Song> = {
    frenzy: {
        file: 'frenzy/Frenzy theme.mid',
        bpm: 150,
        sections: [[A, 0], [A2, 0], [B, 0], [BREAKDOWN, 0], [A, 2], [B, 2], [A2, 2]],
    },
    'sudden-death': {
        file: 'sudden-death/Sudden Death theme.mid',
        bpm: 140,
        sections: [[SUDDEN_DEATH, 0]],
    },
    'sudden-death-ending': {
        file: 'sudden-death/Sudden Death ending.mid',
        bpm: 100,
        sections: [[SUDDEN_DEATH_ENDING, 0]],
    },
};

// ----------------------------------------------------------- Notes

type Note = { note: number; start: number; length: number }; // in sixteenths
const NAMES: Record<string, number> = { C: 0, 'C#': 1, D: 2, 'D#': 3, E: 4, F: 5, 'F#': 6, G: 7, 'G#': 8, A: 9, 'A#': 10, B: 11 };
const midiNote = (n: string) => {
    const m = /^([A-G]#?)(\d)$/.exec(n);
    if (!m) throw new Error(`bad note ${n}`);
    return (Number(m[2]) + 1) * 12 + NAMES[m[1]];
};

// Lead, arpeggio and bass, and the length in bars.
function notes(song: Song): { lines: Note[][]; bars: number } {
    const lines: Note[][] = [[], [], []];
    let bar = 0;
    for (const [section, key] of song.sections) {
        let t = bar * 16;
        for (const tok of section.lead.replaceAll('|', ' ').split(/\s+/).filter(Boolean)) {
            const [n, len] = tok.split(':');
            if (n !== '-') lines[0].push({ note: midiNote(n) + key, start: t, length: Number(len) });
            t += Number(len);
        }
        if (t !== (bar + section.chords.length) * 16)
            throw new Error(`a lead is ${t - bar * 16} sixteenths, not ${section.chords.length * 16}`);

        section.chords.forEach((name, i) => {
            const c = ch(name);
            const start = (bar + i) * 16;
            // Arpeggio: chord tones in octave 4-5, up and down.
            const up = [...tones(c).map((n) => n + 24), c.root + 36];
            const pattern = [0, 1, 2, 3, 2, 1, 2, 3];
            if (section.arp === 'sixteenths')
                for (let s = 0; s < 16; s++) lines[1].push({ note: up[pattern[s % 8]] + key, start: start + s, length: 1 });
            else for (let s = 0; s < 8; s++) lines[1].push({ note: up[pattern[s]] + key, start: start + s * 2, length: 2 });
            // Bass: driving octave eighths, or a long root.
            if (section.bass === 'drive')
                for (let s = 0; s < 8; s++) lines[2].push({ note: c.root + (s % 2 ? 12 : 0) + key, start: start + s * 2, length: 2 });
            else lines[2].push({ note: c.root + key, start, length: 16 });
        });
        bar += section.chords.length;
    }
    return { lines, bars: bar };
}

// ----------------------------------------------------------- MIDI

const vlq = (v: number) => {
    const out = [v & 0x7f];
    for (v >>= 7; v; v >>= 7) out.unshift((v & 0x7f) | 0x80);
    return out;
};
const chunk = (tag: string, body: number[]) => {
    const n = body.length;
    return [...Buffer.from(tag), (n >>> 24) & 0xff, (n >> 16) & 0xff, (n >> 8) & 0xff, n & 0xff, ...body];
};
const track = (events: { tick: number; bytes: number[] }[]) => {
    events.sort((a, b) => a.tick - b.tick || (a.bytes[0] & 0xf0) - (b.bytes[0] & 0xf0)); // offs first
    const body: number[] = [];
    let last = 0;
    for (const e of events) {
        body.push(...vlq(e.tick - last), ...e.bytes);
        last = e.tick;
    }
    return chunk('MTrk', [...body, 0, 0xff, 0x2f, 0]);
};

function write(song: Song): void {
    const { lines, bars } = notes(song);
    const us = Math.round(60_000_000 / song.bpm);
    const tracks = [track([{ tick: 0, bytes: [0xff, 0x51, 3, (us >> 16) & 0xff, (us >> 8) & 0xff, us & 0xff] }])];
    ['lead', 'arpeggio', 'bass'].forEach((name, c) => {
        const events = [{ tick: 0, bytes: [0xff, 0x03, name.length, ...Buffer.from(name)] }];
        for (const n of lines[c]) {
            events.push({ tick: n.start * TICKS_PER_16TH, bytes: [0x90 | c, n.note, 100] });
            events.push({ tick: (n.start + n.length) * TICKS_PER_16TH - 1, bytes: [0x80 | c, n.note, 0] });
        }
        tracks.push(track(events));
    });
    const out = join(MIDIS, song.file);
    writeFileSync(out, Uint8Array.from([...chunk('MThd', [0, 1, 0, tracks.length, 0x01, 0xe0]), ...tracks.flat()]));
    const seconds = (bars * 16 * 60) / song.bpm / 4;
    console.log(`${song.file}: ${bars} bars at ${song.bpm} bpm, ${seconds.toFixed(4)} s`);
}

const wanted = process.argv.slice(2);
for (const name of wanted) if (!SONGS[name]) throw new Error(`no song "${name}". Songs: ${Object.keys(SONGS).join(', ')}`);
for (const [name, song] of Object.entries(SONGS)) if (wanted.length === 0 || wanted.includes(name)) write(song);
