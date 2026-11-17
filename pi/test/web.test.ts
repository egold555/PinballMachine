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
const { recordScores } = await import('../src/highScores.js');
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
    assert.deepEqual(s.modes.map((m: { id: string }) => m.id), ['original', 'switch-test', 'sound-test', 'light-test']);
    assert.deepEqual(s.modes[0].highScores, [{ score: 18950, date: '' }]);
    assert.equal(s.modes[1].highScores, null, 'switch test keeps no scores');
});

test('high score list stays sorted and keeps the top 10', () => {
    assert.equal(recordScores('original', [500, 20000]), true);
    assert.equal(recordScores('original', [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]), false);
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
    assert.equal(s.modes[0].highScores[0].score, 20000);
    assert.equal(s.modes[0].highScores.length, 10);
});
