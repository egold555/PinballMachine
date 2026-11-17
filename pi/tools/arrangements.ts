// Reads Midis/arrangements.yaml: for each sound, which source MIDI to use
// and how to fit it onto the machine's three voices. See Midis/README.md.

import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import { parse } from 'yaml';
import { GAME_MODES } from '../src/games/index.js';

export const MIDIS_DIR = join(import.meta.dirname, '..', '..', 'Midis');
export const CONFIG_FILE = join(MIDIS_DIR, 'arrangements.yaml');

export interface Part {
    track?: number;
    channel?: number; // 1-16, like in MIDI editors
}

interface Common {
    source: string; // relative to Midis/
    startAt?: number; // seconds of the source to skip
    endAt?: number; // seconds into the source where it ends, exactly (for a clean loop)
}

export interface PluckArrangement extends Common {
    style: 'pluck';
    channels?: number[]; // default: every channel except drums (10)
    pluckMs?: number; // how long each note sounds at most, default 250
}

export interface LinesArrangement extends Common {
    style: 'lines';
    melody: Part[];
    middle: Part[];
    bass: Part[];
    transpose?: Partial<Record<'melody' | 'middle' | 'bass', number>>; // semitones
}

export type Arrangement = LinesArrangement | PluckArrangement;

// Every "<mode id>/<sound name>" a game mode declares.
export function allSoundKeys(): string[] {
    return GAME_MODES.flatMap((m) => m.sounds?.names.map((n) => `${m.id}/${n}`) ?? []);
}

// Keys are "<mode id>/<sound name>", e.g. "original/ending-champ".
export function loadArrangements(): Record<string, Arrangement> {
    const config = (parse(readFileSync(CONFIG_FILE, 'utf8')) ?? {}) as Record<string, Arrangement>;
    const keys = allSoundKeys();
    for (const [name, a] of Object.entries(config)) {
        const where = `${CONFIG_FILE}, "${name}"`;
        if (!keys.includes(name)) throw new Error(`${where}: no mode has that sound. Use one of:\n  ${keys.join('\n  ')}`);
        if (typeof a.source !== 'string') throw new Error(`${where}: needs "source"`);
        if (a.style !== 'lines' && a.style !== 'pluck') throw new Error(`${where}: "style" must be "lines" or "pluck"`);
        if (a.style === 'lines' && !(a.melody && a.middle && a.bass))
            throw new Error(`${where}: "lines" needs "melody", "middle" and "bass"`);
    }
    return config;
}
