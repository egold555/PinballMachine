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

import type { MegaLink } from './MegaLink.js';
import { Msg, SOUND_CHANNELS } from './protocol/index.js';

// low: e.g. the scoring blip, interrupts other low sounds only.
// normal: never interrupted by anything except stop().
export type SoundPriority = 'low' | 'normal';

const ALL_CHANNELS = 0xff;

export class SoundPlayer {
    private score: Uint8Array | null = null;
    private cursor = 0;
    private priority: SoundPriority = 'low';
    private loop = false;
    private loopGapMs = 0;
    private passStart = 0; // when the current pass through the score began
    private nextStep = 0;
    private timer: NodeJS.Timeout | null = null;

    constructor(private readonly link: MegaLink) {}

    // Starts a score unless a normal sound is already playing. A looping
    // sound starts over each time it ends (after gapMs of silence), until
    // stop() or another play().
    play(score: Uint8Array, priority: SoundPriority = 'normal', loop = false, gapMs = 1000): void {
        if (this.playing && this.priority === 'normal') return;
        this.cancelTimer();
        this.link.send(Msg.NOTE_OFF, [ALL_CHANNELS]);
        this.score = score;
        this.priority = priority;
        this.loop = loop;
        this.loopGapMs = gapMs;
        this.cursor = 0;
        this.nextStep = performance.now();
        this.passStart = this.nextStep;
        this.run();
    }

    stop(): void {
        if (!this.playing) return;
        this.cancelTimer();
        this.score = null;
        this.link.send(Msg.NOTE_OFF, [ALL_CHANNELS]);
    }

    get playing(): boolean {
        return this.score !== null;
    }

    private cancelTimer(): void {
        if (this.timer) clearTimeout(this.timer);
        this.timer = null;
    }

    private run(): void {
        this.timer = null;
        // Catch up on anything that is due, then sleep until the next step.
        // nextStep adds up exact score times, so timer lateness doesn't build up.
        while (this.playing && performance.now() >= this.nextStep) this.step();
        if (this.playing) this.timer = setTimeout(() => this.run(), Math.max(0, this.nextStep - performance.now()));
    }

    // Runs score commands up to the next wait.
    private step(): void {
        const s = this.score!;
        for (;;) {
            if (this.cursor >= s.length) return this.end();
            const cmd = s[this.cursor++];
            if (cmd < 0x80) {
                this.nextStep += (cmd << 8) | s[this.cursor++];
                return;
            }

            const chan = cmd & 0x0f;
            switch (cmd & 0xf0) {
                case 0x90: {
                    const note = s[this.cursor++];
                    if (chan < SOUND_CHANNELS) this.link.send(Msg.NOTE_ON, [chan, note]);
                    break;
                }
                case 0x80:
                    if (chan < SOUND_CHANNELS) this.link.send(Msg.NOTE_OFF, [chan]);
                    break;
                case 0xe0:
                    this.cursor = 0;
                    break;
                default: // 0xf0 end, or garbage
                    return this.end();
            }
        }
    }

    private end(): void {
        this.link.send(Msg.NOTE_OFF, [ALL_CHANNELS]);
        // A score that takes no time would repeat forever without waiting.
        if (this.loop && (this.nextStep > this.passStart || this.loopGapMs > 0)) {
            this.cursor = 0;
            this.nextStep += this.loopGapMs;
            this.passStart = this.nextStep;
        } else {
            this.score = null;
        }
    }
}
