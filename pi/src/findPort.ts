import { setTimeout as delay } from 'node:timers/promises';
import { SerialPort } from 'serialport';

// USB vendor IDs: Arduino LLC, Arduino SRL, and the CH340 chip on clone boards.
const ARDUINO_VENDORS = ['2341', '2a03', '1a86'];

// The port given on the command line or in PINBALL_PORT, else the first Arduino plugged in.
export async function findMegaPort(): Promise<string> {
    const given = process.argv[2] ?? process.env.PINBALL_PORT;
    if (given) return given;

    const ports = await SerialPort.list();
    const mega = ports.find((p) => ARDUINO_VENDORS.includes(p.vendorId?.toLowerCase() ?? ''));
    if (mega) return mega.path;

    const seen = ports.map((p) => `  ${p.path} ${p.manufacturer ?? ''}`).join('\n') || '  (none)';
    throw new Error(`No Arduino found. Pass the port, e.g. /dev/ttyACM0. Serial ports:\n${seen}`);
}

// Like findMegaPort(), but keeps looking until the Mega is plugged in.
export async function waitForMegaPort(): Promise<string> {
    for (let first = true; ; first = false) {
        try {
            return await findMegaPort();
        } catch (err) {
            if (first) console.log(`[link] ${(err as Error).message}\n[link] waiting for the Mega...`);
            await delay(2000);
        }
    }
}
