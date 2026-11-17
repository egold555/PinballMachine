// USB serial link to the Mega I/O board.
//
// Opening the port resets the Mega (the same as uploading a sketch). Nothing
// is sent until it has booted: bytes that arrive while its bootloader is
// running can keep it stuck in the bootloader. If the USB cable is pulled the
// link keeps trying to reopen it.

import { SerialPort } from 'serialport';
import { waitForMegaPort } from './findPort.js';
import {
    BAUD_RATE,
    encodeFrame,
    FrameDecoder,
    LINK_TIMEOUT_MS,
    Msg,
    PROTOCOL_VERSION,
    SWITCH_BYTES,
    SWITCH_COUNT,
} from './protocol/index.js';

export interface LinkListener {
    onHello(version: number): void;
    onSwitch(id: number, closed: boolean): void;
    onSwitchState(bits: Uint8Array): void;
}

// The Mega's bootloader runs for about a second after a reset.
const BOOT_WAIT_MS = 2500;
const REOPEN_MS = 2000;

export class MegaLink {
    private port: SerialPort | null = null;
    private decoder = new FrameDecoder();
    private listener: LinkListener | null = null;
    private canSend = false;
    private bootTimer: NodeJS.Timeout | null = null;
    private lastRx = -Infinity;
    private closing = false;

    // With no path, it looks for the Mega every time it (re)opens the port.
    constructor(private readonly fixedPath: string | null = null) {}

    start(listener: LinkListener): void {
        this.listener = listener;
        void this.open();
    }

    send(type: number, payload: ArrayLike<number> = []): void {
        if (!this.port?.isOpen || !this.canSend) return; // the Mega gets everything again on hello
        this.port.write(encodeFrame(type, payload));
    }

    // True if the Mega has said anything recently.
    get connected(): boolean {
        return performance.now() - this.lastRx < LINK_TIMEOUT_MS;
    }

    // Sends anything still queued, then closes the port for good.
    async close(): Promise<void> {
        this.closing = true;
        const port = this.port;
        if (!port?.isOpen) return;
        await new Promise<void>((resolve) => port.drain(() => resolve()));
        await new Promise<void>((resolve) => port.close(() => resolve()));
    }

    private async open(): Promise<void> {
        this.canSend = false;
        this.decoder = new FrameDecoder();
        const path = this.fixedPath ?? (await waitForMegaPort());
        const port = new SerialPort({ path, baudRate: BAUD_RATE, autoOpen: false });

        port.on('data', (data: Buffer) => {
            for (const b of data) {
                const frame = this.decoder.feed(b);
                if (frame) this.handleFrame(frame.type, frame.payload);
            }
        });
        port.on('close', () => {
            if (this.bootTimer) clearTimeout(this.bootTimer);
            if (this.closing) return;
            console.log('[link] port closed, retrying');
            setTimeout(() => void this.open(), REOPEN_MS);
        });
        port.on('error', (err) => console.log(`[link] ${err.message}`));

        port.open((err) => {
            if (err) {
                console.log(`[link] can't open ${path}: ${err.message}`);
                setTimeout(() => void this.open(), REOPEN_MS);
                return;
            }
            console.log(`[link] opened ${path}, waiting for the Mega to boot`);
            this.port = port;
            // The Mega says hello on its own once it boots. If the port didn't
            // reset it (it was already running), ask instead.
            this.bootTimer = setTimeout(() => this.allowSending(), BOOT_WAIT_MS);
        });
    }

    private allowSending(): void {
        if (this.bootTimer) clearTimeout(this.bootTimer);
        this.bootTimer = null;
        if (this.canSend) return;
        this.canSend = true;
        this.send(Msg.HELLO, [PROTOCOL_VERSION]);
    }

    private handleFrame(type: number, p: Uint8Array): void {
        this.lastRx = performance.now();
        this.allowSending();

        switch (type) {
            case Msg.HELLO:
                if (p.length === 1) {
                    if (p[0] !== PROTOCOL_VERSION)
                        console.log(
                            `[link] WARNING: Mega speaks protocol v${p[0]}, Pi speaks v${PROTOCOL_VERSION}. Rebuild both.`,
                        );
                    this.listener?.onHello(p[0]);
                }
                break;
            case Msg.SWITCH:
                if (p.length === 2 && p[0] < SWITCH_COUNT) this.listener?.onSwitch(p[0], p[1] !== 0);
                break;
            case Msg.SWITCH_STATE:
                if (p.length === SWITCH_BYTES) this.listener?.onSwitchState(p);
                break;
            case Msg.LOG:
                console.log(`[mega] ${Buffer.from(p).toString('latin1')}`);
                break;
        }
    }
}
