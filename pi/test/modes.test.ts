// Sound Test and Light Test, driven with fake flipper and start presses.

import assert from 'node:assert/strict';
import { mkdtempSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { test } from 'node:test';
import { setTimeout as delay } from 'node:timers/promises';

process.env.PINBALL_DATA_DIR = mkdtempSync(join(tmpdir(), 'pinball-test-'));

const { Machine } = await import('../src/Machine.js');
await import('../src/games/index.js'); // creates every mode's sound bank
const { SoundTest } = await import('../src/games/SoundTest.js');
const { LightTest } = await import('../src/games/LightTest.js');
const { Frenzy } = await import('../src/games/Frenzy.js');
const { scores } = await import('../src/highScores.js');
const { bitGet, Coil, Lamp, Msg, Switch } = await import('../src/protocol/index.js');
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

test('sound test: every mode\'s sounds, start plays and stops', async () => {
    const { m, sent, press } = setup();
    void new SoundTest().run(m).catch(() => {});
    await delay(50);
    // The data folder is empty here: Original falls back to its built-in
    // sounds, Switch Test has none, so its sound is silent.
    // Checked right after each press: the name scrolls a letter every 180 ms.
    assert.match(m.displayText, /1 ORIGIN/, 'starts at original start-game');
    await press(Switch.FLIPPER_RIGHT);
    assert.match(m.displayText, /2 ORIGIN/);
    await press(Switch.FLIPPER_LEFT);
    await press(Switch.FLIPPER_LEFT); // wraps around to the last one
    assert.match(m.displayText, /19 SWITC/, 'switch-test hit');

    await press(Switch.START);
    assert.ok(!m.soundPlaying, 'no file: nothing plays');

    for (let i = 0; i < 7; i++) await press(Switch.FLIPPER_LEFT); // frenzy's seven: no files here
    await press(Switch.FLIPPER_LEFT); // original ending-fireball: no file, no built-in
    await press(Switch.FLIPPER_LEFT); // original ending-wizard: built-in
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

test('frenzy: countdown, kick, rolling score, jackpot, drains come back, time up', async () => {
    const { m, sent, press } = setup();
    const kicks = () => sent.filter((f) => f.type === Msg.COIL_PULSE && f.payload[0] === Coil.BALL_RETURN).length;
    // No music file here, so the countdown runs at 135 bpm: a beat is 444 ms.
    // FRENZY for 4 beats (1.78 s), each number 2 beats, GO after 14 (6.22 s).
    void new Frenzy(6000).run(m).catch(() => {});
    await delay(50);
    assert.match(m.displayText, /FRENZY/);

    await press(Switch.START);
    await delay(2000);
    assert.equal(m.displayText.trim(), '5', 'countdown');
    assert.equal(kicks(), 0, 'no ball before go');
    await delay(4250);
    assert.equal(kicks(), 1, 'ball kicked out at go');
    assert.equal(m.displayText.trim(), 'GO');
    await delay(1100);
    assert.equal(m.displayText.trim(), '0', 'then the score, never the time');

    await press(Switch.TILT);
    assert.equal(m.displayText.trim(), '0', 'tilt does nothing');

    m.onSwitch(Switch.A, true);
    await delay(40);
    const rolling = Number(m.displayText.trim());
    assert.ok(rolling > 0 && rolling < 10_000, `counts up (${rolling})`);
    await delay(1000); // it gets there in about 0.4 s (more on a busy computer)
    assert.equal(m.displayText.trim(), '10000');
    m.onSwitch(Switch.A, false);

    for (const s of [Switch.B, Switch.C, Switch.D]) await press(s);
    assert.equal(m.displayText.trim(), 'JACKPOT', 'A B C D collected');
    await delay(1700); // JACKPOT for 0.9 s, then it rolls up
    assert.equal(m.displayText.trim(), String(40_000 + 100_000));

    m.onSwitch(Switch.BALL_RETURN, true); // drained
    await delay(60);
    assert.equal(kicks(), 2, 'straight back out');
    m.onSwitch(Switch.BALL_RETURN, false);

    await delay(2300); // about 4.1 s of play so far; time is up at 6, TIME UP stays 2.5
    assert.equal(m.displayText.trim(), 'TIME UP');
    assert.equal(scores('frenzy').at(-1)?.score, 140_000);
    m.stopGame();
});

