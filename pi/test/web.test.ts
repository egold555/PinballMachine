// The web API and switching game modes, against a fake Mega.

import assert from 'node:assert/strict';
import { mkdtempSync, writeFileSync } from 'node:fs';
import type { AddressInfo } from 'node:net';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { test } from 'node:test';
import { setTimeout as delay } from 'node:timers/promises';

// A settings file from before the high score list existed
const dataDir = mkdtempSync(join(tmpdir(), 'pinball-test-'));
writeFileSync(join(dataDir, 'settings.json'), JSON.stringify({ 'original.highScore': 18950 }));
process.env.PINBALL_DATA_DIR = dataDir;

const { Machine } = await import('../src/Machine.js');
const { GameRunner } = await import('../src/GameRunner.js');
const { GAME_MODES } = await import('../src/games/index.js');
const { recordGame } = await import('../src/highScores.js');
const { startWeb } = await import('../src/web.js');
const { Switch } = await import('../src/protocol/index.js');
type MegaLink = import('../src/MegaLink.js').MegaLink;

const fakeLink = { send() {} } as unknown as MegaLink;
const machine = new Machine(fakeLink);
const runner = new GameRunner(machine, GAME_MODES);
void runner.run();
const server = startWeb({ runner, machine, megaConnected: () => true, restart: () => {} }, 0);
await new Promise((r) => server.once('listening', r));
const base = `http://localhost:${(server.address() as AddressInfo).port}`;

const state = async () => (await fetch(`${base}/api/state`)).json();
const selectMode = (id: string) =>
    fetch(`${base}/api/mode`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ id }),
    });

test('state lists the modes and carries over the old high score', async () => {
    const s = await state();
    assert.equal(s.currentMode, 'original');
    assert.deepEqual(s.modes.map((m: { id: string }) => m.id), ['original', 'frenzy', 'sudden-death', 'switch-test', 'sound-test', 'light-test']);
    assert.deepEqual(s.modes[0].scores, [{ score: 18950, date: '' }]);
    assert.equal(s.modes.find((m: { id: string }) => m.id === 'switch-test').scores, null, 'switch test keeps no scores');
});

test('every game is saved, and a new best is reported', () => {
    assert.equal(recordGame('original', 500), false, 'not a new best');
    assert.equal(recordGame('original', 20000), true, 'new best');
    for (let i = 1; i <= 12; i++) recordGame('original', i); // more than the old top 10
});

test('switching modes ends the running game and starts the new one', async () => {
    await delay(100);
    machine.onSwitch(Switch.START, true); // a game of Original is under way
    await delay(50);

    assert.equal((await selectMode('switch-test')).status, 200);
    await delay(100);
    assert.equal((await state()).currentMode, 'switch-test');

    machine.onSwitch(Switch.LEFT_SLINGSHOT, true);
    await delay(400);
    assert.match(machine.displayText, /SLING|LEFT|7/, 'switch test shows the switch');

    assert.equal((await selectMode('nope')).status, 404);
    assert.equal((await selectMode('original')).status, 200);
    await delay(100);
    const s = await state();
    assert.equal(s.currentMode, 'original');
    const list = s.modes[0].scores;
    assert.equal(list.length, 15, 'every game kept: the old one, 500, 20000 and twelve more');
    assert.deepEqual(list.slice(0, 3).map((g: { score: number }) => g.score), [18950, 500, 20000], 'oldest first');
    assert.ok(list.slice(1).every((g: { date: string }) => g.date), 'new games have dates');
});

test('power: a default, and each mode can have its own', async () => {
    const set = (body: object) =>
        fetch(`${base}/api/power`, { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(body) });
    const power = async () => (await state()).power;

    let p = await power();
    assert.equal(p.default, 50, 'starts at the original 50 ms');
    assert.deepEqual(p.modes.frenzy, { own: null, uses: 50 });

    await set({ mode: 'frenzy', ms: 65 });
    await set({ ms: 55 });
    p = await power();
    assert.deepEqual(p.modes.frenzy, { own: 65, uses: 65 }, 'frenzy keeps its own');
    assert.deepEqual(p.modes.original, { own: null, uses: 55 }, 'original follows the default');

    await set({ mode: 'frenzy', ms: null });
    assert.deepEqual((await power()).modes.frenzy, { own: null, uses: 55 }, 'back to the default');

    await set({ ms: 500 });
    assert.equal((await power()).default, 100, 'capped');
    assert.equal((await set({ ms: 'lots' })).status, 400);
    assert.equal((await set({ mode: 'nope', ms: 60 })).status, 404);
});
