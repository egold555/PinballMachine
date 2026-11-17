// Pinball machine brain for a Raspberry Pi.
//
// All game logic lives here. The Mega is only an I/O board on the USB port:
// it reports switches and does what this side tells it. See ../README.md.
//
//   npm run dev [port]     run from source (port is found automatically)
//   npm run build && npm start

import { setTimeout as delay } from 'node:timers/promises';
import { waitForMegaPort } from './findPort.js';
import type { GameMode } from './games/GameMode.js';
import { OriginalGame } from './games/OriginalGame.js';
import { Machine } from './Machine.js';
import { MegaLink } from './MegaLink.js';

console.log('[boot] pinball brain starting');

const link = new MegaLink(await waitForMegaPort());
const machine = new Machine(link);
link.start(machine);

// Stopping the program (systemctl stop, Ctrl+C) turns everything off first.
// The text stays on the display through a Mega firmware upload, which is
// the usual reason to stop it. Otherwise the Mega shows "NO ESP" a second later.
async function shutdown(): Promise<void> {
    console.log('[boot] stopping');
    machine.lockOut('UPDATING');
    await link.close();
    process.exit(0);
}
process.once('SIGTERM', () => void shutdown());
process.once('SIGINT', () => void shutdown());

const mode: GameMode = new OriginalGame();
for (;;) {
    console.log(`[game] ${mode.name}`);
    try {
        await mode.run(machine);
    } catch (err) {
        console.error('[game] crashed, starting over:', err);
        await delay(1000);
    }
}
