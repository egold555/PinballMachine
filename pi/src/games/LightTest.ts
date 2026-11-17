// Turns any light on or off: the flippers pick one, start toggles it. The
// display scrolls its name and state, and the picked light blinks twice so
// it can be found on the playfield. Coils stay off.

import { type Machine } from '../Machine.js';
import { Lamp, LAMP_COUNT, type LampId, Switch } from '../protocol/index.js';
import type { GameMode } from './GameMode.js';

const LAMPS = (Object.entries(Lamp) as [string, LampId][]).sort((a, b) => a[1] - b[1]);

export class LightTest implements GameMode {
    readonly id = 'light-test';
    readonly name = 'Light Test';
    readonly description = 'Flippers pick a light, start turns it on or off. For checking bulbs.';
    readonly keepsHighScores = false;

    async run(m: Machine): Promise<void> {
        for (let i = 0; i < LAMP_COUNT; i++) m.setLamp(i as LampId, false);
        m.setAutofire(0);
        m.stopSound();

        const on = new Set<LampId>();
        let selected = 0;
        const show = () => {
            const [name, id] = LAMPS[selected];
            m.scrollText(`${id} ${name.replaceAll('_', ' ')} - ${on.has(id) ? 'ON' : 'OFF'}      `, 180);
        };
        show();

        for (;;) {
            const ev = await m.nextEvent(60_000);
            if (!ev?.pressed) continue;

            if (ev.id === Switch.FLIPPER_LEFT || ev.id === Switch.FLIPPER_RIGHT) {
                const step = ev.id === Switch.FLIPPER_RIGHT ? 1 : -1;
                selected = (selected + step + LAMPS.length) % LAMPS.length;
                m.blinkLamp(LAMPS[selected][1], 150, 4); // an even count ends where it started
                show();
            } else if (ev.id === Switch.START) {
                const id = LAMPS[selected][1];
                if (on.has(id)) on.delete(id);
                else on.add(id);
                m.setLamp(id, on.has(id)); // also stops a blink
                show();
            }
        }
    }
}
