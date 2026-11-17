// Plays any game mode's sounds: the flippers pick one, start plays or stops
// it. The display scrolls the mode and sound name. Coils stay off.

import { type Machine } from '../Machine.js';
import { LAMP_COUNT, type LampId, Switch } from '../protocol/index.js';
import { soundBanks } from '../sounds/library.js';
import type { GameMode } from './GameMode.js';

export class SoundTest implements GameMode {
    readonly id = 'sound-test';
    readonly name = 'Sound Test';
    readonly description = "Flippers pick any mode's sound, start plays or stops it. For checking the music.";
    readonly keepsHighScores = false;

    async run(m: Machine): Promise<void> {
        for (let i = 0; i < LAMP_COUNT; i++) m.setLamp(i as LampId, false);
        m.setAutofire(0);
        m.stopSound();

        const all = soundBanks().flatMap((bank) => bank.names.map((name) => ({ bank, name })));
        if (all.length === 0) {
            m.scrollText('NO SOUNDS      ');
            for (;;) await m.nextEvent(60_000);
        }
        let selected = 0;
        let playing = false;
        let missing = false;
        const show = () => {
            const { bank, name } = all[selected];
            const label = `${bank.modeId} ${name}`.replaceAll('-', ' ').toUpperCase();
            const state = playing ? ' - PLAYING' : missing ? ' - NO FILE' : '';
            m.scrollText(`${selected + 1} ${label}${state}      `, 180);
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
                selected = (selected + step + all.length) % all.length;
                missing = false;
                stop();
                show();
            } else if (ev.id === Switch.START) {
                if (playing) stop();
                else {
                    const score = all[selected].bank.get(all[selected].name);
                    m.playSound(score);
                    playing = score !== null;
                    missing = score === null;
                }
                show();
            }
        }
    }
}
