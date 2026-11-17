// Pinball machine brain for a Raspberry Pi.
//
// All game logic lives here. The Mega is only an I/O board on the USB port:
// it reports switches and does what this side tells it. See ../README.md.
//
//   npm run dev [port]     run from source (port is found automatically)
//   npm run build && npm start
//
// The web page is on port 8080, or PINBALL_HTTP_PORT.

import { GameRunner } from './GameRunner.js';
import { GAME_MODES } from './games/index.js';
import { Machine } from './Machine.js';
import { MegaLink } from './MegaLink.js';
import { loadSounds } from './sounds/library.js';
import { startWeb } from './web.js';

console.log('[boot] pinball brain starting');
loadSounds();

const link = new MegaLink(process.argv[2] ?? process.env.PINBALL_PORT ?? null);
const machine = new Machine(link);
link.start(machine);

const runner = new GameRunner(machine, GAME_MODES);
startWeb(
    { runner, machine, megaConnected: () => link.connected, restart: () => void shutdown('RESTART') },
    Number(process.env.PINBALL_HTTP_PORT ?? 8080),
);

// Stopping the program (systemctl stop, Ctrl+C) turns everything off first.
// "UPDATING" stays on the display through a Mega firmware upload, which is
// the usual reason to stop it. Otherwise the Mega shows "NO ESP" a second later.
// The service starts the program again whenever it exits, which is how the
// restart button works (with `npm run dev` it just stops).
let stopping = false;
async function shutdown(message: string): Promise<void> {
    if (stopping) return;
    stopping = true;
    console.log('[boot] stopping');
    machine.lockOut(message);
    await link.close();
    process.exit(0);
}
process.once('SIGTERM', () => void shutdown('UPDATING'));
process.once('SIGINT', () => void shutdown('UPDATING'));

await runner.run();
