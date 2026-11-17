SOUNDS
======

Every game mode has its own sounds, in a folder named after the mode:

  original/       the Original game
  frenzy/         Frenzy
  switch-test/    Switch Test

To change a sound, replace its file, then press "Restart program" on the
web page (http://10.0.0.11). Files are read when the program starts. The
web page lists every mode's sounds and which ones loaded. Sound Test mode
plays any of them on the machine.

Note: "make deploy" copies the repo's sounds (pi/sounds/) over these, so a
file changed only here is replaced on the next deploy. Keep the real one in
the repo. Midis/README.md in the repo explains how songs are made.

The names must be exactly these (upper/lower case doesn't matter):

original/
  start-game.mid        a game starts, and all of A B C D are collected
  point.mid             every time something scores
  new-ball.mid          the ball drained, before the bonus is counted
  extra-ball.mid        an extra ball is won
  tilt.mid              tilt
  Ending songs, picked by the best player's score:
  ending-amateur.mid    under 40,000
  ending-super.mid      40,000 to 74,999
  ending-champ.mid      75,000 to 99,999
  ending-fantastic.mid  100,000 to 124,999
  ending-wizard.mid     125,000 to 149,999
  ending-fireball.mid   150,000 and up
  A missing file plays the old built-in sound, and a missing ending song
  plays ending-wizard. The ending song repeats until start is pressed.

frenzy/
  countdown.mid         each of 5 4 3 2 1 (keep it short)
  music.mid             starts on GO, repeats with no gap until time is up.
                        The countdown follows its tempo.
  time-up.mid           time is up

switch-test/
  hit.mid               a switch was hit

A missing file in any other mode is silent.

How the music is played
-----------------------
The machine has three square-wave voices, so at most 3 notes sound at
once, drums are left out, and everything sounds like a beep. Songs written
for 3 voices or fewer come out exactly as written.
