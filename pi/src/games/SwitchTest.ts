// Shows each switch on the display as it is hit, to check the machine works.
// Slingshots and bumpers still fire, and a drained ball is kicked back out.

import { Autofire, type Machine } from '../Machine.js';
import { Coil, LAMP_COUNT, type LampId, Switch, switchName } from '../protocol/index.js';
import { SoundBank } from '../sounds/library.js';
import type { GameMode } from './GameMode.js';

export class SwitchTest implements GameMode {
    readonly id = 'switch-test';
    readonly name = 'Switch Test';
    readonly description = 'Hit any switch and its name scrolls on the display. For checking the machine.';
    readonly keepsHighScores = false;
    readonly sounds = new SoundBank('switch-test', ['hit'] as const); // a switch closed

    async run(m: Machine): Promise<void> {
        for (let i = 0; i < LAMP_COUNT; i++) m.setLamp(i as LampId, false);
        m.setAutofire(Autofire.ALL);
        m.scrollText('SWITCH TEST - HIT ANY SWITCH      ');

        for (;;) {
            const ev = await m.nextEvent(60_000);
            if (!ev?.pressed) continue;
            m.scrollText(`${ev.id} ${switchName(ev.id).replaceAll('_', ' ')}     `, 180);
            m.playSound(this.sounds.get('hit'), { priority: 'low' });
            if (ev.id === Switch.BALL_RETURN) m.pulseCoil(Coil.BALL_RETURN, 100);
        }
    }
}
