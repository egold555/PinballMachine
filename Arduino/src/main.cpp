/**@file PinballMachine.ino */

// imports
#include <Arduino.h>
#include <PinballMachine.h>

#include <run.h> //Modified version of https://github.com/billroy/run

#include "SFXAndMusic.h"

// Constants for delays ahs such.
// TODO: DOcument more
const int SOLENOID_DELAY = 50; // 60

const int SPINNER_SCORE_DELAY = 75;

PinballMachine pinballMachine;

// What state is the game in
enum GameState
{
  GS_BEFORE,
  GS_PLAYING,
  GS_GAMEOVER,
  GS_HIGHSCORE,
  GS_TEST_NAME
};

// Current game state we are in
GameState state = GS_BEFORE;

// Defines a player. 1,2,3,4
typedef struct Player
{
  bool lightA;
  bool lightB;
  bool lightC;
  bool lightD;
  long score;
} Player;

// Time (as returned by millis()) that solenoid should retract.
unsigned long timeRetractLeftSlingshot = 0, timeRetractRightSlingshot = 0;
unsigned long timeRetractLeftThumperBumper = 0, timeRetractRightThumperBumper = 0;
unsigned long timeRetractBallReturn = 0;
unsigned long timeNextSpinnerScore = 0;

// player variables
Player players[4];
int amountOfPlayers = 0;
int currentPlayer = 0;

long oldScore = 0;
byte balls = 1;
byte extraBalls = 0;
byte bonus = 0;
const byte MAX_BALLS = 6; // starts at 1 so you have 5 balls
boolean tilted = false;
boolean hasScoredThisRound = false;

// Decare all functions
void setup(void);
void loop(void);
void solinoids(void);
void checkSwitchesAndLightLights(void);
void updateMPXLeds(void);
void updateScore(void);
void writeScore(long score);
void writeScore(long score, bool sound);
void advanceBonus(void);
void extraBall(void);
void reset(void);
void startGame(void);
void lightPlayerLights(int num);
void setPlayerLight(void);
void switchPlayersOrAdvanceBall(void);
void endOfBall(void);
bool delayWithLights(int delayTime);
void endGame(void);
String getRankingTitle(long score);
void tilt(void);
void timerBlinkExtraBallLeft(void);
void timerBlinkExtraBallRight(void);
void typingInYourName(void);

/**
 * Arduino setup function
 * TODO:
 */
void setup()
{

  Serial.begin(57600);
  // Serial2.begin(115200); //Recieve ESP

  pinballMachine.setup();

  reset();
  // pinballMachine.playSFX(SOUND_WIZARD);
  // state = GS_TEST_NAME; //JUST FOR TESTING
  // pinballMachine.clearDisplay(); //debug
}

/**
 * Arduino loop
 * TODO: dd
 */
void loop()
{
  runner();
  checkSwitchesAndLightLights();
  updateScore();
  solinoids();
  typingInYourName();
  // pinballMachine.scrollTextTest();

  if (state == GS_BEFORE)
  {
    pinballMachine.scrollText("PRESS START TO PLAY AGAIN           HIGH SCORE - (PLAYER) - (SCORE)            ");
  }

  // serial();
}

/**
 * WIP code to insert your name
 *
 */
char nameChars[8] = {'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A'};
int posToWriteAt = 0;
int charsShouldRender = 0;
char theCharLast = ' ';
boolean charIsOn = true;
int charInOnCount = 0;
boolean enteredName = false;
String winName = "";
void typingInYourName()
{
  if (state != GS_TEST_NAME)
  {
    return;
  }

  if (!enteredName)
  {
    if (pinballMachine.swFlipperLeft.pr)
    {
      if (charsShouldRender < 7)
      {
        charsShouldRender++;
      }
      posToWriteAt++;
      if (posToWriteAt > 7)
      {
        posToWriteAt = 0;
      }
    }

    if (pinballMachine.swFlipperRight.pr)
    {
      nameChars[posToWriteAt]++;
      charIsOn = true;
      charInOnCount = 0;
    }
  }

  if (pinballMachine.swStart.pr)
  {
    pinballMachine.playSFX(SOUND_NAME_ENTER);
    for (int i = 0; i < charsShouldRender; i++)
    {
      winName += nameChars[i];
    }
    enteredName = true;
  }

  if (!enteredName)
  {

    if (nameChars[posToWriteAt] > 'Z')
    {
      nameChars[posToWriteAt] = 'A';
    }

    if (charInOnCount > 70)
    {
      charInOnCount = 0;
      charIsOn = !charIsOn;
    }

    charInOnCount++;

    if (charIsOn)
    {
      if (theCharLast != nameChars[posToWriteAt])
      {
        pinballMachine.writeDisplay(posToWriteAt, nameChars[posToWriteAt]);
        pinballMachine.updateDisplay();
        theCharLast = nameChars[posToWriteAt];

        for (int i = 0; i < charsShouldRender; i++)
        {
          pinballMachine.writeDisplay(i, nameChars[i]);
        }
      }
    }
    else
    {
      pinballMachine.writeDisplay(posToWriteAt, ' ');
      pinballMachine.updateDisplay();
      theCharLast = ' ';
    }
  }
  else
  {
    pinballMachine.scrollText("Hello, " + winName + "   ");
  }
}

// void serial(){
//   while(Serial2.available() > 0){
//     int c = Serial2.read();
//     Serial.write(c);
//   }
// }

/**
 * Updates the solinoids states
 *
 */
void solinoids()
{

  unsigned long currentTime = millis();

  if (pinballMachine.swBallReturn.sw && state != GS_GAMEOVER && state != GS_BEFORE)
  {
    digitalWrite(OUT_BALL_RETURN, HIGH);
    timeRetractBallReturn = currentTime + /*SOLENOID_DELAY*/ 100;
  }

  if (!tilted)
  {

    // Fire solenoids that should be fired.
    if (pinballMachine.swLeftSlingShot.sw)
    {
      digitalWrite(OUT_SLINGSHOT_LEFT, HIGH);
      timeRetractLeftSlingshot = currentTime + SOLENOID_DELAY;
    }

    if (pinballMachine.swRightSlingShot.sw)
    {
      digitalWrite(OUT_SLINGSHOT_RIGHT, HIGH);
      timeRetractRightSlingshot = currentTime + SOLENOID_DELAY;
    }

    if (pinballMachine.swLeftThumperBumper.sw)
    {
      digitalWrite(OUT_THUMPER_LEFT, HIGH);
      timeRetractLeftThumperBumper = currentTime + SOLENOID_DELAY;
    }

    if (pinballMachine.swRightThumperBumper.sw)
    {
      digitalWrite(OUT_THUMPER_RIGHT, HIGH);
      timeRetractRightThumperBumper = currentTime + SOLENOID_DELAY;
    }
  }

  // Retract solenoids that should be retracted.
  if (currentTime > timeRetractLeftSlingshot)
  {
    digitalWrite(OUT_SLINGSHOT_LEFT, LOW);
  }

  if (currentTime > timeRetractRightSlingshot)
  {
    digitalWrite(OUT_SLINGSHOT_RIGHT, LOW);
  }

  if (currentTime > timeRetractLeftThumperBumper)
  {
    digitalWrite(OUT_THUMPER_LEFT, LOW);
  }

  if (currentTime > timeRetractRightThumperBumper)
  {
    digitalWrite(OUT_THUMPER_RIGHT, LOW);
  }

  if (currentTime > timeRetractBallReturn)
  {
    digitalWrite(OUT_BALL_RETURN, LOW);
  }
}

/**
 * Check and update the giant matrix's for all the lights and buttons
 *
 */
void checkSwitchesAndLightLights()
{

  pinballMachine.loop();

  if (pinballMachine.swTilt.sw && !tilted)
  {
    tilted = true;
    tilt();
  }
}

/**
 * update the currently playing player score
 * TODO
 */
void updateScore()
{

  oldScore = players[currentPlayer].score;

  if (pinballMachine.swStart.pr)
  {
    startGame();
  }

  if (pinballMachine.swBallReturn.pr && state != GS_GAMEOVER)
  {
    endOfBall();
  }

  if (tilted)
  {
    return;
  }

  // ThumperBumpers +100
  if (pinballMachine.swLeftThumperBumper.pr || pinballMachine.swRightThumperBumper.pr)
  {
    players[currentPlayer].score += 100;
  }

  // Spinners +100
  if (pinballMachine.swLeftSpinner.sw || pinballMachine.swRightSpinner.sw)
  {
    unsigned long currTime = millis();
    if (currTime > timeNextSpinnerScore)
    {
      players[currentPlayer].score += 100;
      timeNextSpinnerScore = currTime + SPINNER_SCORE_DELAY;
    }
  }

  // Extra ball lanes and advance lanes +500
  if (pinballMachine.swLeftExtraBallLane.pr || pinballMachine.swRightExtraBallLane.pr)
  {
    players[currentPlayer].score += 500;
  }

  if (pinballMachine.swLeftExtraBallLane.pr && pinballMachine.ltExtraBallLeft)
  {
    extraBall();
    pinballMachine.ltExtraBallLeft = false;
    runDelay(timerBlinkExtraBallLeft, 100, 20);
  }

  if (pinballMachine.swRightExtraBallLane.pr && pinballMachine.ltExtraBallRight)
  {
    extraBall();
    pinballMachine.ltExtraBallRight = false;
    runDelay(timerBlinkExtraBallRight, 100, 20);
  }

  if (pinballMachine.swLeftAdvanceLane.pr || pinballMachine.swRightAdvanceLane.pr)
  {
    advanceBonus();
    players[currentPlayer].score += 500;
  }

  // ABCD  +1000
  if (pinballMachine.swA.pr || pinballMachine.swB.pr || pinballMachine.swC.pr || pinballMachine.swD.pr)
  {
    players[currentPlayer].score += 1000;
  }

  // trigger lights for ABCD
  if (pinballMachine.swA.pr && pinballMachine.ltA)
  {
    pinballMachine.ltA = false;
    advanceBonus();
  }
  if (pinballMachine.swB.pr && pinballMachine.ltB)
  {
    pinballMachine.ltB = false;
    advanceBonus();
  }
  if (pinballMachine.swC.pr && pinballMachine.ltC)
  {
    pinballMachine.ltC = false;
    advanceBonus();
  }
  if (pinballMachine.swD.pr && pinballMachine.ltD)
  {
    pinballMachine.ltD = false;
    advanceBonus();
  }

  // Advance bonus if ABCD is all off
  if (!pinballMachine.ltA && !pinballMachine.ltB && !pinballMachine.ltC && !pinballMachine.ltD)
  {
    // TODO
    pinballMachine.ltA = true;
    pinballMachine.ltB = true;
    pinballMachine.ltC = true;
    pinballMachine.ltD = true;
    // runDelay(timerBlinkABCD, 100, 10);
    players[currentPlayer].score += 25000;
    pinballMachine.ltExtraBallLeft = true;
    pinballMachine.ltExtraBallRight = true;
    pinballMachine.playSFX(SOUND_STARTUP);
  }

  // Left and Right Bumper +50
  if (pinballMachine.swLeftBumper.pr || pinballMachine.swRightBumper.pr)
  {
    players[currentPlayer].score += 50;
  }

  // Targets

  if (pinballMachine.swLeftTarget.pr || pinballMachine.swRightTarget.pr || pinballMachine.swCenterTarget.pr)
  {
    players[currentPlayer].score += 500;
  }

  if (pinballMachine.swLeftTarget.pr && pinballMachine.lt1)
  {
    pinballMachine.lt1 = false;
    advanceBonus();
    advanceBonus();
  }

  if (pinballMachine.swRightTarget.pr && pinballMachine.lt2)
  {
    pinballMachine.lt2 = false;
    advanceBonus();
    advanceBonus();
  }

  if (pinballMachine.swCenterTarget.pr && pinballMachine.lt3)
  {
    players[currentPlayer].score += 500; // Accounting fo 500 of above function
    pinballMachine.lt3 = false;
    advanceBonus();
    advanceBonus();
  }

  if (!pinballMachine.lt1 && !pinballMachine.lt2)
  {
    if (!pinballMachine.lt3)
    {
      pinballMachine.ltDoubleBonus = false;
      pinballMachine.ltTrippleBonus = true;
    }
    else
    {
      pinballMachine.ltDoubleBonus = true;
    }
  }

  if (bonus >= 1)
  {
    pinballMachine.ltBonus1000 = true;
  }
  if (bonus >= 2)
  {
    pinballMachine.ltBonus2000 = true;
  }
  if (bonus >= 3)
  {
    pinballMachine.ltBonus3000 = true;
  }
  if (bonus >= 4)
  {
    pinballMachine.ltBonus4000 = true;
  }
  if (bonus >= 5)
  {
    pinballMachine.ltBonus5000 = true;
  }
  if (bonus >= 6)
  {
    pinballMachine.ltBonus6000 = true;
  }
  if (bonus >= 7)
  {
    pinballMachine.ltBonus7000 = true;
  }
  if (bonus >= 8)
  {
    pinballMachine.ltBonus8000 = true;
  }
  if (bonus >= 9)
  {
    pinballMachine.ltBonus9000 = true;
  }
  if (bonus >= 10)
  {
    pinballMachine.ltBonus10000 = true;
  }

  //////////////////////////////////
  if (oldScore != players[currentPlayer].score)
  {
    writeScore(players[currentPlayer].score);
    hasScoredThisRound = true;
    setPlayerLight();
  }
}

/**
 * Writes the score to the screen and plays the score sound
 *
 * @param score
 */
void writeScore(long score)
{
  writeScore(score, true);
}

/**
 * Writes the score to the screen
 *
 * @param score players score
 * @param sound should we play the score sound?
 */
void writeScore(long score, bool sound)
{
  pinballMachine.writeDisplay(score);
  if (sound)
  {
    if (pinballMachine.isSFXPlaying())
    {
      pinballMachine.stopSFX();
    }
    pinballMachine.playSFX(SOUND_POINT);
  }
}

/**
 * Advance the bonus up by one
 *
 */
void advanceBonus()
{
  bonus++;
  if (bonus > 10)
  {
    bonus = 10;
  }
}

/**
 * Give the player a extra ball
 *
 */
void extraBall()
{
  pinballMachine.ltSamePlayerShoots = true;
  extraBalls++;
  pinballMachine.playSFX(SOUND_EXTRABALL);
}

/**
 * reset the game back to the initial state.
 *
 */
void reset()
{
  for (int p = 0; p < 4; ++p)
  {
    players[p].score = 0;
    players[p].lightA = true;
    players[p].lightB = true;
    players[p].lightC = true;
    players[p].lightD = true;
  }

  amountOfPlayers = 0;
  currentPlayer = 0;

  players[currentPlayer].score = 0;
  oldScore = 0;
  balls = 1;
  extraBalls = 0;
  bonus = 0;
  tilted = false;

  hasScoredThisRound = false;

  pinballMachine.ltA = pinballMachine.ltB = pinballMachine.ltC = pinballMachine.ltD = true;
  pinballMachine.lt1 = pinballMachine.lt2 = pinballMachine.lt3 = true;
  pinballMachine.ltBonus1000 = pinballMachine.ltBonus2000 = pinballMachine.ltBonus3000 = pinballMachine.ltBonus4000 = pinballMachine.ltBonus5000 = false;
  pinballMachine.ltBonus6000 = pinballMachine.ltBonus7000 = pinballMachine.ltBonus8000 = pinballMachine.ltBonus9000 = pinballMachine.ltBonus10000 = false;
  pinballMachine.ltExtraBallRight = pinballMachine.ltDoubleBonus = pinballMachine.ltExtraBallLeft = pinballMachine.ltSamePlayerShoots = pinballMachine.ltTrippleBonus = false;
  pinballMachine.ltBall1 = false;
  pinballMachine.ltBall2 = pinballMachine.ltBall3 = pinballMachine.ltBall4 = pinballMachine.ltBall5 = false;
  pinballMachine.ltPlayer1 = false;
  pinballMachine.ltPlayer2 = pinballMachine.ltPlayer3 = pinballMachine.ltPlayer4 = false;

  state = GS_BEFORE;

  pinballMachine.playSFX(SOUND_STARTUP);
}

/**
 * Start a brand new game
 *
 */
void startGame()
{

  if (state == GS_BEFORE)
  {
    state = GS_PLAYING;
    pinballMachine.ltBall1 = true;
    pinballMachine.writeDisplay(0);
  }

  if (state == GS_GAMEOVER)
  {
    reset();
  }
  else if (state != GS_TEST_NAME && !hasScoredThisRound && players[0].score == 0)
  {
    amountOfPlayers++;
    if (amountOfPlayers > 4)
    {
      amountOfPlayers = 1;
    }
    pinballMachine.playSFX(SOUND_POINT);
    lightPlayerLights(amountOfPlayers);
  }
}

/**
 * Light up the current player light
 * A = 1
 * B = 2
 * C = 3
 * D = 4
 * CLEAR = any other number
 *
 * @param num player light to display, see above comment
 */
void lightPlayerLights(int num)
{
  pinballMachine.ltPlayer1 = (num == 1);
  pinballMachine.ltPlayer2 = (num == 2);
  pinballMachine.ltPlayer3 = (num == 3);
  pinballMachine.ltPlayer4 = (num == 4);
}

/**
 * Set the Player Light object
 * Light up the current players light
 */
void setPlayerLight()
{
  lightPlayerLights(currentPlayer + 1);
}

/**
 * The current player has no extra balls hit end of ball.
 * Advance to the next player and/or next ball.
 */
void switchPlayersOrAdvanceBall()
{
  players[currentPlayer].lightA = pinballMachine.ltA;
  players[currentPlayer].lightB = pinballMachine.ltB;
  players[currentPlayer].lightC = pinballMachine.ltC;
  players[currentPlayer].lightD = pinballMachine.ltD;

  ++currentPlayer;
  if (currentPlayer >= amountOfPlayers)
  {
    balls++;
    currentPlayer = 0;
  }

  pinballMachine.ltA = players[currentPlayer].lightA;
  pinballMachine.ltB = players[currentPlayer].lightB;
  pinballMachine.ltC = players[currentPlayer].lightC;
  pinballMachine.ltD = players[currentPlayer].lightD;
  oldScore = players[currentPlayer].score;

  setPlayerLight();
}

/**
 * When the ball goes into the "gutter"
 *
 */
void endOfBall()
{

  if (!hasScoredThisRound)
  {
    return; // Ball fireer has failed, so we should not count this as a new ball, we should keep trying to push the ball out of the shooter
  }

  long bonusAmount = 0;

  pinballMachine.playSFX(SOUND_NEWBALL);

  if (!tilted)
  {

    if (pinballMachine.ltDoubleBonus)
    {
      bonusAmount = bonus * 1000 * 2;
    }
    else if (pinballMachine.ltTrippleBonus)
    {
      bonusAmount = bonus * 1000 * 3;
    }
    else
    {
      bonusAmount = bonus * 1000;
    }
  }

  pinballMachine.lt1 = true;
  pinballMachine.lt2 = true;
  pinballMachine.lt3 = true;

  bonus = 0;

  pinballMachine.ltBonus1000 = false;
  pinballMachine.ltBonus2000 = false;
  pinballMachine.ltBonus3000 = false;
  pinballMachine.ltBonus4000 = false;
  pinballMachine.ltBonus5000 = false;
  pinballMachine.ltBonus6000 = false;
  pinballMachine.ltBonus7000 = false;
  pinballMachine.ltBonus8000 = false;
  pinballMachine.ltBonus9000 = false;
  pinballMachine.ltBonus10000 = false;

  pinballMachine.ltExtraBallLeft = false;
  pinballMachine.ltExtraBallRight = false;

  pinballMachine.ltDoubleBonus = false;
  pinballMachine.ltTrippleBonus = false;
  pinballMachine.ltSamePlayerShoots = false;

  pinballMachine.ltBall1 = false;
  pinballMachine.ltBall2 = false;
  pinballMachine.ltBall3 = false;
  pinballMachine.ltBall4 = false;
  pinballMachine.ltBall5 = false;

  // Wait until the music finishes playing
  while (pinballMachine.isSFXPlaying())
  {
    /*do nothing*/
  }

  while (bonusAmount > 0)
  {
    delay(500);
    bonusAmount -= 1000;
    players[currentPlayer].score += 1000;
    writeScore(players[currentPlayer].score);
  }

  delay(1000);

  // Move to next ball or player.
  if (extraBalls == 0)
  {
    switchPlayersOrAdvanceBall();
  }
  else
  {
    extraBalls--;
  }

  switch (balls)
  {
  case 1:
    pinballMachine.ltBall1 = true;
    break;
  case 2:
    pinballMachine.ltBall2 = true;
    break;
  case 3:
    pinballMachine.ltBall3 = true;
    break;
  case 4:
    pinballMachine.ltBall4 = true;
    break;
  case 5:
    pinballMachine.ltBall5 = true;
    break;
  }

  if (balls >= MAX_BALLS)
  {
    endGame();
    return;
  }
  hasScoredThisRound = false;
  digitalWrite(OUT_BALL_RETURN, HIGH);
  timeRetractBallReturn = millis() + /*SOLENOID_DELAY*/ 100;
  writeScore(players[currentPlayer].score);
  tilted = false;
}

bool delayWithLights(int delayTime)
{
  long startTime = millis();
  long endTime = startTime + delayTime;
  while (millis() < (unsigned long)endTime)
  {
    checkSwitchesAndLightLights();
    if (pinballMachine.swStart.pr)
    {
      return true;
    }
  }

  return false;
}

/**
 * Called when the game ends
 * Plays the end game animation
 * Automatically resets the game as well.
 */
void endGame()
{
  bool endNow = false;
  state = GS_GAMEOVER;
  pinballMachine.playSFX(SOUND_ENDING_SONG);

  pinballMachine.ltBall1 = pinballMachine.ltBall2 = pinballMachine.ltBall3 = pinballMachine.ltBall4 = pinballMachine.ltBall5 = false;
  pinballMachine.ltA = pinballMachine.ltB = pinballMachine.ltC = pinballMachine.ltD = true;
  pinballMachine.lt1 = pinballMachine.lt2 = pinballMachine.lt3 = true;

  while (pinballMachine.isSFXPlaying() && !endNow)
  {
    // Do display animation
    int animationDelay = 2000;
    for (int i = 0; i < amountOfPlayers; i++)
    {
      Player p = players[i];
      lightPlayerLights(i + 1);
      writeScore(p.score, false);
      endNow = delayWithLights(animationDelay);
      if (endNow)
        break;
      pinballMachine.writeDisplay(getRankingTitle(p.score));
      endNow = delayWithLights(animationDelay);
      if (endNow)
        break;
    }
  }

  pinballMachine.stopSFX();
  lightPlayerLights(0);
  reset();
}

/**
 * Get the Ranking Title based off of your score
 *
 * @param score players score
 * @return String their rank
 */
String getRankingTitle(long score)
{
  if (score < 40000)
  {
    return "AMATEUR";
  }
  else if (score < 75000)
  {
    return "SUPER";
  }
  else if (score < 100000)
  {
    return "CHAMP";
  }
  else if (score < 125000)
  {
    return "FANTASTIC";
  }
  else if (score < 150000)
  {
    return "WIZARD";
  }
  else
  {
    return "FIREBALL";
  }
  // else if (score > 150000)
  // {
  //   return "FIREBALL";
  // }
}

/**
 * Called if the machine is tilted during game play
 *
 */
void tilt()
{
  pinballMachine.playSFX(SOUND_TILT);
  pinballMachine.writeDisplay("TILTED");
}

/**
 * Blink the extra ball left light
 *
 */
void timerBlinkExtraBallLeft(void)
{
  pinballMachine.ltExtraBallLeft = !pinballMachine.ltExtraBallLeft;
}

/**
 * Blink the extra ball right light
 *
 */
void timerBlinkExtraBallRight(void)
{
  pinballMachine.ltExtraBallRight = !pinballMachine.ltExtraBallRight;
}