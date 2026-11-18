// Runs the selected game mode over and over, and switches to another one on
// request (from the web page). The choice is remembered across restarts.

import { setTimeout as delay } from 'node:timers/promises';
import type { GameMode } from './games/GameMode.js';
import { GameStopped, type Machine } from './Machine.js';
import { powerFor } from './power.js';
import { load, save } from './storage.js';

const MODE_KEY = 'mode';

export class GameRunner {
    private current: GameMode;

    constructor(
        private readonly m: Machine,
        readonly modes: GameMode[],
    ) {
        this.current = modes.find((mode) => mode.id === load(MODE_KEY, '')) ?? modes[0];
        this.watchLink();
    }

    // Unplugging the Mega abandons the game in progress: the mode starts
    // over, at its "press start" screen, ready for when it's plugged back in.
    private watchLink(): void {
        this.m.whenDisconnected = () => {
            console.log(`[game] Mega gone, ${this.current.name} starts over`);
            this.m.stopGame();
        };
    }

    get currentMode(): GameMode {
        return this.current;
    }

    // Ends whatever is running and starts this mode from the beginning.
    // Selecting the mode that is already running restarts it.
    select(id: string): boolean {
        const mode = this.modes.find((m) => m.id === id);
        if (!mode) return false;
        console.log(`[game] switching to ${mode.name}`);
        this.current = mode;
        save(MODE_KEY, id);
        this.m.stopGame();
        return true;
    }

    // Sends the running mode's bumper and slingshot power to the Mega. Called
    // when a mode starts and when the power settings change.
    applyPower(): void {
        this.m.setAutofireMs(powerFor(this.current.id));
    }

    async run(): Promise<never> {
        for (;;) {
            const mode = this.current;
            console.log(`[game] ${mode.name}`);
            this.applyPower();
            try {
                await mode.run(this.m);
            } catch (err) {
                if (!(err instanceof GameStopped)) {
                    console.error('[game] crashed, starting over:', err);
                    await delay(1000);
                }
            }
            this.m.resumeGame();
        }
    }
}
