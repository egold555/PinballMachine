// Top scores for each game mode, kept in data/settings.json.

import { load, save } from './storage.js';

export interface HighScore {
    score: number;
    date: string; // ISO time, or '' if not known
}

const KEEP = 10;

export function highScores(modeId: string): HighScore[] {
    const list = load<HighScore[] | null>(`${modeId}.highScores`, null);
    if (list) return list;
    // Before there was a list, only the best score was saved.
    const best = load(`${modeId}.highScore`, 0);
    return best > 0 ? [{ score: best, date: '' }] : [];
}

export function bestScore(modeId: string): number {
    return highScores(modeId)[0]?.score ?? 0;
}

// Adds a finished game's scores. Returns true if one of them is a new best.
export function recordScores(modeId: string, scores: number[]): boolean {
    const before = bestScore(modeId);
    const date = new Date().toISOString();
    const list = [...highScores(modeId), ...scores.filter((s) => s > 0).map((score) => ({ score, date }))]
        .sort((a, b) => b.score - a.score)
        .slice(0, KEEP);
    save(`${modeId}.highScores`, list);
    return (list[0]?.score ?? 0) > before;
}
