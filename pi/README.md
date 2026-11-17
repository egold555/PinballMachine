# Pinball brain for the Raspberry Pi

TypeScript version of [`esp/`](../esp/). It runs all the game rules on a Raspberry Pi and drives the Mega over the USB cable it already has. You don't need to touch any wiring.

The Mega runs the same firmware as with the ESP ([`mega/`](../mega/)) and accepts the protocol on its USB port, so either brain works with it.

## Running it

Needs Node 22 or newer.

```sh
npm install
npm run dev              # run from source, finds the Mega automatically
npm run dev /dev/ttyACM0 # or name the port
npm run console          # drive lamps, coils, display and sound by hand
npm test                 # protocol check, type check, lint, tests
```

For the real thing, build once and run the compiled version:

```sh
npm run build && npm start
```

Opening the port resets the Mega, so it takes about two seconds to come up. Only one program can use the port at a time, so stop the game before running the console.

## Layout

| File | What it is |
|---|---|
| `src/main.ts` | Starts everything and runs the game mode in a loop. |
| `src/Machine.ts` | What game code talks to: switch events, lamps, display, sound, coils. |
| `src/MegaLink.ts` | The USB serial link. Reconnects if the cable is pulled. |
| `src/SoundPlayer.ts` | Streams Playtune scores to the Mega's three voices. |
| `src/games/` | Game modes. `OriginalGame.ts` is the full original rule set. |
| `src/protocol/` | Framing, plus `map.ts`, which is generated from [`protocol/`](../protocol/) and must not be edited. |
| `src/sounds/scores.ts` | Sounds, converted from `esp/src/sounds/Sounds.h` by `scripts/convert-sounds.mjs`. |
| `src/storage.ts` | High scores and settings in `data/settings.json`. |

## Writing a game mode

Implement `GameMode` in `src/games/` and use the `Machine` API. Game code is async, and it pauses with `await m.sleep(ms)` or waits for switches with `await m.nextEvent(ms)`. Lamps, blinking, scrolling text, music and the heartbeat keep running while it waits.

Forgetting an `await` makes the game run straight past the pause. `npm run lint` catches that.

## Changing the protocol

Edit the headers in [`protocol/`](../protocol/), then rebuild. `map.ts` is regenerated on every build, and `npm test` fails if it is out of date.

## On the Pi

The Pi (`ssh pinballmachine`, 10.0.0.11) keeps everything in `/home/pi/PinballMachine`. That folder is shared over SMB at `smb://10.0.0.11/PinballMachine` with no login, and over FTP when you log in as `pi`.

| Folder | What it is |
|---|---|
| `Gameplay/` | This code, run by the `pinball-gameplay` service. Update it with `./deploy.sh`. High scores live in `Gameplay/data/`. |
| `Reprogram/` | Drop a Mega `.hex` file here and it uploads to the Mega. `LOG.TXT` shows progress. See `README.TXT` in that folder. |

The setup for all of this is in [`system/`](system/). To change it, edit the files there, copy them over and rerun:

```sh
rsync -a --delete system/ pinballmachine:pinball-system/
ssh -t pinballmachine sudo bash pinball-system/setup.sh
```

Game logs: `ssh pinballmachine journalctl -fu pinball-gameplay`
