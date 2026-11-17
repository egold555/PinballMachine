// Every sound the game plays, loaded from .mid files in data/sounds/ when the
// program starts. To change a sound, replace its file and restart (there is
// a button on the web page).
//
// A sound with no file falls back to the built-in version from the old
// firmware, and a missing ending song falls back to the Wizard one, so an
// empty folder still gives a working game.

import { readdirSync, readFileSync } from 'node:fs';
import { join } from 'node:path';
import { DATA_DIR } from '../storage.js';
import { midiToScore } from './midi.js';
import * as builtIn from './scores.js';

export const SOUNDS_DIR = join(DATA_DIR, 'sounds');

// The file for each sound is <name>.mid
export const SOUND_NAMES = [
    'start-game',
    'point',
    'new-ball',
    'extra-ball',
    'tilt',
    'ending-amateur',
    'ending-super',
    'ending-champ',
    'ending-fantastic',
    'ending-wizard',
    'ending-fireball',
] as const;
export type SoundName = (typeof SOUND_NAMES)[number];

const BUILT_IN: Partial<Record<SoundName, Uint8Array>> = {
    'start-game': builtIn.SOUND_STARTUP,
    point: builtIn.SOUND_POINT,
    'new-ball': builtIn.SOUND_NEWBALL,
    'extra-ball': builtIn.SOUND_EXTRABALL,
    tilt: builtIn.SOUND_TILT,
    'ending-wizard': builtIn.SOUND_ENDING_SONG,
};
const ENDING_FALLBACK: SoundName = 'ending-wizard';

export interface SoundInfo {
    name: SoundName;
    source: 'file' | 'built-in' | 'missing';
    seconds: number;
    notes: number;
    dropped: number; // notes cut short to fit three voices
    error?: string; // the file is there but couldn't be read
}

const loaded = new Map<SoundName, Uint8Array>();
let info: SoundInfo[] = [];

export function loadSounds(dir = SOUNDS_DIR): void {
    loaded.clear();
    let files: string[] = [];
    try {
        files = readdirSync(dir);
    } catch {
        console.log(`[sound] no ${dir}, using the built-in sounds`);
    }

    info = SOUND_NAMES.map((name): SoundInfo => {
        const file = files.find((f) => f.toLowerCase() === `${name}.mid`);
        let error: string | undefined;
        if (file) {
            try {
                const midi = midiToScore(readFileSync(join(dir, file)));
                loaded.set(name, midi.score);
                return { name, source: 'file', seconds: midi.ms / 1000, notes: midi.notes, dropped: midi.dropped };
            } catch (err) {
                error = `${file}: ${(err as Error).message}`;
                console.log(`[sound] can't read ${error}`);
            }
        }
        const fallback = BUILT_IN[name];
        return fallback
            ? { name, source: 'built-in', ...scoreStats(fallback), dropped: 0, error }
            : { name, source: 'missing', seconds: 0, notes: 0, dropped: 0, error };
    });

    const fromFiles = info.filter((s) => s.source === 'file').length;
    console.log(`[sound] ${fromFiles} of ${SOUND_NAMES.length} sounds loaded from ${dir}`);
    for (const f of files)
        if (f.toLowerCase().endsWith('.mid') && !SOUND_NAMES.some((n) => f.toLowerCase() === `${n}.mid`))
            console.log(`[sound] ignoring ${f}: not one of the sound names`);
}

export function soundInfo(): SoundInfo[] {
    return info;
}

export function sound(name: SoundName): Uint8Array {
    return (
        loaded.get(name) ??
        BUILT_IN[name] ??
        (name.startsWith('ending-') ? sound(ENDING_FALLBACK) : builtIn.SOUND_POINT)
    );
}

// The ending song for a rank such as "FIREBALL".
export function endingSong(rank: string): Uint8Array {
    const name = `ending-${rank.toLowerCase()}`;
    return sound((SOUND_NAMES as readonly string[]).includes(name) ? (name as SoundName) : ENDING_FALLBACK);
}

// Length and note count of a built-in score.
function scoreStats(s: Uint8Array) {
    let ms = 0;
    let notes = 0;
    for (let i = 0; i < s.length; ) {
        const c = s[i++];
        if (c < 0x80) ms += (c << 8) | s[i++];
        else if ((c & 0xf0) === 0x90) (notes++, i++);
        else if ((c & 0xf0) === 0xf0) break;
    }
    return { seconds: ms / 1000, notes };
}
