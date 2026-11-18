// Sudden Death: one ball. Every time the music comes round again you go up a
// level: the music goes up a key and speeds up, points are multiplied by the
// level, and the lights chase faster. Drain and it's over.
//
// A drain before the first point doesn't count (the ball never really made it
// onto the playfield), so it's kicked out again. Tilt is ignored. Scoring,
// effects and lights are in arcade.ts.

import { recordGame } from '../highScores.js';
import { type Machine, now } from '../Machine.js';
import { Switch } from '../protocol/index.js';
import { SoundBank } from '../sounds/library.js';
import { allLamps, Arcade, attract, finalScreens, strobe } from './arcade.js';
import type { GameMode } from './GameMode.js';

const SOUND_NAMES = [
    'music', // a short loop: each time round is the next level
    'point', // bumpers, slingshots, spinners (on top of the music)
    'big-point', // lanes, targets, letters (on top of the music)
    'jackpot', // all of A B C D, or all three targets (on top of the music)
    'level-up', // on top of the music
    'game-over', // the ball drained
    'ending', // the final score screen, repeating until start
] as const;

const MUSIC_TRANSPOSE_EACH_LOOP = 1;
const MUSIC_SPEED_UP_EACH_LOOP = 1.06;
const LEVEL_MS_WITHOUT_MUSIC = 15_000; // if there's no music file to count loops of
const LEVEL_SHOW_MS = 1200;
const TICK_MS = 25;

export class SuddenDeath implements GameMode {
    readonly id = 'sudden-death';
    readonly name = 'Sudden Death';
    readonly description = 'One ball, no second chances. Survive and it gets faster, and every point is worth more.';
    readonly keepsHighScores = true;
    readonly sounds = new SoundBank('sudden-death', SOUND_NAMES);

    private startPressedAtEnd = false;

    // levelMs is only changed by tests (it's used when there's no music).
    constructor(private readonly levelMs = LEVEL_MS_WITHOUT_MUSIC) {}

    async run(m: Machine): Promise<void> {
        allLamps(m, false);
        m.setAutofire(0);
        if (!this.startPressedAtEnd) await attract(m, 'SUDDEN DEATH', this.id);
        this.startPressedAtEnd = false;

        m.showText(' SUDDEN');
        await m.sleep(800);
        m.showText('  DEATH');
        await m.sleep(800);

        const score = await this.play(m);
        await this.end(m, score);
    }

    // Returns the score when the ball drains.
    private async play(m: Machine): Promise<number> {
        const game = new Arcade(m, (name) => this.sounds.get(name));
        game.start();
        game.kickBall();
        const music = this.sounds.get('music');
        m.playSound(music, {
            loop: true,
            loopGapMs: 0,
            transposeEachLoop: MUSIC_TRANSPOSE_EACH_LOOP,
            speedUpEachLoop: MUSIC_SPEED_UP_EACH_LOOP,
        });
        game.showWord('   GO', 800);

        const startedAt = now();
        let level = 1;
        for (;;) {
            const ev = await m.nextEvent(TICK_MS);
            if (ev?.closed) game.hit(ev, level);

            if (game.ballInOuthole) {
                if (game.score > 0) return game.score; // drained: that's it
                game.kickBall(); // never made it onto the playfield
            }

            // A level for every time the music has come round, or every
            // levelMs if there's no music.
            const reached = 1 + (music ? m.soundLoops : Math.floor((now() - startedAt) / this.levelMs));
            if (reached > level) {
                level = reached;
                m.playEffect(this.sounds.get('level-up'));
                game.showWord(`LEVEL ${level}`.slice(0, 8), LEVEL_SHOW_MS);
                strobe(m);
                console.log(`[sudden-death] level ${level}`);
            }
            game.tick(Math.max(25, 70 - (level - 1) * 8)); // the chase speeds up too
        }
    }

    private async end(m: Machine, score: number): Promise<void> {
        m.setAutofire(0);
        m.stopSound();
        allLamps(m, false);
        // Saved straight away, so switching modes now can't lose it.
        const newHigh = recordGame(this.id, score);
        console.log(`[sudden-death] scored ${score}${newHigh ? ', new high score' : ''}`);
        m.playSound(this.sounds.get('game-over'));
        m.showText('GAMEOVER');
        // Start skips straight to the next game, from here or the score screens.
        if (!(await m.sleepUntilPressed(2500, Switch.START)))
            await finalScreens(m, { score, newHigh, title: ' SUDDEN', ending: this.sounds.get('ending') });
        m.stopSound();
        allLamps(m, false);
        this.startPressedAtEnd = true;
    }
}
