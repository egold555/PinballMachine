// Downloads a song from onlinesequencer.net and saves it as a MIDI file, one
// track per instrument, named after it. Drum kits go on channel 10 (which
// the machine leaves out). Then write its recipe in Midis/arrangements.yaml.
//
//   make import-sequence ID=1905502 OUT="Midis/frenzy/My Song.mid"
//
// The site has no MIDI download without a browser, but the page loads its
// notes from an API in protobuf form, which is decoded here.

import { mkdirSync, writeFileSync } from 'node:fs';
import { dirname } from 'node:path';

// Online Sequencer's instrument list, by ID (from the site's script).
const INSTRUMENTS = [
    'Elec. Piano (Classic)', 'Acoustic Gtr (Classic)', 'Drum Kit', 'Smooth Synth (Classic)', 'Electric Guitar',
    'Bass Guitar (Classic)', 'Synth Pluck (Classic)', 'Scifi', 'Grand Piano (Classic)', 'French Horn (Classic)',
    'Trombone (Classic)', 'Violin (Classic)', 'Cello (Classic)', '8-Bit Sine', '8-Bit Square', '8-Bit Sawtooth',
    '8-Bit Triangle', 'Harpsichord', 'Concert Harp', 'Xylophone', 'Pizzicato', 'Steel Drums', 'Sitar',
    'Flute (Classic)', 'Saxophone', 'Ragtime Piano', 'Music Box', 'Synth Bass (Classic)', 'Church Organ (Classic)',
    'Slap Bass', 'Pop Synth (Classic)', 'Electric Drum Kit', 'Jazz Guitar', 'Koto', 'Vibraphone', 'Muted E-Guitar',
    '808 Drum Kit', '808 Bass', 'Dist. Guitar (Classic)', '8-Bit Drum Kit', '2013 Drum Kit', 'Grand Piano',
    '909 Drum Kit', 'Electric Piano', 'Distortion Guitar', 'Cello', 'Violin', 'Strings', 'Bass', 'Clean Guitar',
    'French Horn', 'Trombone', 'Smooth Synth', '2023 Drum Kit', 'Bass Guitar', 'Synthesizer', 'Synth Bass',
    'Pop Synth', 'Acoustic Guitar', 'Lucent Choir', 'EDM Kit (E)', 'Brass', 'Rhodes', 'Church Organ', 'Rock Kit',
    'Release-Cut Piano', 'Choir Ahhs', 'Alto Sax', 'Synth Pluck', 'Upright Piano', 'Flute',
];
const DRUM_KITS = new Set([2, 31, 36, 39, 40, 42, 53, 60, 64]);

const TICKS_PER_BEAT = 480;
const TICKS_PER_STEP = TICKS_PER_BEAT / 4; // the site's time unit is a 16th note

interface Note {
    type: number; // 0 = C0, so MIDI note = type + 12
    time: number; // in 16ths
    length: number;
    instrument: number;
    volume: number;
}

// ------------------------------------------------------------ Protobuf

type Field = { field: number; wire: number; value: number | Uint8Array };

function fields(b: Uint8Array): Field[] {
    const out: Field[] = [];
    const view = new DataView(b.buffer, b.byteOffset, b.byteLength);
    let p = 0;
    const varint = () => {
        let v = 0;
        let mul = 1;
        for (;;) {
            const x = b[p++];
            v += (x & 0x7f) * mul;
            mul *= 128;
            if (x < 0x80) return v;
        }
    };
    while (p < b.length) {
        const key = varint();
        const [field, wire] = [Math.floor(key / 8), key & 7];
        if (wire === 0) out.push({ field, wire, value: varint() });
        else if (wire === 5) out.push({ field, wire, value: (view.getFloat32(p, true), (p += 4), view.getFloat32(p - 4, true)) });
        else if (wire === 1) out.push({ field, wire, value: (view.getFloat64(p, true), (p += 8), view.getFloat64(p - 8, true)) });
        else if (wire === 2) {
            const len = varint();
            out.push({ field, wire, value: b.subarray(p, p + len) });
            p += len;
        } else throw new Error(`unexpected protobuf wire type ${wire}`);
    }
    return out;
}

function decode(data: Uint8Array): { bpm: number; notes: Note[] } {
    let bpm = 110; // the site's default
    const notes: Note[] = [];
    for (const f of fields(data)) {
        if (f.field === 1 && f.value instanceof Uint8Array) {
            const b = fields(f.value).find((s) => s.field === 1);
            if (typeof b?.value === 'number' && b.value > 0) bpm = b.value;
        } else if (f.field === 2 && f.value instanceof Uint8Array) {
            const n: Note = { type: 0, time: 0, length: 0, instrument: 0, volume: 1 };
            for (const s of fields(f.value)) {
                if (typeof s.value !== 'number') continue;
                if (s.field === 1) n.type = s.value;
                else if (s.field === 2) n.time = s.value;
                else if (s.field === 3) n.length = s.value;
                else if (s.field === 4) n.instrument = s.value;
                else if (s.field === 5) n.volume = s.value;
            }
            notes.push(n);
        }
    }
    return { bpm, notes };
}

// ---------------------------------------------------------------- MIDI

function vlq(v: number): number[] {
    const out = [v & 0x7f];
    for (v >>= 7; v; v >>= 7) out.unshift((v & 0x7f) | 0x80);
    return out;
}

function chunk(tag: string, body: number[]): number[] {
    const n = body.length;
    return [...Buffer.from(tag), (n >>> 24) & 0xff, (n >> 16) & 0xff, (n >> 8) & 0xff, n & 0xff, ...body];
}

function track(events: { tick: number; bytes: number[] }[]): number[] {
    events.sort((a, b) => a.tick - b.tick || (a.bytes[0] & 0xf0) - (b.bytes[0] & 0xf0)); // offs (0x8_) first
    const body: number[] = [];
    let last = 0;
    for (const e of events) {
        body.push(...vlq(e.tick - last), ...e.bytes);
        last = e.tick;
    }
    return chunk('MTrk', [...body, 0, 0xff, 0x2f, 0]);
}

function toMidi(bpm: number, notes: Note[]): Uint8Array {
    const usPerBeat = Math.round(60_000_000 / bpm);
    const tracks = [track([{ tick: 0, bytes: [0xff, 0x51, 3, (usPerBeat >> 16) & 0xff, (usPerBeat >> 8) & 0xff, usPerBeat & 0xff] }])];

    const instruments = [...new Set(notes.map((n) => n.instrument))];
    let nextChannel = 0;
    for (const inst of instruments) {
        const drums = DRUM_KITS.has(inst);
        const ch = drums ? 9 : nextChannel++;
        if (nextChannel === 9) nextChannel++; // 10 is for drums
        if (ch > 15) throw new Error('more than 15 instruments');
        const name = INSTRUMENTS[inst] ?? `instrument ${inst}`;
        const events = [{ tick: 0, bytes: [0xff, 0x03, ...vlq(name.length), ...Buffer.from(name, 'latin1')] }];
        for (const n of notes.filter((x) => x.instrument === inst)) {
            const note = n.type + 12;
            if (note > 127) continue;
            const start = Math.round(n.time * TICKS_PER_STEP);
            const end = Math.max(start + 1, Math.round((n.time + n.length) * TICKS_PER_STEP));
            const vel = Math.max(1, Math.min(127, Math.round(n.volume * 100)));
            events.push({ tick: start, bytes: [0x90 | ch, note, vel] }, { tick: end, bytes: [0x80 | ch, note, 0] });
        }
        tracks.push(track(events));
        console.log(`  channel ${ch + 1}: ${name}${drums ? ' (drums)' : ''}`);
    }
    return Uint8Array.from([
        ...chunk('MThd', [0, 1, 0, tracks.length, TICKS_PER_BEAT >> 8, TICKS_PER_BEAT & 0xff]),
        ...tracks.flat(),
    ]);
}

// ---------------------------------------------------------------- Main

const [id, out] = process.argv.slice(2);
if (!/^\d+$/.test(id ?? '') || !out) {
    console.error('usage: npx tsx tools/import-sequence.ts <song id> <out.mid>');
    process.exit(1);
}
const res = await fetch(`https://onlinesequencer.net/app/api/get_proto.php?v=2&id=${id}`);
if (!res.ok) throw new Error(`onlinesequencer.net answered ${res.status}`);
const { bpm, notes } = decode(new Uint8Array(await res.arrayBuffer()));
if (notes.length === 0) throw new Error(`song ${id} has no notes (wrong ID?)`);
console.log(`Song ${id}: ${notes.length} notes at ${bpm} bpm`);
mkdirSync(dirname(out), { recursive: true });
writeFileSync(out, toMidi(bpm, notes));
console.log(`Saved ${out}`);
