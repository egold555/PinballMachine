// Small values that survive restarts (high scores and so on), kept in
// data/settings.json. Set PINBALL_DATA_DIR to keep them somewhere else.
//
// Writes go to a temp file that is then renamed over the old one, so pulling
// the plug mid-write leaves either the old file or the new one, never half.

import { mkdirSync, readFileSync, renameSync, writeFileSync } from 'node:fs';
import { join } from 'node:path';

const DIR = process.env.PINBALL_DATA_DIR ?? join(import.meta.dirname, '..', 'data');
const FILE = join(DIR, 'settings.json');

let values: Record<string, unknown> = {};
try {
    values = JSON.parse(readFileSync(FILE, 'utf8')) as Record<string, unknown>;
} catch {
    // first run, or unreadable: start empty
}

export function load<T>(key: string, fallback: T): T {
    return key in values ? (values[key] as T) : fallback;
}

export function save(key: string, value: unknown): void {
    values[key] = value;
    mkdirSync(DIR, { recursive: true });
    const tmp = `${FILE}.tmp`;
    writeFileSync(tmp, JSON.stringify(values, null, 2));
    renameSync(tmp, FILE);
}
