// Plays a little of the original game against a fake Mega.

import assert from 'node:assert/strict';
import { mkdtempSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { test } from 'node:test';
import { setTimeout as delay } from 'node:timers/promises';

process.env.PINBALL_DATA_DIR = mkdtempSync(join(tmpdir(), 'pinball-test-'));

const { Machine } = await import('../src/Machine.js');
const { OriginalGame } = await import('../src/games/OriginalGame.js');
const { bitGet, Coil, Lamp, Msg, Switch } = await import('../src/protocol/index.js');
type MegaLink = import('../src/MegaLink.js').MegaLink;

class FakeMega {
    sent: { type: number; payload: number[] }[] = [];
    send(type: number, payload: ArrayLike<number> = []) {
        this.sent.push({ type, payload: Array.from(payload) });
    }
    last(type: number) {
        return this.sent.findLast((f) => f.type === type)?.payload;
    }
    get display() {
        return String.fromCharCode(...(this.last(Msg.DISPLAY_TEXT) ?? []).slice(0, 8));
    }
    lamp(id: number) {
        return bitGet(Uint8Array.from(this.last(Msg.LAMPS) ?? []), id);
    }
}

test('start a game, kick, score, drain', async () => {
    const mega = new FakeMega();
    const m = new Machine(mega as unknown as MegaLink);
    m.onHello();
    const press = async (id: number) => {
        m.onSwitch(id, true);
        m.onSwitch(id, false);
        await delay(30);
    };

    void new OriginalGame().run(m);
    await delay(300);
    assert.equal(mega.last(Msg.AUTOFIRE_ENABLE)?.[0], 0, 'autofire off before the game');

    await press(Switch.START);
    assert.ok(mega.lamp(Lamp.PLAYER_1), 'player 1 lit');
    assert.ok(mega.lamp(Lamp.BALL_1), 'ball 1 lit');
    assert.equal(mega.last(Msg.AUTOFIRE_ENABLE)?.[0], 0x0f, 'autofire on');
    assert.equal(mega.display, '       0');

    // Ball in the outhole before any score: it never reached the playfield, kick it out
    m.onSwitch(Switch.BALL_RETURN, true);
    await delay(50);
    assert.deepEqual(mega.last(Msg.COIL_PULSE), [Coil.BALL_RETURN, 100]);
    m.onSwitch(Switch.BALL_RETURN, false);
    await delay(30);

    await press(Switch.A);
    assert.equal(mega.display, '    1000');
    assert.ok(!mega.lamp(Lamp.A), 'A collected');

    await press(Switch.CENTER_TARGET);
    assert.equal(mega.display, '    2000', 'lit center target is worth 1000');

    // Draining after scoring ends the ball: targets reset, bonus lamps off
    await press(Switch.BALL_RETURN);
    assert.ok(mega.lamp(Lamp.TARGET_CENTER), 'targets relit');
    assert.ok(!mega.lamp(Lamp.BONUS_1000), 'bonus cleared');
});
