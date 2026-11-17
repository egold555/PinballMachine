// A way to play the machine. Add a new mode by implementing this and listing
// it in games/index.ts.

import type { Machine } from '../Machine.js';

export interface GameMode {
    readonly id: string; // stable, used for saved high scores and settings
    readonly name: string;
    readonly description: string;
    readonly keepsHighScores: boolean;

    // Plays one full cycle: attract mode, wait for start, play the game,
    // game over. Resolves when the machine is back to idle.
    run(m: Machine): Promise<void>;
}
