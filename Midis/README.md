# Music for the machine

The machine plays music on **three square-wave voices**: at most three notes at a time, all of them beeps. Every game mode has its own sounds, in a folder named after the mode (`original/`, `switch-test/`, …). A MIDI file from the internet usually has 5 to 15 parts with drums and chords, so it has to be cut down before it sounds good. This folder holds the raw MIDI files and the recipes that cut them down.

```
Midis/pi-ending-songs/*.mid      raw downloads (keep them as they came)
Midis/arrangements.yaml          recipe for each sound: which file, which parts, how
        │
        │  make songs
        ▼
pi/sounds/<mode>/<name>.mid      finished files, exactly what the machine plays
        │
        │  make deploy
        ▼
the Pi, Gameplay/data/sounds/<mode>/   read by the game when it starts
```

All `make` commands are run from the top of the repo.

## Adding or replacing a song

1. **Find a MIDI file.** Versions that sound good on beeps, best first:
   - **8-bit, chiptune or NES** versions. They were written for 3 or 4 beeper voices.
   - **Easy piano or piano solo** versions: right hand melody, left hand bass.
   - Full band versions work, but need the most cutting.

   Prefer files where the melody (the part you'd sing) is its own instrument.

2. **Put it in `Midis/pi-ending-songs/`** (or a folder for its mode, like `Midis/frenzy/`). Keep the original file name.

   **From onlinesequencer.net:** the site's MIDI download needs a browser, so use the song's number from its link instead:

   ```sh
   make import-sequence ID=1905502 OUT="Midis/frenzy/Quick 8-bit song.mid"
   ```

3. **Look inside it:**

   ```sh
   make midi-info FILE="Midis/pi-ending-songs/Some Song.mid"
   ```

   This lists every part (channel) with its instrument, note range, how busy it is, how many notes it holds at once, and when it plays. Things to look for:
   - **The melody:** usually a single note at a time ("at once 1"), in the middle-to-high range, playing most of the song. Sometimes it's labelled "Melody" or "Vocal". If two channels have identical numbers, one copies the other; use just one.
   - **The bass:** "bass" instrument, low notes, one at a time.
   - **Parts to avoid:** "busy" over about 10 (strumming, fast chords). Drums are always left out.
   - **A long intro** with only drums or strumming: note when the melody starts (the "plays" column).

4. **Add a recipe to `arrangements.yaml`.** The entry's name is `<mode>/<sound>`, the sound it replaces (see [the list](#sound-names)). Two styles:

   **`lines`** works when there's a clear melody part. It makes three clean parts: melody, middle and bass. Each lists channels, first choice first. When the first is silent, the next one that is playing fills in.

   ```yaml
   original/ending-champ:
     source: pi-ending-songs/Some Song.mid
     style: lines
     startAt: 12              # optional: skip the first 12 seconds
     endAt: 40                # optional: end exactly here (for loops)
     melody: [{ channel: 4 }, { channel: 1 }]
     middle: [{ channel: 2 }]
     bass: [{ channel: 3 }]
     transpose: { melody: 12 } # optional: move the melody up an octave (-12 = down)
   ```

   **`pluck`** works for busy, chord-heavy songs. It's the same method as [Musicotron](https://8bitworkshop.com/musicotron/). Every note of the chosen channels competes for the three voices, newest note wins, and each note rings briefly like a plucked string.

   ```yaml
   original/ending-fireball:
     source: pi-ending-songs/Some Song.mid
     style: pluck
     startAt: 23.5            # optional
     channels: [1, 2, 5]      # optional, default: everything but drums
     pluckMs: 250             # optional: how long each note rings
   ```

   Add a `#` comment saying why you picked what you picked. It helps next time.

5. **Build it and listen:**

   ```sh
   make songs      # writes pi/sounds/<mode>/*.mid
   make preview    # writes Midis/pi-ending-songs/preview/<mode>-<sound>.wav
   ```

   The `.wav` files are exactly what the machine will play. Open them on your computer. Or, after `make deploy`, use **Sound Test** mode to play any sound on the machine itself.

6. **Fix what sounds wrong** (see below), and repeat step 5.

7. **Put it on the machine:**

   ```sh
   make deploy
   ```

   The game restarts and loads the new sounds. The web page's Sounds section shows what loaded.

## When it sounds wrong

| It sounds… | Try |
|---|---|
| like random notes or noise | A busy part (strumming or fast chords) is on a line. Take it out, or switch to `style: pluck`. |
| like the wrong tune | The melody is on the wrong channel. Run `midi-info` again and try another channel first in `melody`. |
| thin or empty in places | Add a second-choice channel to that line, so something fills in when the first is quiet. |
| muddy, too much going on | Use fewer channels. In `pluck`, list only `channels` you need. |
| wrong at the start only | Skip the intro with `startAt`. |
| too low (clicky) or too high (piercing) | `transpose` that line by 12 or -12. Bass is already moved up automatically when it's very low. |
| choppy, notes too short (`pluck`) | Raise `pluckMs`, e.g. 400. |
| like it has gaps | Silences over 3 seconds are already shortened to one beat. Shorter ones are in the song. |

If it still won't sound right, a different MIDI file of the same song (an 8-bit or piano version) usually helps more than more tweaking.

## Sound names

Each game mode lists the sounds it plays, in its code (`pi/src/games/`). A mode only has the sounds it needs. A file whose name isn't on its mode's list is ignored, and `make songs` refuses a recipe for a sound no mode has.

**`original/`**, the Original game:

| Name | When it plays |
|---|---|
| `start-game` | a game starts, and all of A B C D are collected |
| `point` | every time something scores (keep it very short) |
| `new-ball` | the ball drained, before the bonus is counted |
| `extra-ball` | an extra ball is won |
| `tilt` | tilt |
| `ending-amateur` | end of game, best score under 40,000 |
| `ending-super` | 40,000 to 74,999 |
| `ending-champ` | 75,000 to 99,999 |
| `ending-fantastic` | 100,000 to 124,999 |
| `ending-wizard` | 125,000 to 149,999 |
| `ending-fireball` | 150,000 and up |

Ending songs repeat until someone presses start, so any length works. Without a file, the Original game plays its old built-in sound instead, and a missing ending song plays `ending-wizard`.

**`frenzy/`**, Frenzy:

| Name | When it plays |
|---|---|
| `countdown` | each of 5 4 3 2 1 before the ball comes out (keep it short) |
| `music` | starts on GO and plays until the 90 seconds are up. If it's shorter it repeats with no gap, a key higher and a bit faster each time |
| `point` | bumpers, slingshots, spinners. Plays on top of the music |
| `big-point` | lanes, targets, letters. On top of the music |
| `jackpot` | all of A B C D, or all three targets. On top of the music |
| `time-up` | time is up |
| `ending` | the final score screen, repeating until start |

Sounds "on top of the music" borrow the music's middle voice (the arpeggio in a `lines` arrangement) while they play, so keep them short: well under a second.

The music is an original theme from `pi/tools/compose.ts` (see [Original songs](#original-songs)). The ending is "Quick 8-bit song" from Online Sequencer.

**`switch-test/`**, Switch Test:

| Name | When it plays |
|---|---|
| `hit` | a switch was hit |

In any mode other than Original, a sound without a file is silent.

**A new game mode** declares its own list with a `SoundBank` (see `OriginalGame.ts`) and gets its own folder: `pi/sounds/<its id>/`.

A short sound that's already simple (one or two notes at a time) doesn't need a recipe. Name it correctly and put it straight in `pi/sounds/<mode>/`, then `make deploy`.

## Original songs

Some songs are written for the machine instead of downloaded, like the Frenzy theme. They're in `pi/tools/compose.ts` as 8-bar sections (chords, a lead line in a simple note format, and a style for the arpeggio and bass). Edit them there, then:

```sh
make compose    # rewrites the .mid files in Midis/
make songs      # then as usual
make preview
```

## Trying something quickly

You can also drop a `.mid` with the right name into the Pi's share (`smb://10.0.0.11/PinballMachine`, folder `Gameplay/data/sounds/<mode>/`) and press **Restart program** on the web page. That's handy for a quick test, but the next `make deploy` overwrites it with whatever is in `pi/sounds/`. Keep the real version in the repo.

## Other files here

The older files at the top of this folder (`Start Game.mid`, `Tilt.mid` and so on) are the originals of the machine's sounds from 2019, and `new ending music ideas/` holds the first Wizard and Fireball. Their machine-ready copies are in `pi/sounds/`.
