// Plays Playtune scores by streaming their notes to the Mega.
//
// The Mega just turns its three square-wave voices on and off. A timer walks
// the score and sends "note on" / "note off" messages at the right times.
//
// Score format (from Playtune / miditones):
//   0x9c nn   start note nn on channel c
//   0x8c      stop channel c
//   0xe0      restart from the beginning
//   0xf0      end
//   0x00-0x7f followed by one more byte: wait that many ms (15 bit, big endian)
//
// Besides the main sound there can be one short effect (a scoring blip) on
// top. It borrows EFFECT_VOICE while it plays: the main sound's notes on that
// voice are skipped until the effect ends, the other voices carry on.

import type { MegaLink } from './MegaLink.js';
import { Msg, SOUND_CHANNELS } from './protocol/index.js';

// low: e.g. the scoring blip, interrupts other low sounds only.
// normal: never interrupted by anything except stop().
export type SoundPriority = 'low' | 'normal';

export interface PlayOptions {
    priority?: SoundPriority; // default normal
    loop?: boolean; // start over each time it ends, until stop() or another play()
    loopGapMs?: number; // silence between repeats, default 1000
    transposeEachLoop?: number; // semitones added on every repeat
    speedUpEachLoop?: number; // tempo multiplied by this on every repeat, e.g. 1.05
}

const ALL_CHANNELS = 0xff;
// The voice effects borrow. Arranged songs put the middle part here, so the
// melody (voice 0) and bass (voice 2) never drop out.
const EFFECT_VOICE = 1;

// One score being walked through.
class Track {
    cursor = 0;
    nextStep: number;
    passStart: number; // when the current pass through the score began
    transpose = 0;
    speed = 1;
    loops = 0; // times it has started over

    constructor(
        readonly score: Uint8Array,
        readonly options: PlayOptions,
        start: number,
    ) {
        this.nextStep = start;
        this.passStart = start;
    }
}

export class SoundPlayer {
    private main: Track | null = null;
    private effect: Track | null = null;
    private timer: NodeJS.Timeout | null = null;

    constructor(private readonly link: MegaLink) {}

    // Starts a score unless a normal sound is already playing.
    play(score: Uint8Array, options: PlayOptions = {}): void {
        if (this.main && (this.main.options.priority ?? 'normal') === 'normal') return;
        this.link.send(Msg.NOTE_OFF, [ALL_CHANNELS]);
        this.effect = null;
        this.main = new Track(score, options, performance.now());
        this.schedule();
    }

    // A short sound on top of whatever plays, on one borrowed voice.
    // A new effect replaces one that is still playing.
    playEffect(score: Uint8Array): void {
        this.effect = new Track(score, {}, performance.now());
        this.link.send(Msg.NOTE_OFF, [EFFECT_VOICE]);
        this.schedule();
    }

    stop(): void {
        if (!this.main && !this.effect) return;
        this.main = null;
        this.effect = null;
        this.link.send(Msg.NOTE_OFF, [ALL_CHANNELS]);
        this.schedule();
    }

    // The main sound (effects don't count).
    get playing(): boolean {
        return this.main !== null;
    }

    // How many times the main sound has started over (0 on its first play).
    get loops(): number {
        return this.main?.loops ?? 0;
    }

    private schedule(): void {
        if (this.timer) clearTimeout(this.timer);
        this.timer = null;
        // Catch up on anything that is due, then sleep until the next step.
        // nextStep adds up exact score times, so timer lateness doesn't build up.
        for (;;) {
            const now = performance.now();
            const due = [this.main, this.effect].find((t) => t !== null && now >= t.nextStep);
            if (!due) break;
            this.step(due);
        }
        const next = Math.min(this.main?.nextStep ?? Infinity, this.effect?.nextStep ?? Infinity);
        if (next !== Infinity) this.timer = setTimeout(() => this.schedule(), Math.max(0, next - performance.now()));
    }

    // Runs one track's commands up to its next wait.
    private step(t: Track): void {
        const s = t.score;
        const isEffect = t === this.effect;
        for (;;) {
            if (t.cursor >= s.length) return this.end(t);
            const cmd = s[t.cursor++];
            if (cmd < 0x80) {
                t.nextStep += ((cmd << 8) | s[t.cursor++]) / t.speed;
                return;
            }

            const chan = cmd & 0x0f;
            // An effect plays all its notes on its one voice. The main sound
            // stays off that voice while an effect has it.
            const voice = isEffect ? EFFECT_VOICE : chan;
            const audible = chan < SOUND_CHANNELS && (isEffect || !(this.effect && voice === EFFECT_VOICE));
            switch (cmd & 0xf0) {
                case 0x90: {
                    const note = s[t.cursor++] + t.transpose;
                    if (audible && note >= 0 && note <= 127) this.link.send(Msg.NOTE_ON, [voice, note]);
                    break;
                }
                case 0x80:
                    if (audible) this.link.send(Msg.NOTE_OFF, [voice]);
                    break;
                case 0xe0:
                    t.cursor = 0;
                    break;
                default: // 0xf0 end, or garbage
                    return this.end(t);
            }
        }
    }

    private end(t: Track): void {
        if (t === this.effect) {
            this.effect = null;
            this.link.send(Msg.NOTE_OFF, [EFFECT_VOICE]); // the main sound gets it back at its next note
            return;
        }
        for (let v = 0; v < SOUND_CHANNELS; v++)
            if (!(this.effect && v === EFFECT_VOICE)) this.link.send(Msg.NOTE_OFF, [v]); // an effect keeps playing
        const o = t.options;
        const gap = o.loopGapMs ?? 1000;
        // A score that takes no time would repeat forever without waiting.
        if (o.loop && (t.nextStep > t.passStart || gap > 0)) {
            t.cursor = 0;
            t.nextStep += gap;
            t.passStart = t.nextStep;
            t.loops++;
            t.transpose += o.transposeEachLoop ?? 0;
            t.speed *= o.speedUpEachLoop ?? 1;
        } else {
            this.main = null;
        }
    }
}
