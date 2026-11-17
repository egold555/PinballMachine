// Plays any of the machine's sounds: the flippers pick one, start plays or
// stops it. The display scrolls its name. Coils stay off.

import { type Machine } from '../Machine.js';
import { LAMP_COUNT, type LampId, Switch } from '../protocol/index.js';
import { sound, SOUND_NAMES } from '../sounds/library.js';
import type { GameMode } from './GameMode.js';

export class SoundTest implements GameMode {
    readonly id = 'sound-test';
    readonly name = 'Sound Test';
    readonly description = 'Flippers pick a sound, start plays or stops it. For checking the music.';
    readonly keepsHighScores = false;

    async run(m: Machine): Promise<void> {
        for (let i = 0; i < LAMP_COUNT; i++) m.setLamp(i as LampId, false);
        m.setAutofire(0);
        m.stopSound();

        let selected = 0;
        let playing = false;
        const show = () => {
            const name = SOUND_NAMES[selected].replaceAll('-', ' ').toUpperCase();
            m.scrollText(`${selected + 1} ${name}${playing ? ' - PLAYING' : ''}      `, 180);
        };
        const stop = () => {
            m.stopSound();
            playing = false;
        };
        show();

        for (;;) {
            const ev = await m.nextEvent(100);
            if (playing && !m.soundPlaying) {
                playing = false; // it ended by itself
                show();
            }
            if (!ev?.pressed) continue;

            if (ev.id === Switch.FLIPPER_LEFT || ev.id === Switch.FLIPPER_RIGHT) {
                const step = ev.id === Switch.FLIPPER_RIGHT ? 1 : -1;
                selected = (selected + step + SOUND_NAMES.length) % SOUND_NAMES.length;
                stop();
                show();
            } else if (ev.id === Switch.START) {
                if (playing) stop();
                else {
                    m.playSound(sound(SOUND_NAMES[selected]));
                    playing = true;
                }
                show();
            }
        }
    }
}
