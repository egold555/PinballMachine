# Future ideas

Things that are possible but not built yet.

## Flash the Mega over WiFi through the ESP

Only needed with the ESP brain. With the Pi brain ([`pi/`](pi/)) the Pi is already on the Mega's USB port and can flash it directly.

Right now the Mega can only be flashed over its own USB port. It can also be done over the network, with the ESP passing the upload through.

### Why the current wiring isn't enough

The Mega's bootloader only listens on **Serial0** (pins 0/1, the same lines as USB), not Serial2. It also only runs for about a second after a reset, so the ESP needs a way to reset the Mega.

### Extra wiring (new jumpers only, nothing existing changes)

| XIAO ESP32-C6 | | Mega |
|---|---|---|
| free pin, e.g. D0 (TX) | → | pin 0 (RX0) |
| free pin, e.g. D1 (RX) | ← voltage divider ← | pin 1 (TX0) |
| free pin, e.g. D2 | ← 1N4148 diode ← | RESET (power header) |

- Use the same 1 kΩ / 2 kΩ divider as on the Serial2 link, because Mega TX is 5 V.
- The diode's stripe (cathode) goes toward the ESP. When the ESP pulls D2 low, the Mega resets. When D2 is high, the diode blocks the Mega's 5 V from reaching the ESP.
- The Mega already has 1 kΩ resistors between its USB chip and pins 0/1, so the ESP can drive pin 0 without a conflict.

### Software

- **ESP:** open a TCP port (e.g. `pinball.local:2323`). When a client connects:
  1. pause the game and show `UPDATING`
  2. pulse RESET
  3. pass bytes both ways between TCP and Serial0 at the bootloader's baud rate (115200)
  4. go back to normal when the client disconnects
- **Mega:** add an `ota` env to `mega/platformio.ini` that runs avrdude with `-c wiring -P net:pinball.local:2323`. Then `pio run -e ota -t upload` works for both boards.

### Risk

Low. A WiFi upload never touches the bootloader, so a failed upload is fixed by plugging in USB and flashing normally. The fiddly part is reset timing, which has to land inside the bootloader's short window. If it misses, the upload fails and can be retried.

## Public scores page, password-protected settings

Right now the web page (`pi/src/web.ts`, `pi/public/`) is internal only and has no login, so anyone who can reach it can switch game modes. To put it on the internet, split it in two:

- **Public, read only:** the high scores, and maybe the live display. Nothing on it changes the machine.
- **Settings, behind a password:** the game mode buttons, restarting a game, and whatever gets added later (resetting scores, volume, ball count, and so on).

### How

- **Routes:** public pages and `GET` APIs stay at `/`. Everything that changes something moves under `/admin` (pages) and `/api/admin/*` (actions such as `POST /api/admin/mode`).
- **Login,** one of:
  - **At the reverse proxy** (10.0.0.34): require a login only for `/admin` and `/api/admin`. This needs no code in the game.
  - **In the game:** a password set in `data/settings.json` (stored hashed, never in git), a login page, and a session cookie. Every `/api/admin/*` route checks it.

  Either way, the `/api/admin/*` routes must refuse anything without a login, not just hide the buttons.
- **HTTPS:** the proxy already provides it. The password must never travel over plain HTTP.
- **Rate limiting:** add it on the login, so the password can't be guessed by brute force.

## Real audio from the Pi

All sound now comes from the Mega's three square-wave voices: 8-bit beeps, which suit the machine, but three notes at a time and no drums or instruments.

The Pi could play proper audio as well:

- **Hardware:** a small amplifier board and a speaker on the Pi's headphone jack, or a USB sound card. It all sits on the Pi side, so the box's wiring stays untouched.
- **What it could play:** `.wav`/`.mp3` files (real recordings, voice callouts, sound effects), or the same MIDI files rendered with real instruments and drums through a synth (FluidSynth with a General MIDI soundfont).
- **How it would fit:** each sound in a mode's folder could be a `.mid` (played on the beeper, as now) or a `.wav` (played on the Pi's speaker). The beeper keeps the chiptune feel, and the speaker adds anything the beeper can't do.
- **Things to work out:** keeping the Pi's audio in time with the game (it starts a few milliseconds later than the beeper), volume control, and whether both play at once.

## Machine off / on, and shutting down the Pi

When the machine is switched off, the Mega stays powered over the Pi's USB cable, so the display keeps going and flickers. For now the cable gets unplugged by hand (safe: the game starts over when it's plugged back in). Two buttons for the Machine settings page would make that unnecessary:

- **Machine off / on:** the Pi 4 can switch power to its USB ports in software, with `uhubctl` (`uhubctl -l 1-1 -a off` / `on`; on a Pi 4 all four ports switch together, which is fine with only the Mega plugged in). Off: the Mega and display go dark, and the game sees it as an unplug. On: the Mega boots and the game reconnects. Needs `uhubctl` installed and allowed in `pi/system/sudoers`, then `make setup-pi`.
- **Shut down Pi:** a clean `shutdown`, for the rare times the Pi itself must be unplugged (pull the plug once its green light stops). Also needs a sudoers entry.

The Pi is meant to stay on otherwise (3-4 W). Pulling its power while running risks corrupting the SD card. A read-only SD card setup would make that always safe, but scores and settings would have to be stored elsewhere.

