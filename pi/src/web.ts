// The machine's web page: high scores, a button per game mode, and what the
// display shows right now. No login, it's for the home network.
//
//   GET  /api/state     everything the page shows
//   POST /api/mode      {"id": "original"}  switch to (or restart) a game mode
//   POST /api/restart   restart the program, which reloads the sounds
//   POST /api/power     {"ms": 60} the default bumper and slingshot power,
//                       {"mode": "frenzy", "ms": 60} a mode's own ({"ms": null}: follow the default)

import express from 'express';
import type { Server } from 'node:http';
import { join } from 'node:path';
import type { GameRunner } from './GameRunner.js';
import { scores } from './highScores.js';
import type { Machine } from './Machine.js';
import { defaultPower, modePower, POWER_MS, powerFor, setDefaultPower, setModePower } from './power.js';
import { soundInfo } from './sounds/library.js';

export interface WebDeps {
    runner: GameRunner;
    machine: Machine;
    megaConnected: () => boolean;
    restart: () => void;
}

export function startWeb({ runner, machine, megaConnected, restart }: WebDeps, port: number): Server {
    const app = express();
    app.use(express.json());
    app.use(express.static(join(import.meta.dirname, '..', 'public')));

    app.get('/api/state', (_req, res) => {
        res.json({
            display: machine.displayMessage,
            megaConnected: megaConnected(),
            currentMode: runner.currentMode.id,
            modes: runner.modes.map((m) => ({
                id: m.id,
                name: m.name,
                description: m.description,
                scores: m.keepsHighScores ? scores(m.id) : null, // every game, oldest first
            })),
            sounds: soundInfo(),
            power: {
                ...POWER_MS,
                default: defaultPower(),
                modes: Object.fromEntries(
                    runner.modes.map((m) => [m.id, { own: modePower(m.id), uses: powerFor(m.id) }]),
                ),
            },
        });
    });

    app.post('/api/mode', (req, res) => {
        const id: unknown = req.body?.id;
        if (typeof id === 'string' && runner.select(id)) res.json({ ok: true });
        else res.status(404).json({ ok: false, error: 'no such game mode' });
    });

    app.post('/api/power', (req, res) => {
        const { mode, ms } = (req.body ?? {}) as { mode?: unknown; ms?: unknown };
        if (mode === undefined) {
            if (typeof ms !== 'number') return void res.status(400).json({ ok: false, error: 'ms must be a number' });
            setDefaultPower(ms);
        } else {
            if (typeof mode !== 'string' || !runner.modes.some((m) => m.id === mode))
                return void res.status(404).json({ ok: false, error: 'no such game mode' });
            if (typeof ms !== 'number' && ms !== null)
                return void res.status(400).json({ ok: false, error: 'ms must be a number or null' });
            setModePower(mode, ms);
        }
        runner.applyPower();
        res.json({ ok: true });
    });

    app.post('/api/restart', (_req, res) => {
        res.json({ ok: true });
        setTimeout(restart, 100); // let the answer go out first
    });

    const server = app.listen(port, () => {
        const addr = server.address();
        console.log(`[web] http://localhost:${typeof addr === 'object' && addr ? addr.port : port}`);
    });
    server.on('error', (err) => console.log(`[web] can't start: ${err.message}`));
    return server;
}
