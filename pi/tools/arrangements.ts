// Reads Midis/arrangements.yaml: for each sound, which source MIDI to use
// and how to fit it onto the machine's three voices. See Midis/README.md.

import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import { parse } from 'yaml';
import { SOUND_NAMES } from '../src/sounds/library.js';

export const MIDIS_DIR = join(import.meta.dirname, '..', '..', 'Midis');
export const CONFIG_FILE = join(MIDIS_DIR, 'arrangements.yaml');

export interface Part {
    track?: number;
    channel?: number; // 1-16, like in MIDI editors
}

interface Common {
    source: string; // relative to Midis/
    startAt?: number; // seconds of the source to skip
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

export function loadArrangements(): Record<string, Arrangement> {
    const config = (parse(readFileSync(CONFIG_FILE, 'utf8')) ?? {}) as Record<string, Arrangement>;
    for (const [name, a] of Object.entries(config)) {
        const where = `${CONFIG_FILE}, "${name}"`;
        if (!(SOUND_NAMES as readonly string[]).includes(name))
            throw new Error(`${where}: not a sound name. Use one of: ${SOUND_NAMES.join(', ')}`);
        if (typeof a.source !== 'string') throw new Error(`${where}: needs "source"`);
        if (a.style !== 'lines' && a.style !== 'pluck') throw new Error(`${where}: "style" must be "lines" or "pluck"`);
        if (a.style === 'lines' && !(a.melody && a.middle && a.bass))
            throw new Error(`${where}: "lines" needs "melody", "middle" and "bass"`);
    }
    return config;
}
