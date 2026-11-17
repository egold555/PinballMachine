// Sound Test and Light Test, driven with fake flipper and start presses.

import assert from 'node:assert/strict';
import { mkdtempSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { test } from 'node:test';
import { setTimeout as delay } from 'node:timers/promises';

process.env.PINBALL_DATA_DIR = mkdtempSync(join(tmpdir(), 'pinball-test-'));

const { Machine } = await import('../src/Machine.js');
const { SoundTest } = await import('../src/games/SoundTest.js');
const { LightTest } = await import('../src/games/LightTest.js');
const { bitGet, Lamp, Msg, Switch } = await import('../src/protocol/index.js');
type MegaLink = import('../src/MegaLink.js').MegaLink;

function setup() {
    const sent: { type: number; payload: number[] }[] = [];
    const link = { send: (type: number, payload: ArrayLike<number> = []) => void sent.push({ type, payload: [...payload] }) };
    const m = new Machine(link as unknown as MegaLink);
    const press = async (id: number) => {
        m.onSwitch(id, true);
        m.onSwitch(id, false);
        await delay(60);
    };
    const lamp = (id: number) => bitGet(Uint8Array.from(sent.findLast((f) => f.type === Msg.LAMPS)?.payload ?? []), id);
    return { m, sent, press, lamp };
}

test('sound test: flippers pick, start plays and stops', async () => {
    const { m, sent, press } = setup();
    void new SoundTest().run(m).catch(() => {});
    await delay(50);
    assert.match(m.displayText, /1 START/);

    // Checked right after each press: the name scrolls a letter every 180 ms.
    await press(Switch.FLIPPER_RIGHT);
    assert.match(m.displayText, /2 POINT/);
    await press(Switch.FLIPPER_LEFT);
    await press(Switch.FLIPPER_LEFT); // wraps around to the last one
    assert.match(m.displayText, /11 ENDI/);

    sent.length = 0;
    await press(Switch.START);
    assert.ok(m.soundPlaying);
    assert.ok(sent.some((f) => f.type === Msg.NOTE_ON), 'notes sent');
    await press(Switch.START);
    assert.ok(!m.soundPlaying, 'start again stops it');
    m.stopGame();
});

test('light test: flippers pick, start toggles the lamp', async () => {
    const { m, press, lamp } = setup();
    void new LightTest().run(m).catch(() => {});
    await delay(50);
    assert.match(m.displayText, /1 BONUS/, 'starts at the lowest lamp ID');

    await press(Switch.START);
    await delay(20);
    assert.ok(lamp(Lamp.BONUS_8000), 'on');
    await press(Switch.START);
    await delay(20);
    assert.ok(!lamp(Lamp.BONUS_8000), 'off again');

    await press(Switch.FLIPPER_RIGHT); // BONUS_9000, blinks twice and ends off
    await delay(700);
    assert.ok(!lamp(Lamp.BONUS_9000), 'blink ends where it started');
    m.stopGame();
});
