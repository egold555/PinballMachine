import assert from 'node:assert/strict';
import { test } from 'node:test';
import { crc8, encodeFrame, FrameDecoder, switchIsUnused } from '../src/protocol/index.js';

function decodeAll(bytes: Iterable<number>) {
    const d = new FrameDecoder();
    const frames = [];
    for (const b of bytes) {
        const f = d.feed(b);
        if (f) frames.push({ type: f.type, payload: [...f.payload] });
    }
    return frames;
}

test('frames survive a round trip, zeros included', () => {
    for (const payload of [[], [0], [0, 0, 0], [1, 2, 3], Array.from({ length: 24 }, (_, i) => i % 3)]) {
        const frame = encodeFrame(0x11, payload);
        assert.equal(frame.indexOf(0), frame.length - 1, 'only the terminator is zero');
        assert.deepEqual(decodeAll(frame), [{ type: 0x11, payload }]);
    }
});

// Bytes produced by protocol/Protocol.h (encodeFrame in C++) for the same input.
test('matches the C++ encoder', () => {
    assert.deepEqual([...encodeFrame(0x12, [1, 100])], [0x05, 0x12, 0x01, 0x64, 0x5a, 0x00]);
    assert.deepEqual([...encodeFrame(0x18, [0xff])], [0x04, 0x18, 0xff, 0x0c, 0x00]);
    assert.deepEqual([...encodeFrame(0x14, [0])], [0x02, 0x14, 0x02, 0x03, 0x00]);
});

test('drops corrupted frames and resyncs on the next one', () => {
    const bad = [...encodeFrame(0x02, [5, 1])];
    bad[2] ^= 0xff;
    const good = [...encodeFrame(0x02, [7, 0])];
    assert.deepEqual(decodeAll([0x33, 0x44, ...bad, ...good]), [{ type: 0x02, payload: [7, 0] }]);
});

test('crc8 is polynomial 0x07', () => {
    assert.equal(crc8(new TextEncoder().encode('123456789')), 0xf4);
});

test('unwired matrix positions', () => {
    assert.ok(switchIsUnused(17) && switchIsUnused(21));
    assert.ok(!switchIsUnused(16) && !switchIsUnused(25));
});
