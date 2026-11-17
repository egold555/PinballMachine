// Serial protocol between the Pi (game logic) and the Mega (I/O board).
// Same wire format as protocol/Protocol.h:
//
//     COBS( [type] [payload ...] [crc8] )  0x00
//
// IDs and message types come from map.ts, which is generated from the C headers.

import { DISPLAY_CHARS, LAMP_COUNT, MAX_PAYLOAD, Switch, SWITCH_COUNT } from './map.js';

export * from './map.js';

export const SWITCH_BYTES = Math.ceil(SWITCH_COUNT / 8);
export const LAMP_BYTES = Math.ceil(LAMP_COUNT / 8);
export const DISPLAY_TEXT_BYTES = DISPLAY_CHARS + 1;

const MAX_RAW = MAX_PAYLOAD + 2; // type + payload + crc
const MAX_FRAME = MAX_RAW + 2; // + COBS overhead + 0x00 terminator

// Matrix positions with nothing wired to them: every ID below SWITCH_COUNT
// that has no name in MachineMap.h.
const NAMED_SWITCHES = new Set<number>(Object.values(Switch));
export function switchIsUnused(id: number): boolean {
    return !NAMED_SWITCHES.has(id);
}

// ------------------------------------------------------------------- Bitmaps

export function bitGet(bits: Uint8Array, i: number): boolean {
    return (bits[i >> 3] & (1 << (i & 7))) !== 0;
}

export function bitPut(bits: Uint8Array, i: number, on: boolean): void {
    if (on) bits[i >> 3] |= 1 << (i & 7);
    else bits[i >> 3] &= ~(1 << (i & 7));
}

// ------------------------------------------------------------------- Framing

export function crc8(data: Uint8Array): number {
    let crc = 0;
    for (const b of data) {
        crc ^= b;
        for (let i = 0; i < 8; i++) crc = crc & 0x80 ? ((crc << 1) ^ 0x07) & 0xff : (crc << 1) & 0xff;
    }
    return crc;
}

// Builds a complete frame, ready to write to the port.
export function encodeFrame(type: number, payload: ArrayLike<number> = []): Buffer {
    if (payload.length > MAX_PAYLOAD) throw new Error(`payload too big (${payload.length} bytes)`);

    const raw = new Uint8Array(payload.length + 2);
    raw[0] = type;
    raw.set(payload, 1);
    raw[raw.length - 1] = crc8(raw.subarray(0, raw.length - 1));

    // COBS: each 0x00 is replaced by the distance to the next one.
    const out = Buffer.alloc(raw.length + 2);
    let codeAt = 0;
    let o = 1;
    let code = 1;
    for (const b of raw) {
        if (b === 0) {
            out[codeAt] = code;
            codeAt = o++;
            code = 1;
        } else {
            out[o++] = b;
            code++;
        }
    }
    out[codeAt] = code;
    out[o++] = 0;
    return out.subarray(0, o);
}

export interface Frame {
    type: number;
    payload: Uint8Array;
}

// Feed it received bytes. Returns a frame each time a valid one completes.
export class FrameDecoder {
    private buf = new Uint8Array(MAX_FRAME);
    private n = 0;
    private overflow = false;

    feed(b: number): Frame | null {
        if (b === 0) {
            const frame = this.overflow ? null : this.decode();
            this.n = 0;
            this.overflow = false;
            return frame;
        }
        if (this.n >= this.buf.length) this.overflow = true;
        else this.buf[this.n++] = b;
        return null;
    }

    private decode(): Frame | null {
        const raw = new Uint8Array(MAX_FRAME);
        let i = 0;
        let o = 0;
        while (i < this.n) {
            const code = this.buf[i++];
            if (code === 0) return null;
            for (let k = 1; k < code; k++) {
                if (i >= this.n) return null;
                raw[o++] = this.buf[i++];
            }
            if (i < this.n) raw[o++] = 0;
        }
        if (o < 2 || crc8(raw.subarray(0, o - 1)) !== raw[o - 1]) return null;
        return { type: raw[0], payload: raw.slice(1, o - 1) };
    }
}
