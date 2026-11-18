// Frenzy: score as much as you can in 90 seconds. Drains are kicked straight
// back out, the display shows only the score (never the time left), and the
// music plays until time is up.
//
// The countdown runs on the music's beat (its tempo comes from music.mid),
// so on GO the ball is kicked out, the clock starts and the music comes in,
// all on the downbeat. Scoring, effects and lights are in arcade.ts.

import { recordGame } from '../highScores.js';
import { type Machine, now } from '../Machine.js';
import { Switch } from '../protocol/index.js';
import { SoundBank } from '../sounds/library.js';
import { allLamps, Arcade, attract, finalScreens } from './arcade.js';
import type { GameMode } from './GameMode.js';

const SOUND_NAMES = [
    'countdown', // each of 5 4 3 2 1
    'music', // starts on GO and repeats (a key higher each time) until time is up
    'point', // bumpers, slingshots, spinners (on top of the music)
    'big-point', // lanes, targets, letters (on top of the music)
    'jackpot', // all of A B C D, or all three targets (on top of the music)
    'time-up',
    'ending', // the final score screen, repeating until start
] as const;

const DEFAULT_BPM = 135; // when there's no music file to take the tempo from
const TITLE_BEATS = 4; // "FRENZY" before the count
const BEATS_PER_COUNT = 2;
// Each time the music comes round again it goes up a key and a bit faster.
const MUSIC_TRANSPOSE_EACH_LOOP = 2;
const MUSIC_SPEED_UP_EACH_LOOP = 1.04;
const TICK_MS = 25;

export class Frenzy implements GameMode {
    readonly id = 'frenzy';
    readonly name = 'Frenzy';
    readonly description = 'As many points as you can before the music stops. Drains come straight back out.';
    readonly keepsHighScores = true;
    readonly sounds = new SoundBank('frenzy', SOUND_NAMES);

    private startPressedAtEnd = false;

    // lengthMs is only changed by tests.
    constructor(private readonly lengthMs = 90_000) {}

    async run(m: Machine): Promise<void> {
        allLamps(m, false);
        m.setAutofire(0);
        if (!this.startPressedAtEnd) await attract(m, 'FRENZY', this.id);
        this.startPressedAtEnd = false;
        await this.countdown(m);
        const score = await this.frenzy(m);
        await this.end(m, score);
    }

    // Runs on an absolute schedule, so the count can't drift off the beat.
    private async countdown(m: Machine): Promise<void> {
        const beat = 60_000 / (this.sounds.bpm('music') ?? DEFAULT_BPM);
        const t0 = now();
        const at = (beats: number) => m.sleep(t0 + beats * beat - now());

        m.showText(' FRENZY');
        for (let i = 0; i < 5; i++) {
            await at(TITLE_BEATS + i * BEATS_PER_COUNT);
            m.showText(`    ${5 - i}`);
            m.playSound(this.sounds.get('countdown'));
            allLamps(m, true);
            await at(TITLE_BEATS + i * BEATS_PER_COUNT + 0.5);
            allLamps(m, false);
        }
        await at(TITLE_BEATS + 5 * BEATS_PER_COUNT); // GO, on the downbeat
    }

    // Returns the score. Tilt is ignored in Frenzy.
    private async frenzy(m: Machine): Promise<number> {
        const game = new Arcade(m, (name) => this.sounds.get(name));
        game.start();

        // GO: the ball, the clock and the music, all at once.
        game.kickBall();
        const endAt = now() + this.lengthMs;
        m.stopSound(); // a countdown beep that's still ringing
        m.playSound(this.sounds.get('music'), {
            loop: true,
            loopGapMs: 0,
            transposeEachLoop: MUSIC_TRANSPOSE_EACH_LOOP,
            speedUpEachLoop: MUSIC_SPEED_UP_EACH_LOOP,
        });
        game.showWord('   GO', 1000);

        while (now() < endAt) {
            const ev = await m.nextEvent(Math.min(TICK_MS, endAt - now()));
            if (ev?.closed) game.hit(ev);
            if (game.ballInOuthole) game.kickBall(); // drained: straight back out
            game.tick();
        }
        return game.score;
    }

    private async end(m: Machine, score: number): Promise<void> {
        m.setAutofire(0);
        m.stopSound();
        allLamps(m, false);
        // Saved straight away, so switching modes during TIME UP can't lose it.
        const newHigh = recordGame(this.id, score);
        console.log(`[frenzy] scored ${score}${newHigh ? ', new high score' : ''}`);
        m.playSound(this.sounds.get('time-up'));
        m.showText('TIME UP');
        // Start skips straight to the next frenzy, from here or the score screens.
        if (!(await m.sleepUntilPressed(2500, Switch.START)))
            await finalScreens(m, { score, newHigh, title: ' FRENZY', ending: this.sounds.get('ending') });
        m.stopSound();
        allLamps(m, false);
        this.startPressedAtEnd = true;
    }
}
