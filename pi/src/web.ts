// The machine's web page: high scores, a button per game mode, and what the
// display shows right now. No login, it's for the home network.
//
//   GET  /api/state     everything the page shows
//   POST /api/mode      {"id": "original"}  switch to (or restart) a game mode
//   POST /api/restart   restart the program, which reloads the sounds

import express from 'express';
import type { Server } from 'node:http';
import { join } from 'node:path';
import type { GameRunner } from './GameRunner.js';
import { highScores } from './highScores.js';
import type { Machine } from './Machine.js';
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
            display: machine.displayText,
            megaConnected: megaConnected(),
            currentMode: runner.currentMode.id,
            modes: runner.modes.map((m) => ({
                id: m.id,
                name: m.name,
                description: m.description,
                highScores: m.keepsHighScores ? highScores(m.id) : null,
            })),
            sounds: soundInfo(),
        });
    });

    app.post('/api/mode', (req, res) => {
        const id: unknown = req.body?.id;
        if (typeof id === 'string' && runner.select(id)) res.json({ ok: true });
        else res.status(404).json({ ok: false, error: 'no such game mode' });
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
