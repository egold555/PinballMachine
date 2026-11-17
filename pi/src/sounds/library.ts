// Every game mode has its own sounds: .mid files in data/sounds/<mode id>/,
// named after the sounds the mode declares. They are loaded when the program
// starts; to change one, replace its file and restart (there is a button on
// the web page).
//
// A sound with no file is silent, unless the mode gives a built-in fallback
// (the Original game has the sounds from the old firmware).

import { readdirSync, readFileSync } from 'node:fs';
import { join } from 'node:path';
import { DATA_DIR } from '../storage.js';
import { midiToScore } from './midi.js';

export const SOUNDS_DIR = join(DATA_DIR, 'sounds');

export interface SoundInfo {
    mode: string; // mode id, also the folder name
    name: string;
    source: 'file' | 'built-in' | 'missing';
    seconds: number;
    notes: number;
    dropped: number; // notes cut short to fit three voices
    error?: string; // the file is there but couldn't be read
}

const banks: SoundBank<string>[] = [];

// One mode's sounds. Create it once per mode, with every sound the mode plays.
export class SoundBank<N extends string> {
    private loaded = new Map<N, Uint8Array>();
    private bpms = new Map<N, number>();
    private status: SoundInfo[] = [];

    constructor(
        readonly modeId: string,
        readonly names: readonly N[],
        private readonly builtIn: Partial<Record<N, Uint8Array>> = {},
    ) {
        banks.push(this as SoundBank<string>);
    }

    // The sound, or null if it has no file and no built-in version.
    get(name: N): Uint8Array | null {
        return this.loaded.get(name) ?? this.builtIn[name] ?? null;
    }

    // The tempo a sound's file starts at, or null if it has no file.
    bpm(name: N): number | null {
        return this.bpms.get(name) ?? null;
    }

    info(): SoundInfo[] {
        return this.status;
    }

    load(root: string): void {
        this.loaded.clear();
        this.bpms.clear();
        const dir = join(root, this.modeId);
        let files: string[] = [];
        try {
            files = readdirSync(dir);
        } catch {
            // no folder for this mode yet: every sound falls back
        }

        this.status = this.names.map((name): SoundInfo => {
            const file = files.find((f) => f.toLowerCase() === `${name}.mid`);
            let error: string | undefined;
            if (file) {
                try {
                    const midi = midiToScore(readFileSync(join(dir, file)));
                    this.loaded.set(name, midi.score);
                    this.bpms.set(name, midi.bpm);
                    const seconds = midi.ms / 1000;
                    return { mode: this.modeId, name, source: 'file', seconds, notes: midi.notes, dropped: midi.dropped };
                } catch (err) {
                    error = `${this.modeId}/${file}: ${(err as Error).message}`;
                    console.log(`[sound] can't read ${error}`);
                }
            }
            const fallback = this.builtIn[name];
            return fallback
                ? { mode: this.modeId, name, source: 'built-in', ...scoreStats(fallback), dropped: 0, error }
                : { mode: this.modeId, name, source: 'missing', seconds: 0, notes: 0, dropped: 0, error };
        });

        for (const f of files)
            if (f.toLowerCase().endsWith('.mid') && !this.names.some((n) => f.toLowerCase() === `${n}.mid`))
                console.log(`[sound] ignoring ${this.modeId}/${f}: ${this.modeId} has no sound by that name`);
    }
}

// Every mode's sound bank, in the order the modes were created.
export function soundBanks(): SoundBank<string>[] {
    return banks;
}

// (Re)reads every mode's sound files.
export function loadSounds(root = SOUNDS_DIR): void {
    for (const bank of banks) bank.load(root);
    const all = banks.flatMap((b) => b.info());
    console.log(`[sound] ${all.filter((s) => s.source === 'file').length} of ${all.length} sounds loaded from ${root}`);
}

export function soundInfo(): SoundInfo[] {
    return banks.flatMap((b) => b.info());
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
