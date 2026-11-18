# PinballMachine

The code for my pinball machine.

A **Raspberry Pi** runs the game (TypeScript, in [`pi/`](pi/)) and drives the **Arduino Mega** over the USB cable that comes out of the bottom of the cabinet. The Mega does the hardware: switches, lamps, coils, the display and the speaker. The wiring inside the box is unchanged from the original machine.

| Folder | What it is |
|---|---|
| [`pi/`](pi/) | **The brain.** All game modes, sounds, scores and the web page, running on the Pi. |
| [`mega/`](mega/) | **The I/O board.** Arduino Mega 2560, wired as it always was. Scans switches and lamps, fires coils, drives the display and speaker. No game logic. |
| [`protocol/`](protocol/) | The serial protocol and hardware map (switch, lamp and coil IDs) shared by the Pi, the Mega and the ESP. |
| [`Midis/`](Midis/) | Music: raw MIDI files, the recipes that fit them to the machine's 3 voices, and how-to ([`Midis/README.md`](Midis/README.md)). |
| [`esp/`](esp/) | An earlier brain for a Seeed XIAO ESP32-C6 wired to the Mega. Not in use; the Pi replaced it. See [below](#the-esp-version-not-in-use). |
| [`Arduino/`](Arduino/) | The original all-in-one Mega firmware. Still works. Flash it to go back to how things were. |
| [`GAME_IDEAS.md`](GAME_IDEAS.md) | Ideas for new game modes. |
| [`FUTURE.md`](FUTURE.md) | Other ideas that are possible but not built yet. |

## Everyday use

**Playing:** the web page at **http://10.0.0.11** (or `pinball.ericshome.xyz` at home) picks the game mode and shows every game's score. Its **Machine settings** page has bumper and slingshot power (per mode), the list of sounds, and a restart button.

Game modes: **Original** (the machine's own rules), **Frenzy** (90 seconds, score all you can), **Sudden Death** (one ball, gets faster every level), plus **Switch Test**, **Sound Test** and **Light Test** for checking the hardware.

**Changing things,** from this repo on your computer (`make` lists everything):

| Command | What it does |
|---|---|
| `make deploy` | Builds the game and installs it on the Pi, with the sounds. Restarts the game, so it interrupts a round in progress. |
| `make test` | Runs the game's tests. |
| `make logs` | Follows the game's output on the Pi. |
| `make flash` | Builds the Mega firmware and uploads it through the Pi (drop-folder `Reprogram/` on the share). |
| `make console` | Stops the game and lets you drive the Mega by hand. |
| `make songs` / `make preview` / `make compose` | Music: see [`Midis/README.md`](Midis/README.md). |
| `make setup-pi` | Reruns the Pi's setup after changing `pi/system/` (asks for the sudo password). |

More detail: [`pi/README.md`](pi/README.md) (the game code and the Pi) and [`Midis/README.md`](Midis/README.md) (music).

**Turning it off:** switch the machine off and unplug the Mega's USB cable from the Pi (the Mega would otherwise stay powered over USB and the display flickers). That's safe, even mid-game: the game starts over at "press start" when the cable goes back in. **Leave the Pi itself on.** Pulling its power can corrupt its SD card. FUTURE.md has buttons planned for both.

**Reaching the Pi:** `ssh pinballmachine` (pi@10.0.0.11, SSH key). Its files are shared at `smb://10.0.0.11/PinballMachine` (no login) and over FTP (login `pi`). Scores and settings are in `Gameplay/data/settings.json`. Stop the game before editing that file by hand (`ssh pinballmachine sudo systemctl stop pinball-gameplay`), or the next finished game overwrites the edit.

## Adding a game mode

See the steps at the end of [`GAME_IDEAS.md`](GAME_IDEAS.md). In short: a class in `pi/src/games/` using the `Machine` API, listed in `pi/src/games/index.ts`, with its own sounds in `pi/sounds/<mode id>/`. Frenzy and Sudden Death are short examples built on the shared `arcade.ts`.

## The ESP version (not in use)

Before the Pi, the plan was a Seeed XIAO ESP32-C6 as the brain, wired to the Mega's Serial2 header. The code in [`esp/`](esp/) still works (the Original game only), and the Mega firmware accepts it unchanged.

### Wiring the ESP to the Mega

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

### Building and flashing

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

**Testing the Mega without the ESP:** `mega/tools/console.py` runs on a laptop over the Mega's USB port (or `make console` through the Pi).
