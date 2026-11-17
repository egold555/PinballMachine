SOUNDS
======

Every sound the game plays is a MIDI file in this folder. To change one,
replace its file, then press "Restart program" on the web page
(http://10.0.0.11). The files are read when the program starts.

The names must be exactly these (upper/lower case doesn't matter):

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

If a file is missing, the old built-in sound plays instead, and a missing
ending song plays ending-wizard. The web page shows which ones are loaded.

How the music is played
-----------------------
The machine has three square-wave voices, so:
  - At most 3 notes sound at once. When a song has more, the highest and
    lowest notes are kept (melody and bass) and the middle ones are cut.
  - Every track is used. Drums (MIDI channel 10) are left out.
  - Instruments, volume and pedal are ignored. Everything sounds like a beep.
Songs written for 3 voices or fewer come out exactly as written.

The ending song repeats while the scores are shown, until someone presses
start (which also starts the next game). Full-length songs are fine.
