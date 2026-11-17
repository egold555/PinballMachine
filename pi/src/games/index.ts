// Every game mode the machine can play. The first one is the default.
// Each one gets a button on the web page.

import type { GameMode } from './GameMode.js';
import { LightTest } from './LightTest.js';
import { OriginalGame } from './OriginalGame.js';
import { SoundTest } from './SoundTest.js';
import { SwitchTest } from './SwitchTest.js';

export const GAME_MODES: GameMode[] = [new OriginalGame(), new SwitchTest(), new SoundTest(), new LightTest()];
