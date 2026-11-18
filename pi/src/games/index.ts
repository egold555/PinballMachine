// Every game mode the machine can play. The first one is the default.
// Each one gets a button on the web page.

import { Frenzy } from './Frenzy.js';
import type { GameMode } from './GameMode.js';
import { LightTest } from './LightTest.js';
import { OriginalGame } from './OriginalGame.js';
import { SoundTest } from './SoundTest.js';
import { SuddenDeath } from './SuddenDeath.js';
import { SwitchTest } from './SwitchTest.js';

export const GAME_MODES: GameMode[] = [
    new OriginalGame(),
    new Frenzy(),
    new SuddenDeath(),
    new SwitchTest(),
    new SoundTest(),
    new LightTest(),
];
