// Shared by the arcade-style modes (Frenzy, Sudden Death): what each switch
// scores, jackpots for collecting A B C D or all three targets, a blip and a
// flash of lights on every score, a light chase, a score that rolls up to its
// new value, and the final score screens.

import { bestScore } from '../highScores.js';
import { Autofire, type Machine, now, type SwitchEvent } from '../Machine.js';
import { Coil, Lamp, LAMP_COUNT, type LampId, Switch, type SwitchId } from '../protocol/index.js';

// Every arcade mode has these sounds (plus its own).
export type ArcadeSound = 'point' | 'big-point' | 'jackpot';

const BALL_RETURN_PULSE = 100;
const BALL_KICK_RETRY_MS = 250;
const SPINNER_SCORE_DELAY = 75;
const CHASE_STEP_MS = 70;
const ROLL_STEP_MS = 25; // the score counts up to its new value this often
const ROLL_STEPS = 16; // ...and gets there in this many steps (0.4 s)
const JACKPOT_SHOW_MS = 900;

// What each switch scores, and whether it's a big hit (its own sound and lights).
const POINTS: Partial<Record<SwitchId, { points: number; big?: boolean }>> = {
    [Switch.LEFT_THUMPER_BUMPER]: { points: 1000 },
    [Switch.RIGHT_THUMPER_BUMPER]: { points: 1000 },
    [Switch.LEFT_BUMPER]: { points: 500 },
    [Switch.RIGHT_BUMPER]: { points: 500 },
    [Switch.LEFT_SLINGSHOT]: { points: 100 },
    [Switch.RIGHT_SLINGSHOT]: { points: 100 },
    [Switch.LEFT_SPINNER]: { points: 1000 },
    [Switch.RIGHT_SPINNER]: { points: 1000 },
    [Switch.LEFT_ADVANCE_LANE]: { points: 5000, big: true },
    [Switch.RIGHT_ADVANCE_LANE]: { points: 5000, big: true },
    [Switch.LEFT_EXTRA_BALL_LANE]: { points: 5000, big: true },
    [Switch.RIGHT_EXTRA_BALL_LANE]: { points: 5000, big: true },
    [Switch.LEFT_TARGET]: { points: 5000, big: true },
    [Switch.RIGHT_TARGET]: { points: 5000, big: true },
    [Switch.CENTER_TARGET]: { points: 5000, big: true },
    [Switch.A]: { points: 10_000, big: true },
    [Switch.B]: { points: 10_000, big: true },
    [Switch.C]: { points: 10_000, big: true },
    [Switch.D]: { points: 10_000, big: true },
};
const SPINNERS: SwitchId[] = [Switch.LEFT_SPINNER, Switch.RIGHT_SPINNER];

// Switches that put out their light when hit. Collecting the whole group is a jackpot.
const GROUPS: { switches: SwitchId[]; lamps: LampId[]; jackpot: number }[] = [
    { switches: [Switch.A, Switch.B, Switch.C, Switch.D], lamps: [Lamp.A, Lamp.B, Lamp.C, Lamp.D], jackpot: 100_000 },
    {
        switches: [Switch.LEFT_TARGET, Switch.RIGHT_TARGET, Switch.CENTER_TARGET],
        lamps: [Lamp.TARGET_LEFT, Lamp.TARGET_RIGHT, Lamp.TARGET_CENTER],
        jackpot: 50_000,
    },
];

// Runs up the bonus ladder the whole time.
const CHASE: LampId[] = [
    Lamp.BONUS_1000, Lamp.BONUS_2000, Lamp.BONUS_3000, Lamp.BONUS_4000, Lamp.BONUS_5000,
    Lamp.BONUS_6000, Lamp.BONUS_7000, Lamp.BONUS_8000, Lamp.BONUS_9000, Lamp.BONUS_10000,
];
// Flash on every score (the backbox lights).
const PULSE: LampId[] = [
    Lamp.PLAYER_1, Lamp.PLAYER_2, Lamp.PLAYER_3, Lamp.PLAYER_4,
    Lamp.BALL_1, Lamp.BALL_2, Lamp.BALL_3, Lamp.BALL_4, Lamp.BALL_5,
];
// Flash on big hits.
const BIG_FLASH: LampId[] = [
    Lamp.DOUBLE_BONUS, Lamp.TRIPLE_BONUS, Lamp.SAME_PLAYER_SHOOTS, Lamp.EXTRA_BALL_LEFT, Lamp.EXTRA_BALL_RIGHT,
];

export function allLamps(m: Machine, on: boolean): void {
    for (let i = 0; i < LAMP_COUNT; i++) m.setLamp(i as LampId, on);
}

// Every light but the chase strobes for a moment (an even count ends where it started).
export function strobe(m: Machine): void {
    for (let i = 0; i < LAMP_COUNT; i++) if (!CHASE.includes(i as LampId)) m.blinkLamp(i as LampId, 50, 24);
}

// Scrolls "<title> - PRESS START" and the high score until start is pressed.
export async function attract(m: Machine, title: string, modeId: string): Promise<void> {
    let msg = `${title} - PRESS START      `;
    const high = bestScore(modeId);
    if (high > 0) msg += `HIGH SCORE - ${high}      `;
    m.scrollText(msg);
    for (;;) {
        const ev = await m.nextEvent(1000);
        if (ev?.pressed && ev.id === Switch.START) return;
    }
}

// The final score, as fixed screens in turn (scrolling text got cut off), with
// the ending song, until start is pressed.
export async function finalScreens(
    m: Machine,
    o: { score: number; newHigh: boolean; title: string; ending: Uint8Array | null },
): Promise<void> {
    m.stopSound();
    m.playSound(o.ending, { loop: true });
    const score = String(o.score).padStart(8);
    const screens: [string, number][] = o.newHigh
        ? [['NEW HIGH', 1200], [' SCORE!', 1000], [score, 3000]]
        : [[o.title, 1200], [score, 3000]];
    for (let i = 0; ; i = (i + 1) % screens.length) {
        m.showText(screens[i][0]);
        if (o.newHigh && i < 2) allLamps(m, i === 0); // the lights flash with "NEW HIGH"
        if (await m.sleepUntilPressed(screens[i][1], Switch.START)) break;
    }
    m.stopSound();
    allLamps(m, false);
}

// The playing part: scoring, effects, lights and the score display.
export class Arcade {
    score = 0;
    private shown = 0; // what the score display has rolled up to
    private rollBy = 0; // how much it counts up each step
    private drawn = -1; // the number on the display, -1 if it shows a word
    private displayFreeAt = 0; // a word (GO, JACKPOT) stays up until then
    private nextSpinnerScore = 0;
    private nextBallKick = 0;
    private chase = 0;
    private nextChase = 0;
    private nextRoll = 0;

    constructor(
        private readonly m: Machine,
        private readonly sound: (name: ArcadeSound) => Uint8Array | null,
    ) {}

    // A new game: score 0, A B C D and the targets lit, bumpers and slingshots on.
    start(): void {
        this.score = 0;
        this.shown = 0;
        this.rollBy = 0;
        this.drawn = -1;
        this.displayFreeAt = 0;
        for (const g of GROUPS) for (const l of g.lamps) this.m.setLamp(l, true);
        this.m.setAutofire(Autofire.ALL);
    }

    kickBall(): void {
        this.m.pulseCoil(Coil.BALL_RETURN, BALL_RETURN_PULSE);
        this.nextBallKick = now() + BALL_KICK_RETRY_MS;
    }

    // The ball is in the outhole (and hasn't just been kicked).
    get ballInOuthole(): boolean {
        return this.m.isClosed(Switch.BALL_RETURN) && now() >= this.nextBallKick;
    }

    // Shows a word instead of the score for a while.
    showWord(text: string, ms: number): void {
        this.m.showText(text);
        this.drawn = -1;
        this.displayFreeAt = now() + ms;
    }

    // Call often: moves the light chase and rolls the score display.
    tick(chaseStepMs = CHASE_STEP_MS): void {
        if (now() >= this.nextChase) {
            this.m.setLamp(CHASE[this.chase], false);
            this.chase = (this.chase + 1) % CHASE.length;
            this.m.setLamp(CHASE[this.chase], true);
            this.nextChase = now() + chaseStepMs;
        }
        if (now() >= this.nextRoll) {
            this.roll();
            this.nextRoll = now() + ROLL_STEP_MS;
        }
    }

    // Scores a switch hit (times the multiplier), with its sound and lights.
    // Returns the points added. Tilt and switches that don't score add nothing.
    hit(ev: SwitchEvent, multiplier = 1): number {
        let points = 0;
        let big = false;
        if (SPINNERS.includes(ev.id)) {
            if (ev.time >= this.nextSpinnerScore) {
                points += POINTS[ev.id]!.points;
                this.nextSpinnerScore = ev.time + SPINNER_SCORE_DELAY;
            }
        } else if (ev.pressed && POINTS[ev.id]) {
            points += POINTS[ev.id]!.points;
            big = POINTS[ev.id]!.big ?? false;
        }

        let jackpot = false;
        for (const g of GROUPS) {
            const i = g.switches.indexOf(ev.id);
            if (i < 0 || !ev.pressed) continue;
            this.m.setLamp(g.lamps[i], false);
            if (g.lamps.every((l) => !this.m.lamp(l))) {
                points += g.jackpot;
                jackpot = true;
                for (const l of g.lamps) this.m.setLamp(l, true);
            }
        }
        if (!points) return 0;

        points *= multiplier;
        this.score += points;
        for (const l of PULSE) this.m.blinkLamp(l, 60, 2);

        if (jackpot) {
            this.m.playEffect(this.sound('jackpot'));
            this.showWord('JACKPOT', JACKPOT_SHOW_MS);
            strobe(this.m);
        } else if (big) {
            this.m.playEffect(this.sound('big-point'));
            for (const l of BIG_FLASH) this.m.blinkLamp(l, 70, 4);
        } else {
            this.m.playEffect(this.sound('point'));
        }
        return points;
    }

    // Counts the display up towards the score at a steady pace.
    private roll(): void {
        if (now() < this.displayFreeAt) return;
        const behind = this.score - this.shown;
        // Fast enough to catch up within ROLL_STEPS steps, in round tens.
        this.rollBy = behind > 0 ? Math.max(this.rollBy, Math.ceil(behind / ROLL_STEPS / 10) * 10) : 0;
        this.shown += Math.min(behind, this.rollBy);
        if (this.drawn !== this.shown) {
            this.m.showNumber(this.shown);
            this.drawn = this.shown;
        }
    }
}
