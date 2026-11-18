# Game mode ideas

Ideas for new game modes. **Frenzy** and **Sudden Death** came from this list and are built (`pi/src/games/`). The rest are not built yet.

What every mode has to work with:

- **Switches:** A B C D lanes, 3 targets (left "1", right "2", center "3"), 2 spinners, 2 extra-ball lanes, 2 advance lanes, 2 bumpers, 2 thumper bumpers, 2 slingshots, the outhole, start, both flipper buttons.
- **Lights:** A–D, the 3 targets, the 2 extra-ball lanes, the bonus ladder (1,000–10,000), double and triple bonus, "same player shoots again", ball 1–5 and player 1–4.
- **Lights with a matching switch:** A–D, the 3 targets and the 2 extra-ball lanes. Spinners, bumpers and slingshots have no light.
- **Outputs:** 8-character display, 3-voice music plus short effects on top (`playEffect`), the ball-return kicker, bumper and slingshot power per mode.
- **Limits:** one ball at a time (no multiball). The flippers can't be moved by the game.

## Hunt

One light among A–D, the targets and the extra-ball lanes flashes. Hit it within a few seconds and its value goes up, then a new one lights. Missing resets the value.

- Uses the lights that have switches.
- Rewards aiming over bashing bumpers.
- No phone needed.

## Missions

The display announces a goal, such as `SPIN 25`, `BUMP 40`, `ABCD` or `ALL TARGETS`. Finishing it pays a big bonus and plays a jingle, and the next mission starts, each harder than the last.

- Gives the spinners and bumpers a purpose even though they have no lights.
- No phone needed.

## Hot Streak

Like Frenzy, but each score has to come within a few seconds of the last to keep a **multiplier** climbing (×2, ×3, ×4…). The music speeds up as it rises. Go quiet too long and it drops back to ×1.

- Rewards keeping the ball busy, not just surviving.
- Most of Frenzy's code carries over: scoring, effects, lights, the rolling score, music that can speed up.

## Sudden Death (built)

One ball, no kick-back. Points are worth more the longer you survive, and the music keeps getting faster and higher until you drain.

- The opposite of Frenzy: no clock, one life.

## Head-to-head (with phones)

Two players alternate balls, and the web page shows a live scoreboard and whose turn it is. A "steal" option would let hitting a lit target take points from the other player.

- Needs the web page during play. The scores page already updates every second.

## Bingo (with phones)

The 9 lit shots form a card shown on your phone. Each hit marks a square, and you need a row before draining.

- Very visual, but it needs the phone.

## Adding a mode

1. Copy an existing mode in `pi/src/games/` and give it an `id`, `name` and `description`. Frenzy and Sudden Death are short examples: the scoring, sounds, lights, rolling score and end screens they share are in `arcade.ts`.
2. List it in `pi/src/games/index.ts`. It gets a button on the web page automatically.
3. Give it a `SoundBank` with its own sound names, and put the files in `pi/sounds/<id>/` (see `Midis/README.md`).
4. Set `keepsHighScores = true` if it should save scores.
