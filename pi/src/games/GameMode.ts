// A way to play the machine. Add a new mode by implementing this.

import type { Machine } from '../Machine.js';

export interface GameMode {
    readonly name: string;

    // Plays one full cycle: attract mode, wait for start, play the game,
    // game over. Resolves when the machine is back to idle.
    run(m: Machine): Promise<void>;
}
