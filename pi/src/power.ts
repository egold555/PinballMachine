// Bumper and slingshot power: how long they fire. One default for the whole
// machine, which each game mode can override. Set from the web page and kept
// in data/settings.json. Longer hits harder but wears coils and rubbers
// faster; the Mega caps any pulse at 150 ms whatever is sent.

import { load, save } from './storage.js';

export const POWER_MS = { original: 50, min: 30, max: 100 } as const; // 50 ms is the original SOLENOID_DELAY

const DEFAULT_KEY = 'power.default';
const modeKey = (modeId: string) => `power.${modeId}`;

function clamp(ms: number): number {
    return Math.round(Math.min(POWER_MS.max, Math.max(POWER_MS.min, ms)));
}

export function defaultPower(): number {
    return clamp(load(DEFAULT_KEY, POWER_MS.original));
}

export function setDefaultPower(ms: number): number {
    save(DEFAULT_KEY, clamp(ms));
    return defaultPower();
}

// The mode's own setting, or null if it follows the default.
export function modePower(modeId: string): number | null {
    const ms = load<number | null>(modeKey(modeId), null);
    return ms === null ? null : clamp(ms);
}

// null goes back to following the default.
export function setModePower(modeId: string, ms: number | null): void {
    save(modeKey(modeId), ms === null ? null : clamp(ms));
}

// What the mode actually uses.
export function powerFor(modeId: string): number {
    return modePower(modeId) ?? defaultPower();
}
