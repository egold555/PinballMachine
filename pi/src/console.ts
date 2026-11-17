// Test the Mega I/O board by hand, without running a game.
//
// Prints every switch event and lets you drive lamps, coils, the display and
// sound. Stop the game first, only one program can use the port at a time.
//
//   npm run console [port]

import { createInterface } from 'node:readline/promises';
import { setTimeout as delay } from 'node:timers/promises';
import { findMegaPort } from './findPort.js';
import { MegaLink } from './MegaLink.js';
import {
    bitPut,
    Coil,
    HEARTBEAT_MS,
    LAMP_BYTES,
    LAMP_COUNT,
    Msg,
    Switch,
    SOUND_CHANNELS,
    switchName,
} from './protocol/index.js';

const COIL_IDS = new Map<string, number>(Object.entries(Coil));

// Same rules as Machine.ts: (switch, coil)
const AUTOFIRE = [
    [Switch.LEFT_SLINGSHOT, Coil.SLINGSHOT_LEFT],
    [Switch.RIGHT_SLINGSHOT, Coil.SLINGSHOT_RIGHT],
    [Switch.LEFT_THUMPER_BUMPER, Coil.THUMPER_LEFT],
    [Switch.RIGHT_THUMPER_BUMPER, Coil.THUMPER_RIGHT],
];

const HELP = `
  lamp <0-${LAMP_COUNT - 1}|all> on|off     set a lamp (IDs in protocol/MachineMap.h)
  chase                      light each lamp in turn, to map them out
  pulse <coil> [ms]          fire a coil, default 30 ms (coils: 0-4, or ${[...COIL_IDS.keys()].join(', ')})
  autofire on|off            slingshots and thumpers fire on their own switches
  text <message>             show up to 8 characters on the display
  note <channel 0-${SOUND_CHANNELS - 1}> <midi>  play a note (60 = middle C);  "note off" stops all
  quit
`;

const lamps = new Uint8Array(LAMP_BYTES);
const link = new MegaLink(await findMegaPort());
link.start({
    onHello: (v) => console.log(`\n  Mega says hello (protocol v${v})`),
    onSwitch: (id, closed) =>
        console.log(`\n  switch ${String(id).padStart(2)} ${switchName(id).padEnd(22)} ${closed ? 'CLOSED' : 'open'}`),
    onSwitchState: () => {},
});
setInterval(() => link.send(Msg.LAMPS, lamps), HEARTBEAT_MS);

console.log('Waiting for the Mega to boot...');
await delay(2500);
console.log('Ready. Type "help".');

const rl = createInterface({ input: process.stdin, output: process.stdout });
rl.on('close', () => void link.close().then(() => process.exit(0)));

for (;;) {
    const parts = (await rl.question('> ')).trim().split(/\s+/);
    const [cmd, ...args] = [parts[0].toLowerCase(), ...parts.slice(1)];
    const num = (s: string | undefined) => {
        const n = Number(s);
        if (s === undefined || !Number.isInteger(n)) throw new Error('bad number');
        return n;
    };
    try {
        switch (cmd) {
            case '':
                break;
            case 'help':
                console.log(HELP);
                break;
            case 'quit':
                rl.close();
                break;
            case 'lamp': {
                const ids = args[0] === 'all' ? [...Array(LAMP_COUNT).keys()] : [num(args[0])];
                for (const i of ids) bitPut(lamps, i, args[1] === 'on');
                break;
            }
            case 'chase':
                for (let i = 0; i < LAMP_COUNT; i++) {
                    lamps.fill(0);
                    bitPut(lamps, i, true);
                    console.log(`  lamp ${i}`);
                    await delay(600);
                }
                lamps.fill(0);
                break;
            case 'pulse': {
                const coil = COIL_IDS.get(args[0]?.toUpperCase()) ?? num(args[0]);
                link.send(Msg.COIL_PULSE, [coil, args[1] ? num(args[1]) : 30]);
                break;
            }
            case 'autofire':
                AUTOFIRE.forEach(([sw, coil], i) => link.send(Msg.AUTOFIRE_RULE, [i, sw, coil, 50]));
                link.send(Msg.AUTOFIRE_ENABLE, [args[0] === 'on' ? 0x0f : 0]);
                break;
            case 'text': {
                const payload = Buffer.alloc(9);
                payload.write(args.join(' ').slice(0, 8).padEnd(8), 'latin1');
                link.send(Msg.DISPLAY_TEXT, payload);
                break;
            }
            case 'note':
                if (args[0] === 'off') link.send(Msg.NOTE_OFF, [0xff]);
                else link.send(Msg.NOTE_ON, [num(args[0]), num(args[1])]);
                break;
            default:
                console.log('  unknown command, type "help"');
        }
    } catch {
        console.log('  bad arguments, type "help"');
    }
}
