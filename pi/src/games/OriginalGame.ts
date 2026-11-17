// The original rules, ported from esp/src/games/OriginalGame.cpp (which came
// from the old Arduino code). It should play exactly like the machine always
// has. Deliberate differences are marked "Change from the original".

import { Autofire, type Machine, now, type SwitchEvent } from '../Machine.js';
import { Coil, Lamp, type LampId, Switch, type SwitchId } from '../protocol/index.js';
import { SoundBank } from '../sounds/library.js';
import * as builtIn from '../sounds/scores.js';
import { bestScore, recordScores } from '../highScores.js';
import type { GameMode } from './GameMode.js';

const BALL_RETURN_PULSE = 100;
const BALL_KICK_RETRY_MS = 250;
const SPINNER_SCORE_DELAY = 75;

const MAX_PLAYERS = 4;
const MAX_BALLS = 6; // starts at 1, so 5 balls

const ABCD_SWITCHES: SwitchId[] = [Switch.A, Switch.B, Switch.C, Switch.D];
const ABCD_LAMPS: LampId[] = [Lamp.A, Lamp.B, Lamp.C, Lamp.D];
const TARGET_LAMPS: LampId[] = [Lamp.TARGET_LEFT, Lamp.TARGET_RIGHT, Lamp.TARGET_CENTER];
const BONUS_LAMPS: LampId[] = [
    Lamp.BONUS_1000, Lamp.BONUS_2000, Lamp.BONUS_3000, Lamp.BONUS_4000, Lamp.BONUS_5000,
    Lamp.BONUS_6000, Lamp.BONUS_7000, Lamp.BONUS_8000, Lamp.BONUS_9000, Lamp.BONUS_10000,
];
const BALL_LAMPS: LampId[] = [Lamp.BALL_1, Lamp.BALL_2, Lamp.BALL_3, Lamp.BALL_4, Lamp.BALL_5];
const PLAYER_LAMPS: LampId[] = [Lamp.PLAYER_1, Lamp.PLAYER_2, Lamp.PLAYER_3, Lamp.PLAYER_4];

const SOUND_NAMES = [
    'start-game', // a game starts, and all of A B C D are collected
    'point', // anything scores
    'new-ball', // the ball drained, before the bonus is counted
    'extra-ball',
    'tilt',
    // End of game, by the best player's rank (see rankingTitle)
    'ending-amateur',
    'ending-super',
    'ending-champ',
    'ending-fantastic',
    'ending-wizard',
    'ending-fireball',
] as const;
type SoundName = (typeof SOUND_NAMES)[number];

interface Player {
    lightA: boolean;
    lightB: boolean;
    lightC: boolean;
    lightD: boolean;
    score: number;
}

export class OriginalGame implements GameMode {
    readonly id = 'original';
    readonly name = 'Original';
    readonly description = 'The rules the machine has always had. Up to 4 players, 5 balls.';
    readonly keepsHighScores = true;
    // Without files, the sounds from the old firmware play.
    readonly sounds = new SoundBank<SoundName>('original', SOUND_NAMES, {
        'start-game': builtIn.SOUND_STARTUP,
        point: builtIn.SOUND_POINT,
        'new-ball': builtIn.SOUND_NEWBALL,
        'extra-ball': builtIn.SOUND_EXTRABALL,
        tilt: builtIn.SOUND_TILT,
        'ending-wizard': builtIn.SOUND_ENDING_SONG,
    });

    private players: Player[] = [];
    private amountOfPlayers = 0;
    private currentPlayer = 0;
    private balls = 1;
    private extraBalls = 0;
    private bonus = 0;
    private tilted = false;
    private hasScoredThisRound = false;
    private gameOver = false;
    private nextSpinnerScore = 0;
    private nextBallKick = 0;
    private startPressedAtEnd = false; // start ended the last game's score screen

    async run(m: Machine): Promise<void> {
        this.reset(m);
        await this.attract(m);
        await this.playGame(m);
    }

    // Reset the game back to the initial state.
    private reset(m: Machine): void {
        this.players = Array.from({ length: MAX_PLAYERS }, () => ({
            lightA: true, lightB: true, lightC: true, lightD: true, score: 0,
        }));
        this.amountOfPlayers = 0;
        this.currentPlayer = 0;
        this.balls = 1;
        this.extraBalls = 0;
        this.bonus = 0;
        this.tilted = false;
        this.hasScoredThisRound = false;
        this.gameOver = false;

        for (const l of [...ABCD_LAMPS, ...TARGET_LAMPS]) m.setLamp(l, true);
        for (const l of [
            ...BONUS_LAMPS, Lamp.EXTRA_BALL_RIGHT, Lamp.EXTRA_BALL_LEFT, Lamp.DOUBLE_BONUS, Lamp.TRIPLE_BONUS,
            Lamp.SAME_PLAYER_SHOOTS, ...BALL_LAMPS, ...PLAYER_LAMPS,
        ])
            m.setLamp(l, false);

        m.setAutofire(0);
        m.playSound(this.sounds.get('start-game'));
    }

    // Scroll the attract message until start is pressed.
    // Change from the original: other switches are ignored here. The original
    // scored them (and could tilt) before the game had started.
    // Change from the original: if start ended the last game's score screen,
    // the new game begins at once instead of waiting for a second press.
    private async attract(m: Machine): Promise<void> {
        if (this.startPressedAtEnd) {
            this.startPressedAtEnd = false;
            this.startGame(m);
            return;
        }

        let msg = 'PRESS START TO PLAY AGAIN           ';
        const high = bestScore(this.id);
        if (high > 0) msg += `HIGH SCORE - ${high}            `;
        m.scrollText(msg);

        for (;;) {
            const ev = await m.nextEvent(1000);
            if (ev?.pressed && ev.id === Switch.START) break;
        }
        this.startGame(m);
    }

    // startGame() from the original, state GS_BEFORE
    private startGame(m: Machine): void {
        m.setLamp(Lamp.BALL_1, true);
        this.writeScore(m, 0, false);
        m.setAutofire(Autofire.ALL);
        this.addPlayer(m);
    }

    private async playGame(m: Machine): Promise<void> {
        while (!this.gameOver) {
            const ev = await m.nextEvent(10);
            if (ev) {
                if (ev.closed) await this.handleSwitch(m, ev);
                continue;
            }

            // Nothing waiting: kick the ball out of the outhole if it is sitting
            // there (the original held the coil on while the switch was closed).
            if (m.isClosed(Switch.BALL_RETURN) && now() >= this.nextBallKick) {
                m.pulseCoil(Coil.BALL_RETURN, BALL_RETURN_PULSE);
                this.nextBallKick = now() + BALL_KICK_RETRY_MS;
            }
        }
    }

    // One switch closing. This is updateScore() from the original.
    private async handleSwitch(m: Machine, ev: SwitchEvent): Promise<void> {
        const player = this.players[this.currentPlayer];
        const oldScore = player.score;
        const pressed = (id: SwitchId) => ev.pressed && ev.id === id;

        if (pressed(Switch.START)) this.addPlayer(m);

        if (pressed(Switch.BALL_RETURN)) {
            // Not scored yet means the ball never made it to the playfield:
            // just kick it again instead of ending the ball.
            if (this.hasScoredThisRound) await this.endOfBall(m);
            return;
        }

        if (ev.id === Switch.TILT && !this.tilted) {
            this.tilt(m);
            return;
        }

        if (this.tilted) return;

        // Thumper bumpers +100
        if (pressed(Switch.LEFT_THUMPER_BUMPER) || pressed(Switch.RIGHT_THUMPER_BUMPER)) player.score += 100;

        // Spinners +100, at most once per SPINNER_SCORE_DELAY
        if (ev.id === Switch.LEFT_SPINNER || ev.id === Switch.RIGHT_SPINNER) {
            if (ev.time >= this.nextSpinnerScore) {
                player.score += 100;
                this.nextSpinnerScore = ev.time + SPINNER_SCORE_DELAY;
            }
        }

        // Extra ball lanes +500, and an extra ball when lit
        if (pressed(Switch.LEFT_EXTRA_BALL_LANE) || pressed(Switch.RIGHT_EXTRA_BALL_LANE)) player.score += 500;

        if (pressed(Switch.LEFT_EXTRA_BALL_LANE) && m.lamp(Lamp.EXTRA_BALL_LEFT)) {
            this.extraBall(m);
            m.setLamp(Lamp.EXTRA_BALL_LEFT, false);
            m.blinkLamp(Lamp.EXTRA_BALL_LEFT, 100, 20);
        }

        if (pressed(Switch.RIGHT_EXTRA_BALL_LANE) && m.lamp(Lamp.EXTRA_BALL_RIGHT)) {
            this.extraBall(m);
            m.setLamp(Lamp.EXTRA_BALL_RIGHT, false);
            m.blinkLamp(Lamp.EXTRA_BALL_RIGHT, 100, 20);
        }

        // Advance lanes +500
        if (pressed(Switch.LEFT_ADVANCE_LANE) || pressed(Switch.RIGHT_ADVANCE_LANE)) {
            this.advanceBonus();
            player.score += 500;
        }

        // ABCD +1000, and advance the bonus the first time each is hit
        for (let i = 0; i < 4; i++) {
            if (pressed(ABCD_SWITCHES[i])) {
                player.score += 1000;
                if (m.lamp(ABCD_LAMPS[i])) {
                    m.setLamp(ABCD_LAMPS[i], false);
                    this.advanceBonus();
                }
            }
        }

        // All of ABCD collected: +25000 and light both extra ball lanes
        if (ABCD_LAMPS.every((l) => !m.lamp(l))) {
            for (const l of ABCD_LAMPS) m.setLamp(l, true);
            player.score += 25000;
            m.setLamp(Lamp.EXTRA_BALL_LEFT, true);
            m.setLamp(Lamp.EXTRA_BALL_RIGHT, true);
            m.playSound(this.sounds.get('start-game'));
        }

        // Bumpers +50
        if (pressed(Switch.LEFT_BUMPER) || pressed(Switch.RIGHT_BUMPER)) player.score += 50;

        // Targets +500, and the first hit on each advances the bonus twice
        if (pressed(Switch.LEFT_TARGET) || pressed(Switch.RIGHT_TARGET) || pressed(Switch.CENTER_TARGET))
            player.score += 500;

        if (pressed(Switch.LEFT_TARGET) && m.lamp(Lamp.TARGET_LEFT)) {
            m.setLamp(Lamp.TARGET_LEFT, false);
            this.advanceBonus();
            this.advanceBonus();
        }

        if (pressed(Switch.RIGHT_TARGET) && m.lamp(Lamp.TARGET_RIGHT)) {
            m.setLamp(Lamp.TARGET_RIGHT, false);
            this.advanceBonus();
            this.advanceBonus();
        }

        if (pressed(Switch.CENTER_TARGET) && m.lamp(Lamp.TARGET_CENTER)) {
            player.score += 500; // the center target is worth 1000 when lit
            m.setLamp(Lamp.TARGET_CENTER, false);
            this.advanceBonus();
            this.advanceBonus();
        }

        // Targets 1 and 2 down: double bonus. All three: triple bonus.
        if (!m.lamp(Lamp.TARGET_LEFT) && !m.lamp(Lamp.TARGET_RIGHT)) {
            if (!m.lamp(Lamp.TARGET_CENTER)) {
                m.setLamp(Lamp.DOUBLE_BONUS, false);
                m.setLamp(Lamp.TRIPLE_BONUS, true);
            } else {
                m.setLamp(Lamp.DOUBLE_BONUS, true);
            }
        }

        for (let i = 0; i < this.bonus; i++) m.setLamp(BONUS_LAMPS[i], true);

        if (oldScore !== player.score) {
            this.writeScore(m, player.score);
            this.hasScoredThisRound = true;
            this.lightPlayerLights(m, this.currentPlayer + 1);
        }
    }

    // Start pressed during a game: add players until the first point is scored.
    private addPlayer(m: Machine): void {
        if (this.hasScoredThisRound || this.players[0].score !== 0) return;

        this.amountOfPlayers++;
        if (this.amountOfPlayers > MAX_PLAYERS) this.amountOfPlayers = 1;
        m.playSound(this.sounds.get('point'), 'low');
        this.lightPlayerLights(m, this.amountOfPlayers);
    }

    private tilt(m: Machine): void {
        this.tilted = true;
        m.setAutofire(0);
        m.playSound(this.sounds.get('tilt'));
        m.showText('TILTED');
    }

    private advanceBonus(): void {
        if (this.bonus < 10) this.bonus++;
    }

    private extraBall(m: Machine): void {
        m.setLamp(Lamp.SAME_PLAYER_SHOOTS, true);
        this.extraBalls++;
        m.playSound(this.sounds.get('extra-ball'));
    }

    // Change from the original: the scoring blip is low priority, so it no longer
    // cuts off the extra ball and ABCD sounds that are triggered by the same hit.
    private writeScore(m: Machine, score: number, withSound = true): void {
        m.showNumber(score);
        if (withSound) m.playSound(this.sounds.get('point'), 'low');
    }

    // Light player lamp 1-4, or none for any other number.
    private lightPlayerLights(m: Machine, num: number): void {
        PLAYER_LAMPS.forEach((l, i) => m.setLamp(l, num === i + 1));
    }

    // No extra balls left: next player, and next ball after the last player.
    private switchPlayersOrAdvanceBall(m: Machine): void {
        const old = this.players[this.currentPlayer];
        old.lightA = m.lamp(Lamp.A);
        old.lightB = m.lamp(Lamp.B);
        old.lightC = m.lamp(Lamp.C);
        old.lightD = m.lamp(Lamp.D);

        if (++this.currentPlayer >= this.amountOfPlayers) {
            this.balls++;
            this.currentPlayer = 0;
        }

        const next = this.players[this.currentPlayer];
        m.setLamp(Lamp.A, next.lightA);
        m.setLamp(Lamp.B, next.lightB);
        m.setLamp(Lamp.C, next.lightC);
        m.setLamp(Lamp.D, next.lightD);
        this.lightPlayerLights(m, this.currentPlayer + 1);
    }

    // The ball drained: count the bonus, then the next ball or game over.
    private async endOfBall(m: Machine): Promise<void> {
        let bonusAmount = 0;
        m.playSound(this.sounds.get('new-ball'));

        if (!this.tilted) {
            const multiplier = m.lamp(Lamp.DOUBLE_BONUS) ? 2 : m.lamp(Lamp.TRIPLE_BONUS) ? 3 : 1;
            bonusAmount = this.bonus * 1000 * multiplier;
        }

        for (const l of TARGET_LAMPS) m.setLamp(l, true);

        this.bonus = 0;
        for (const l of [
            ...BONUS_LAMPS, Lamp.EXTRA_BALL_LEFT, Lamp.EXTRA_BALL_RIGHT, Lamp.DOUBLE_BONUS, Lamp.TRIPLE_BONUS,
            Lamp.SAME_PLAYER_SHOOTS, ...BALL_LAMPS,
        ])
            m.setLamp(l, false);

        await m.waitForSound();

        while (bonusAmount > 0) {
            await m.sleep(500);
            bonusAmount -= 1000;
            this.players[this.currentPlayer].score += 1000;
            this.writeScore(m, this.players[this.currentPlayer].score);
        }

        await m.sleep(1000);

        if (this.extraBalls === 0) this.switchPlayersOrAdvanceBall(m);
        else this.extraBalls--;

        if (this.balls >= 1 && this.balls <= 5) m.setLamp(BALL_LAMPS[this.balls - 1], true);

        if (this.balls >= MAX_BALLS) {
            await this.endGame(m);
            return;
        }

        this.hasScoredThisRound = false;
        m.pulseCoil(Coil.BALL_RETURN, BALL_RETURN_PULSE);
        this.nextBallKick = now() + BALL_KICK_RETRY_MS;
        this.writeScore(m, this.players[this.currentPlayer].score);
        this.tilted = false;
        m.setAutofire(Autofire.ALL);
    }

    // Plays the ending song while showing each player's score and rank, until
    // start is pressed, which also starts the next game.
    // Change from the original: the song depends on the best player's rank,
    // and it repeats (with the scores) until start instead of ending the
    // screen when it ends.
    private async endGame(m: Machine): Promise<void> {
        this.gameOver = true;
        m.setAutofire(0);
        const best = Math.max(0, ...this.players.slice(0, this.amountOfPlayers).map((p) => p.score));
        // A rank with no song of its own plays the Wizard one.
        const rankSong = `ending-${rankingTitle(best).toLowerCase()}` as SoundName;
        m.playSound(this.sounds.get(rankSong) ?? this.sounds.get('ending-wizard'), 'normal', true);
        this.saveHighScore();

        for (const l of BALL_LAMPS) m.setLamp(l, false);
        for (const l of [...ABCD_LAMPS, ...TARGET_LAMPS]) m.setLamp(l, true);

        let endNow = false;
        while (!endNow) {
            for (let i = 0; i < this.amountOfPlayers && !endNow; i++) {
                this.lightPlayerLights(m, i + 1);
                this.writeScore(m, this.players[i].score, false);
                endNow = await m.sleepUntilPressed(2000, Switch.START);
                if (endNow) break;
                m.showText(rankingTitle(this.players[i].score));
                endNow = await m.sleepUntilPressed(2000, Switch.START);
            }
            if (this.amountOfPlayers === 0) endNow = await m.sleepUntilPressed(1000, Switch.START);
        }

        m.stopSound();
        this.lightPlayerLights(m, 0);
        this.startPressedAtEnd = true;
    }

    // Change from the original: every player's score goes on the high score
    // list, not just the best one.
    private saveHighScore(): void {
        const scores = this.players.slice(0, this.amountOfPlayers).map((p) => p.score);
        if (recordScores(this.id, scores)) console.log(`[game] new high score ${Math.max(...scores)}`);
    }
}

// Only 8 characters fit, so "FANTASTIC" shows as "FANTASTI", same as always.
function rankingTitle(score: number): string {
    if (score < 40000) return 'AMATEUR';
    if (score < 75000) return 'SUPER';
    if (score < 100000) return 'CHAMP';
    if (score < 125000) return 'FANTASTIC';
    if (score < 150000) return 'WIZARD';
    return 'FIREBALL';
}
