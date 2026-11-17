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
import { startWeb } from './web.js';

console.log('[boot] pinball brain starting');

const link = new MegaLink(process.argv[2] ?? process.env.PINBALL_PORT ?? null);
const machine = new Machine(link);
link.start(machine);

const runner = new GameRunner(machine, GAME_MODES);
startWeb({ runner, machine, megaConnected: () => link.connected }, Number(process.env.PINBALL_HTTP_PORT ?? 8080));

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

await runner.run();
