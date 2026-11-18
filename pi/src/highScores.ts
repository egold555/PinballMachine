// Every game's score for each game mode, with its date, kept in
// data/settings.json. One entry per game: in a multiplayer game, the best
// player's score.

import { load, save } from './storage.js';

export interface GameScore {
    score: number;
    date: string; // ISO time, or '' if not known
}

const key = (modeId: string) => `${modeId}.scores`;

// Oldest first.
export function scores(modeId: string): GameScore[] {
    const list = load<GameScore[] | null>(key(modeId), null);
    if (list) return list;
    // Earlier versions kept only a top 10, and before that only the best.
    const top = load<GameScore[] | null>(`${modeId}.highScores`, null);
    if (top) return [...top].sort((a, b) => a.date.localeCompare(b.date));
    const best = load(`${modeId}.highScore`, 0);
    return best > 0 ? [{ score: best, date: '' }] : [];
}

export function bestScore(modeId: string): number {
    return Math.max(0, ...scores(modeId).map((s) => s.score));
}

// Saves a finished game. Returns true if it's a new best.
export function recordGame(modeId: string, score: number): boolean {
    if (score <= 0) return false;
    const before = bestScore(modeId);
    save(key(modeId), [...scores(modeId), { score, date: new Date().toISOString() }]);
    return score > before;
}
