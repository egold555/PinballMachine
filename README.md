# PinballMachine

The code for my pinball machine.

| Folder | What it is |
|---|---|
| [`esp/`](esp/) | **The brain.** Seeed XIAO ESP32-C6. All game rules, sound, WiFi and over-the-air updates. |
| [`pi/`](pi/) | **The brain, Raspberry Pi version.** The same game in TypeScript, talking to the Mega over its USB cable. No wiring needed. |
| [`mega/`](mega/) | **The I/O board.** Arduino Mega 2560, wired as it always was. Scans switches and lamps, fires coils, drives the display and speaker. No game logic. |
| [`protocol/`](protocol/) | The serial protocol and hardware map (switch, lamp and coil IDs) shared by both. |
| [`Arduino/`](Arduino/) | The original all-in-one Mega firmware. Still works. Flash it to go back to how things were. |
| [`Midis/`](Midis/) | Source MIDI files for the music. |
| [`FUTURE.md`](FUTURE.md) | Ideas that are possible but not built yet. |
| [`GAME_IDEAS.md`](GAME_IDEAS.md) | Ideas for new game modes. |

## Wiring the ESP to the Mega

Nothing that is already wired changes. These are new jumpers only:

| XIAO ESP32-C6 | | Mega |
|---|---|---|
| D6 (TX) | → | pin 17 (RX2) |
| D7 (RX) | ← voltage divider ← | pin 16 (TX2) |
| GND | — | GND |

The Mega talks at 5 V and the ESP only handles 3.3 V, so pin 16 needs a divider before it reaches D7:

```
Mega pin 16 ──[ 1 kΩ ]──┬── XIAO D7
                        │
                     [ 2 kΩ ]
                        │
                       GND
```

Power the XIAO from its own USB-C. The ESP → Mega direction (3.3 V into a 5 V input) normally works without a divider. If it's ever unreliable, use a small level-shifter board.

## Building and flashing

Both are PlatformIO projects. From VS Code, open `mega/` or `esp/` as the project folder. From a terminal:

```sh
cd mega && pio run -t upload     # Mega over USB
cd esp  && pio run -t upload     # ESP over USB
```

**Over WiFi:** copy `esp/src/secrets.example.h` to `esp/src/secrets.h` and fill it in, then flash once over USB. After that:

```sh
cd esp && pio run -e ota -t upload
```

The machine shows `UPDATING` and turns everything off while it flashes.

**Testing the Mega without the ESP:** `mega/tools/console.py` runs on a laptop over the Mega's USB port. It prints every switch and lets you toggle lamps, pulse coils, write the display and play notes.

## Adding a game mode

Subclass `GameMode` in `esp/src/games/` and use the `Machine` API (switch events, lamps, display, sound, coils). [`OriginalGame.cpp`](esp/src/games/OriginalGame.cpp) is the full original rule set and a working example. Game code may block with `m.sleep()`. Lamps, blinking, scrolling text, music and the safety heartbeat keep running in the background.

