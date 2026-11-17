// The pinball machine as game code sees it.
//
// Game modes only talk to this class: read switch events, set lamps, write
// the display, play sounds, fire coils. Game code is written as async
// functions and pauses with `await m.sleep(ms)`. Lamps, blinking, scrolling
// text, music and the link heartbeat keep running in the background.

import type { LinkListener, MegaLink } from './MegaLink.js';
import {
    bitGet,
    bitPut,
    Coil,
    type CoilId,
    DISPLAY_CHARS,
    HEARTBEAT_MS,
    LAMP_BYTES,
    LAMP_COUNT,
    type LampId,
    Msg,
    Switch,
    SWITCH_BYTES,
    SWITCH_COUNT,
    type SwitchId,
} from './protocol/index.js';
import { SoundPlayer, type SoundPriority } from './SoundPlayer.js';

export interface SwitchEvent {
    id: SwitchId;
    closed: boolean; // the switch closed (true) or opened (false)
    pressed: boolean; // closed, and not a bounce: same rule as the original code
    time: number; // now() when it arrived
}

// Standard autofire rules, set up on the Mega whenever it connects.
export const Autofire = {
    SLINGSHOT_LEFT: 1 << 0,
    SLINGSHOT_RIGHT: 1 << 1,
    THUMPER_LEFT: 1 << 2,
    THUMPER_RIGHT: 1 << 3,
    ALL: 0x0f,
} as const;

// Index i here is Autofire bit i. 50 ms is the original SOLENOID_DELAY.
const AUTOFIRE_RULES: { sw: SwitchId; coil: CoilId; ms: number }[] = [
    { sw: Switch.LEFT_SLINGSHOT, coil: Coil.SLINGSHOT_LEFT, ms: 50 },
    { sw: Switch.RIGHT_SLINGSHOT, coil: Coil.SLINGSHOT_RIGHT, ms: 50 },
    { sw: Switch.LEFT_THUMPER_BUMPER, coil: Coil.THUMPER_LEFT, ms: 50 },
    { sw: Switch.RIGHT_THUMPER_BUMPER, coil: Coil.THUMPER_RIGHT, ms: 50 },
];

// Thrown out of nextEvent()/sleep() when stopGame() is called, so the running
// game unwinds. GameRunner catches it; game code normally doesn't need to.
export class GameStopped extends Error {
    constructor() {
        super('game stopped');
    }
}

const SERVICE_PERIOD_MS = 10;
const DEBOUNCE_MS = 20;
const MAX_QUEUED_EVENTS = 64;

// Milliseconds since the program started. Never jumps when the clock is set.
export const now = (): number => performance.now();

interface Blink {
    interval: number;
    remaining: number;
    next: number;
}

export class Machine implements LinkListener {
    private readonly sound: SoundPlayer;

    private events: SwitchEvent[] = [];
    private waiter: { resolve: (ev: SwitchEvent | null) => void; reject: (err: Error) => void } | null = null;
    private stopping = false;

    private switches = new Uint8Array(SWITCH_BYTES);
    private lastClose = new Array<number>(SWITCH_COUNT).fill(-Infinity);

    private lamps = new Uint8Array(LAMP_BYTES);
    private blinks: Blink[] = Array.from({ length: LAMP_COUNT }, () => ({ interval: 0, remaining: 0, next: 0 }));
    private lampsDirty = true;
    private lastLampSend = -Infinity;

    private text = ' '.repeat(DISPLAY_CHARS);
    private textDirty = true;
    private scroll = '';
    private scrolling = false;
    private scrollStepMs = 220;
    private scrollNext = 0;
    private scrollPos = 0;

    private autofire = 0;
    private lockedOut = false;

    constructor(private readonly link: MegaLink) {
        this.sound = new SoundPlayer(link);
        setInterval(() => this.service(), SERVICE_PERIOD_MS);
    }

    // ------------------------------------------------------------- Switches

    // Waits up to timeoutMs for the next switch event. Returns null on timeout.
    nextEvent(timeoutMs: number): Promise<SwitchEvent | null> {
        if (this.stopping) return Promise.reject(new GameStopped());
        const queued = this.events.shift();
        if (queued) return Promise.resolve(queued);
        return new Promise((resolve, reject) => {
            const timer = setTimeout(() => {
                this.waiter = null;
                resolve(null);
            }, Math.max(0, timeoutMs));
            const done = () => {
                clearTimeout(timer);
                this.waiter = null;
            };
            this.waiter = {
                resolve: (ev) => (done(), resolve(ev)),
                reject: (err) => (done(), reject(err)),
            };
        });
    }

    isClosed(id: SwitchId): boolean {
        return bitGet(this.switches, id);
    }

    // Waits, throwing away switch events (like a delay() in the old code).
    async sleep(ms: number): Promise<void> {
        await this.sleepUntilPressed(ms, null);
    }

    // Like sleep(), but returns true early if the switch is pressed.
    async sleepUntilPressed(ms: number, id: SwitchId | null): Promise<boolean> {
        const end = now() + ms;
        for (;;) {
            const left = end - now();
            if (left <= 0) return false;
            const ev = await this.nextEvent(left);
            if (ev?.pressed && ev.id === id) return true;
        }
    }

    clearEvents(): void {
        this.events = [];
    }

    onSwitch(id: number, closed: boolean): void {
        const ev: SwitchEvent = { id: id as SwitchId, closed, pressed: false, time: now() };
        bitPut(this.switches, id, closed);
        if (closed) {
            ev.pressed = ev.time - this.lastClose[id] > DEBOUNCE_MS;
            this.lastClose[id] = ev.time;
        }
        console.log(`[switch] ${String(id).padStart(2)} ${closed ? 'closed' : 'open'}`);

        if (this.waiter) this.waiter.resolve(ev);
        else if (this.events.length < MAX_QUEUED_EVENTS) this.events.push(ev);
    }

    onSwitchState(bits: Uint8Array): void {
        this.switches.set(bits);
    }

    onHello(): void {
        console.log('[link] Mega connected');
        this.resync();
    }

    // Ends the running game: its next (or current) wait throws GameStopped.
    // Sound and autofire go off right away.
    stopGame(): void {
        this.stopping = true;
        this.stopSound();
        this.setAutofire(0);
        this.clearEvents();
        this.waiter?.reject(new GameStopped());
    }

    // Lets game code wait again after stopGame(). GameRunner calls it.
    resumeGame(): void {
        this.stopping = false;
        this.clearEvents();
    }

    // ---------------------------------------------------------------- Lamps

    // Also stops it blinking.
    setLamp(id: LampId, on: boolean): void {
        if (this.lockedOut) return;
        this.blinks[id].remaining = 0;
        if (bitGet(this.lamps, id) !== on) {
            bitPut(this.lamps, id, on);
            this.lampsDirty = true;
        }
    }

    lamp(id: LampId): boolean {
        return bitGet(this.lamps, id);
    }

    // Toggles the lamp every intervalMs, `toggles` times, then leaves it alone.
    blinkLamp(id: LampId, intervalMs: number, toggles: number): void {
        if (this.lockedOut) return;
        this.blinks[id] = { interval: intervalMs, remaining: toggles, next: now() + intervalMs };
    }

    // -------------------------------------------------------------- Display

    // Left aligned, cut to 8 characters.
    showText(text: string): void {
        if (this.lockedOut) return;
        this.scrolling = false;
        this.setText(text);
    }

    // Right aligned.
    showNumber(n: number): void {
        this.showText(String(n).padStart(DISPLAY_CHARS));
    }

    // Scrolls msg in the background until the next showText()/showNumber().
    // Calling it again with the same message keeps scrolling where it was.
    scrollText(msg: string, stepMs = 220): void {
        if (this.lockedOut || msg.length === 0) return;
        this.scrollStepMs = stepMs;
        if (this.scrolling && msg === this.scroll) return;
        this.scroll = msg;
        this.scrollPos = 0;
        this.scrollNext = now();
        this.scrolling = true;
    }

    // What the display shows right now.
    get displayText(): string {
        return this.text;
    }

    private setText(text: string): void {
        this.text = text.slice(0, DISPLAY_CHARS).padEnd(DISPLAY_CHARS);
        this.textDirty = true;
    }

    // ---------------------------------------------------------------- Sound

    // A looping sound repeats until stopSound() (or another sound replaces it).
    playSound(score: Uint8Array, priority: SoundPriority = 'normal', loop = false): void {
        if (!this.lockedOut) this.sound.play(score, priority, loop);
    }

    stopSound(): void {
        this.sound.stop();
    }

    get soundPlaying(): boolean {
        return this.sound.playing;
    }

    // sleep() until the current sound ends.
    async waitForSound(): Promise<void> {
        while (this.sound.playing) await this.sleep(10);
    }

    // ---------------------------------------------------------------- Coils

    pulseCoil(id: CoilId, ms: number): void {
        if (this.lockedOut) return;
        this.link.send(Msg.COIL_PULSE, [id, ms]);
    }

    // Autofire bits, 0 = all off.
    setAutofire(rules: number): void {
        if (this.lockedOut) return;
        this.autofire = rules;
        this.link.send(Msg.AUTOFIRE_ENABLE, [rules]);
    }

    // --------------------------------------------------------------- Safety

    // Turns everything off and ignores game output from now on (used when shutting down).
    lockOut(message: string): void {
        this.autofire = 0;
        this.link.send(Msg.AUTOFIRE_ENABLE, [0]);
        this.sound.stop();
        this.lamps.fill(0);
        for (const b of this.blinks) b.remaining = 0;
        this.lampsDirty = true;
        this.scrolling = false;
        this.setText(message);
        this.service();
        this.lockedOut = true;
    }

    // ------------------------------------------------------ Background work

    private service(): void {
        const t = now();

        for (let i = 0; i < LAMP_COUNT; i++) {
            const b = this.blinks[i];
            if (b.remaining && t >= b.next) {
                bitPut(this.lamps, i, !bitGet(this.lamps, i));
                this.lampsDirty = true;
                b.remaining--;
                b.next += b.interval;
            }
        }

        if (this.scrolling && t >= this.scrollNext) {
            const len = this.scroll.length;
            let text = '';
            for (let i = 0; i < DISPLAY_CHARS; i++) text += this.scroll[(this.scrollPos + i) % len];
            this.setText(text);
            this.scrollPos = (this.scrollPos + 1) % len;
            this.scrollNext += this.scrollStepMs;
        }

        if (this.textDirty) this.sendDisplay();

        // The lamp frame doubles as the heartbeat that keeps the Mega out of safe mode.
        if (this.lampsDirty || t - this.lastLampSend >= HEARTBEAT_MS) this.sendLamps();
    }

    private sendDisplay(): void {
        const payload = Buffer.alloc(DISPLAY_CHARS + 1); // last byte: no decimal points
        payload.write(this.text, 'latin1');
        this.link.send(Msg.DISPLAY_TEXT, payload);
        this.textDirty = false;
    }

    private sendLamps(): void {
        this.link.send(Msg.LAMPS, this.lamps);
        this.lampsDirty = false;
        this.lastLampSend = now();
    }

    // The Mega just (re)connected and forgot everything: tell it again.
    private resync(): void {
        AUTOFIRE_RULES.forEach((r, i) => this.link.send(Msg.AUTOFIRE_RULE, [i, r.sw, r.coil, r.ms]));
        this.link.send(Msg.AUTOFIRE_ENABLE, [this.autofire]);
        this.lampsDirty = true;
        this.textDirty = true;
    }
}
