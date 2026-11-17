// Renders MIDI files the way the machine plays them (three square-wave
// voices, same conversion as the game) to .wav files, to listen to on a
// computer before putting them on the machine.
//
//   npx tsx tools/preview.ts sounds/ending-champ.mid ...   writes a .wav next to each
//   npx tsx tools/preview.ts --out <folder> <files...>     writes them there instead,
//                                     named <mode>-<sound>.wav for sounds/<mode>/<sound>.mid

import { mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { basename, dirname, join } from 'node:path';
import { midiToScore } from '../src/sounds/midi.js';
import { SOUND_CHANNELS } from '../src/protocol/index.js';

const RATE = 22050;
const VOLUME = 0.15; // per voice, so three at once can't clip

function render(score: Uint8Array): Int16Array {
    // Walk the score into (time, voice, note or null) changes.
    const changes: { ms: number; voice: number; note: number | null }[] = [];
    let ms = 0;
    for (let i = 0; i < score.length; ) {
        const c = score[i++];
        if (c < 0x80) ms += (c << 8) | score[i++];
        else if ((c & 0xf0) === 0x90) changes.push({ ms, voice: c & 0x0f, note: score[i++] });
        else if ((c & 0xf0) === 0x80) changes.push({ ms, voice: c & 0x0f, note: null });
        else break;
    }

    const out = new Int16Array(Math.ceil(((ms + 300) / 1000) * RATE));
    const freq: (number | null)[] = new Array(SOUND_CHANNELS).fill(null);
    const phase = new Array(SOUND_CHANNELS).fill(0);
    let next = 0;
    for (let s = 0; s < out.length; s++) {
        const t = (s / RATE) * 1000;
        while (next < changes.length && changes[next].ms <= t) {
            const ch = changes[next++];
            if (ch.voice < SOUND_CHANNELS) freq[ch.voice] = ch.note === null ? null : 440 * 2 ** ((ch.note - 69) / 12);
        }
        let v = 0;
        for (let k = 0; k < SOUND_CHANNELS; k++) {
            const f = freq[k];
            if (f === null) continue;
            phase[k] = (phase[k] + f / RATE) % 1;
            v += phase[k] < 0.5 ? VOLUME : -VOLUME;
        }
        out[s] = Math.round(v * 32767);
    }
    return out;
}

function wav(samples: Int16Array): Buffer {
    const header = Buffer.alloc(44);
    header.write('RIFF', 0);
    header.writeUInt32LE(36 + samples.byteLength, 4);
    header.write('WAVEfmt ', 8);
    header.writeUInt32LE(16, 16);
    header.writeUInt16LE(1, 20); // PCM
    header.writeUInt16LE(1, 22); // mono
    header.writeUInt32LE(RATE, 24);
    header.writeUInt32LE(RATE * 2, 28);
    header.writeUInt16LE(2, 32);
    header.writeUInt16LE(16, 34);
    header.write('data', 36);
    header.writeUInt32LE(samples.byteLength, 40);
    return Buffer.concat([header, Buffer.from(samples.buffer, samples.byteOffset, samples.byteLength)]);
}

let files = process.argv.slice(2);
let outDir: string | null = null;
if (files[0] === '--out') {
    outDir = files[1];
    files = files.slice(2);
    mkdirSync(outDir, { recursive: true });
}

for (const file of files) {
    const midi = midiToScore(readFileSync(file));
    const name = file.replace(/\.mid$/i, '') + '.wav';
    const out = outDir ? join(outDir, `${basename(dirname(name))}-${basename(name)}`) : name;
    writeFileSync(out, wav(render(midi.score)));
    console.log(`${out}  ${(midi.ms / 1000).toFixed(1)} s, ${midi.notes} notes, ${midi.dropped} cut for lack of a voice`);
}
