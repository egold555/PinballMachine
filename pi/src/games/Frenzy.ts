// Frenzy: score as much as you can in 90 seconds. Drains are kicked straight
// back out, the display shows only the score (never the time left), and the
// music loops until time is up.
//
// The countdown runs on the music's beat (its tempo comes from music.mid),
// so on GO the ball is kicked out, the clock starts and the music comes in,
// all on the downbeat.

import { Autofire, type Machine, now, type SwitchEvent } from '../Machine.js';
import { Coil, Lamp, LAMP_COUNT, type LampId, Switch, type SwitchId } from '../protocol/index.js';
import { bestScore, recordScores } from '../highScores.js';
import { SoundBank } from '../sounds/library.js';
import type { GameMode } from './GameMode.js';

const SOUND_NAMES = [
    'countdown', // each of 5 4 3 2 1
    'music', // starts on GO and repeats with no gap until time is up
    'time-up',
] as const;

const DEFAULT_BPM = 135; // when there's no music file to take the tempo from
const TITLE_BEATS = 4; // "FRENZY" before the count
const BEATS_PER_COUNT = 2;

const BALL_RETURN_PULSE = 100;
const BALL_KICK_RETRY_MS = 250;
const SPINNER_SCORE_DELAY = 75;
const CHASE_STEP_MS = 70;

// What each switch scores. Collecting A B C D or all three targets adds more.
const POINTS: Partial<Record<SwitchId, number>> = {
    [Switch.LEFT_THUMPER_BUMPER]: 100,
    [Switch.RIGHT_THUMPER_BUMPER]: 100,
    [Switch.LEFT_BUMPER]: 50,
    [Switch.RIGHT_BUMPER]: 50,
    [Switch.LEFT_SLINGSHOT]: 10,
    [Switch.RIGHT_SLINGSHOT]: 10,
    [Switch.LEFT_ADVANCE_LANE]: 500,
    [Switch.RIGHT_ADVANCE_LANE]: 500,
    [Switch.LEFT_EXTRA_BALL_LANE]: 500,
    [Switch.RIGHT_EXTRA_BALL_LANE]: 500,
    [Switch.LEFT_TARGET]: 500,
    [Switch.RIGHT_TARGET]: 500,
    [Switch.CENTER_TARGET]: 500,
    [Switch.A]: 1000,
    [Switch.B]: 1000,
    [Switch.C]: 1000,
    [Switch.D]: 1000,
};
const SPINNERS: SwitchId[] = [Switch.LEFT_SPINNER, Switch.RIGHT_SPINNER];
const SPINNER_POINTS = 100;
const ABCD_BONUS = 10_000;
const TARGETS_BONUS = 5_000;

// Switches that put out their light when hit, until the whole group is collected.
const GROUPS: { switches: SwitchId[]; lamps: LampId[]; bonus: number }[] = [
    { switches: [Switch.A, Switch.B, Switch.C, Switch.D], lamps: [Lamp.A, Lamp.B, Lamp.C, Lamp.D], bonus: ABCD_BONUS },
    {
        switches: [Switch.LEFT_TARGET, Switch.RIGHT_TARGET, Switch.CENTER_TARGET],
        lamps: [Lamp.TARGET_LEFT, Lamp.TARGET_RIGHT, Lamp.TARGET_CENTER],
        bonus: TARGETS_BONUS,
    },
];

// Runs up the bonus ladder during the frenzy.
const CHASE: LampId[] = [
    Lamp.BONUS_1000, Lamp.BONUS_2000, Lamp.BONUS_3000, Lamp.BONUS_4000, Lamp.BONUS_5000,
    Lamp.BONUS_6000, Lamp.BONUS_7000, Lamp.BONUS_8000, Lamp.BONUS_9000, Lamp.BONUS_10000,
];

export class Frenzy implements GameMode {
    readonly id = 'frenzy';
    readonly name = 'Frenzy';
    readonly description = 'As many points as you can before the music stops. Drains come straight back out.';
    readonly keepsHighScores = true;
    readonly sounds = new SoundBank('frenzy', SOUND_NAMES);

    private score = 0;
    private tilted = false;
    private nextSpinnerScore = 0;
    private nextBallKick = 0;
    private startPressedAtEnd = false;

    // lengthMs is only changed by tests.
    constructor(private readonly lengthMs = 90_000) {}

    async run(m: Machine): Promise<void> {
        this.allLamps(m, false);
        m.setAutofire(0);
        if (!this.startPressedAtEnd) await this.attract(m);
        this.startPressedAtEnd = false;
        await this.countdown(m);
        await this.frenzy(m);
        await this.end(m);
    }

    private async attract(m: Machine): Promise<void> {
        let msg = 'FRENZY - PRESS START      ';
        const high = bestScore(this.id);
        if (high > 0) msg += `HIGH SCORE - ${high}      `;
        m.scrollText(msg);
        for (;;) {
            const ev = await m.nextEvent(1000);
            if (ev?.pressed && ev.id === Switch.START) return;
        }
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
            this.allLamps(m, true);
            await at(TITLE_BEATS + i * BEATS_PER_COUNT + 0.5);
            this.allLamps(m, false);
        }
        await at(TITLE_BEATS + 5 * BEATS_PER_COUNT); // GO, on the downbeat
    }

    private async frenzy(m: Machine): Promise<void> {
        this.score = 0;
        this.tilted = false;
        for (const g of GROUPS) for (const l of g.lamps) m.setLamp(l, true);
        m.setAutofire(Autofire.ALL);

        // GO: the ball, the clock and the music, all at once.
        m.pulseCoil(Coil.BALL_RETURN, BALL_RETURN_PULSE);
        this.nextBallKick = now() + BALL_KICK_RETRY_MS;
        const endAt = now() + this.lengthMs;
        m.stopSound(); // a countdown beep that's still ringing
        m.playSound(this.sounds.get('music'), 'normal', true, 0);
        m.showText('   GO');
        let showScoreAt = now() + 1000;

        let chase = 0;
        let nextChase = now();
        while (now() < endAt) {
            const ev = await m.nextEvent(Math.min(20, endAt - now()));
            if (ev?.closed) this.handleSwitch(m, ev);
            if (now() >= showScoreAt) {
                if (!this.tilted) m.showNumber(this.score); // GO stays up for a second
                showScoreAt = Infinity;
            }

            // Drained: straight back out (the original held the coil while the switch was closed).
            if (m.isClosed(Switch.BALL_RETURN) && now() >= this.nextBallKick) {
                this.tilted = false;
                m.setAutofire(Autofire.ALL);
                m.pulseCoil(Coil.BALL_RETURN, BALL_RETURN_PULSE);
                this.nextBallKick = now() + BALL_KICK_RETRY_MS;
            }

            if (now() >= nextChase) {
                m.setLamp(CHASE[chase], false);
                chase = (chase + 1) % CHASE.length;
                m.setLamp(CHASE[chase], true);
                nextChase = now() + CHASE_STEP_MS;
            }
        }
    }

    private handleSwitch(m: Machine, ev: SwitchEvent): void {
        if (ev.id === Switch.TILT && !this.tilted) {
            this.tilted = true;
            m.setAutofire(0);
            m.showText(' TILTED');
            return;
        }
        if (this.tilted) return;

        let points = 0;
        if (SPINNERS.includes(ev.id)) {
            if (ev.time >= this.nextSpinnerScore) {
                points += SPINNER_POINTS;
                this.nextSpinnerScore = ev.time + SPINNER_SCORE_DELAY;
            }
        } else if (ev.pressed) {
            points += POINTS[ev.id] ?? 0;
        }

        for (const g of GROUPS) {
            const i = g.switches.indexOf(ev.id);
            if (i < 0 || !ev.pressed) continue;
            m.setLamp(g.lamps[i], false);
            if (g.lamps.every((l) => !m.lamp(l))) {
                points += g.bonus;
                for (const l of g.lamps) m.blinkLamp(l, 80, 9); // odd count: ends lit again
            }
        }

        if (points) {
            this.score += points;
            m.showNumber(this.score); // also replaces GO early
        }
    }

    private async end(m: Machine): Promise<void> {
        m.setAutofire(0);
        m.stopSound();
        this.allLamps(m, false);
        // Saved straight away, so switching modes during TIME UP can't lose it.
        const newHigh = recordScores(this.id, [this.score]);
        console.log(`[frenzy] scored ${this.score}${newHigh ? ', new high score' : ''}`);
        m.playSound(this.sounds.get('time-up'));
        m.showText('TIME UP');
        await m.sleep(2500);

        // Score and "new high score" in turn until start, which starts the next frenzy.
        for (;;) {
            m.showNumber(this.score);
            if (await m.sleepUntilPressed(3000, Switch.START)) break;
            if (newHigh) m.scrollText('NEW HIGH SCORE      ');
            else m.showText(' FRENZY');
            if (await m.sleepUntilPressed(newHigh ? 4000 : 1500, Switch.START)) break;
        }
        this.startPressedAtEnd = true;
    }

    private allLamps(m: Machine, on: boolean): void {
        for (let i = 0; i < LAMP_COUNT; i++) m.setLamp(i as LampId, on);
    }
}
