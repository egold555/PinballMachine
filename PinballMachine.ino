#include "Playtune.h" //Music http://blog.bentgeorge.com/?p=119
#include "Adafruit_LEDBackpack.h" //Display
#include "long2text.h" //Peter
#include "run.h" //Modified version of https://github.com/billroy/run

const int SOLENOID_DELAY = 50; //60
const int SWITCH_READ_DELAY = 800; //680
const int ANALOG_THRESHOLD = 200;
const int SPINNER_SCORE_DELAY = 75;
const unsigned int DEBOUNCE_DELAY = 20;

const int OUT_SLINGSHOT_LEFT = 22;
const int OUT_BALL_RETURN = 23;
const int OUT_SLINGSHOT_RIGHT = 24;
const int OUT_THUMPER_LEFT = 25;
const int OUT_THUMPER_RIGHT = 26;

const int OUT_MX0 = 28;
const int OUT_MX1 = 29;
const int OUT_MX2 = 30;
const int OUT_MX3 = 31;
const int OUT_MX4 = 32;
const int OUT_MX5 = 33;
const int OUT_MX6 = 48;
const int OUT_MX7 = 50;
const int OUT_MX8 = 52;

const int OUT_LT0 = 36;
const int OUT_LT1 = 37;
const int OUT_LT2 = 38;
const int OUT_LT3 = 39;

const int OUT_MPX_DATA = 4;
const int OUT_MPX_LATCH = 5;
const int OUT_MPX_CLOCK = 6;


const int IN_SW0 = A12; //36
const int IN_SW1 = A13; //37
const int IN_SW2 = A14; //38
const int IN_SW3 = A15; //39

const int NOTE_1 = 44;
const int NOTE_2 = 45;
const int NOTE_3 = 46;



typedef struct Button {
  boolean sw = false; //Switch
  boolean pr = false; //Pressed
  long tm = 0; //Time

  void debounce(int pin) {
    bool oldSw = sw;
    bool newSw;
    newSw = analogRead(pin) > ANALOG_THRESHOLD;
    sw = newSw;

    pr = false;
    if (newSw && !oldSw) {
      unsigned long currtm = millis();
      if (currtm - tm > DEBOUNCE_DELAY) {
        pr = true;
      }
      tm = currtm;
    }
  }
} Button;

typedef struct Player {
  bool lightA;
  bool lightB;
  bool lightC;
  bool lightD;
  long score;
} Player;

Button swBallReturn;
Button swTilt;
Button swRightSpinner;
Button swRightExtraBallLane;
Button swA;
Button swStart;
Button swLeftTarget;
Button swLeftSlingShot;
Button swB;
Button swLeftThumperBumper;
Button swLeftBumper;
Button swLeftExtraBallLane;
Button swC;
Button swRightThumperBumper;
Button swLeftSpinner;
Button swLeftAdvanceLane;
Button swD;
//Button mx4_sw1;
Button swRightTarget;
Button swRightSlingShot;
Button swCenterTarget;
//Button mx5_sw1;
Button swRightBumper;
Button swRightAdvanceLane;

bool mx0_lt0 = false; //NONE
bool ltBonus8000 = false;
bool ltBonus9000 = false;
bool ltExtraBallRight = false;
bool ltA = true;
bool ltBonus6000 = false;
bool lt1 = true;
bool ltBonus1000 = false;
bool ltB = true;
bool ltBonus5000 = false;
bool ltDoubleBonus = false;
bool ltExtraBallLeft = false;
bool ltC = true;
bool ltBonus7000 = false;
bool ltBonus10000 = false;
bool ltBonus2000 = false;
bool ltD = true;
bool ltBonus3000 = false;
bool lt2 = true;
bool ltSamePlayerShoots = false;
bool lt3 = true;
bool ltBonus4000 = false;
bool ltTrippleBonus = false;
bool mx5_lt3 = false; //NONE
bool ltBall2 = false; //b2
bool ltBall3 = false; //b3
bool ltBall4 = false; //b4
bool ltBall5 = false; //b5
bool ltPlayer1 = false; //p1
bool ltPlayer2 = false; //p2
bool ltPlayer3 = false; //p3
bool ltPlayer4 = false; //p4
bool mx8_lt0 = false;
bool mx8_lt1 = false;
bool mx8_lt2 = false;
bool ltBall1 = true; //b1


// Time (as returned by millis()) that solenoid should retract.
unsigned long timeRetractLeftSlingshot = 0, timeRetractRightSlingshot = 0;
unsigned long timeRetractLeftThumperBumper = 0, timeRetractRightThumperBumper = 0;
unsigned long timeRetractBallReturn = 0;
unsigned long timeNextSpinnerScore = 0;

Adafruit_AlphaNum4 display1 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 display2 = Adafruit_AlphaNum4();
Playtune pt;

const byte PROGMEM SOUND_STARTUP [] = {
  0, 1, 0x90, 67, 0, 59, 0x80, 0, 3, 0x90, 71, 0, 55, 0x80, 0, 3, 0x90, 74, 0, 55, 0x80,
  0, 3, 0x90, 79, 0, 55, 0x80, 0, 3, 0x90, 83, 0, 56, 0x80, 0, 3, 0x90, 68, 0, 56, 0x80,
  0, 3, 0x90, 72, 0, 54, 0x80, 0, 3, 0x90, 75, 0, 54, 0x80, 0, 3, 0x90, 80, 0, 55, 0x80,
  0, 3, 0x90, 84, 0, 56, 0x80, 0, 3, 0x90, 70, 0, 61, 0x80, 0, 3, 0x90, 74, 0, 55, 0x80,
  0, 3, 0x90, 77, 0, 55, 0x80, 0, 3, 0x90, 82, 0, 55, 0x80, 0, 3, 0x90, 86, 0, 56, 0x80,
  0xf0
};

const byte PROGMEM SOUND_POINT [] = {
  0x91, 50, 0, 200, 0x81, 0xf0
  //Start1, NoteNumMidi, Start delay, Delay, Stop1, End
};

const byte PROGMEM SOUND_NEWBALL [] = {
  0, 1, 0x90, 72, 0, 38, 0x80, 0, 2, 0x90, 73, 0, 34, 0x80, 0, 2, 0x90, 74, 0, 29, 0x80,
  1, 235, 0x90, 55, 0x91, 67, 0x92, 71, 0, 153, 0x80, 0x81, 0x82, 0, 0, 0x90, 74, 0x91, 77, 0, 146,
  0, 3, 0x80, 0x81, 0, 34, 0, 37, 0, 37, 0, 38, 0, 1, 0x90, 74, 0x91, 55, 0x92, 77, 0, 36,
  0, 38, 0, 38, 0, 38, 0x80, 0x82, 0x81, 0, 0, 0x90, 74, 0x91, 55, 0x92, 77, 0, 38, 0, 65,
  0x80, 0x82, 0x81, 0, 100, 0x90, 72, 0x91, 57, 0x92, 76, 0, 99, 0x81, 0x80, 0x82, 0, 100, 0x90, 59, 0x91, 71,
  0x92, 74, 0, 96, 0x80, 0x81, 0x82, 0, 100, 0x90, 60, 0x91, 67, 0x92, 72, 0, 153, 0x80, 0x81, 0x82, 0, 0,
  0x90, 64, 0, 149, 0x80, 0, 0, 0x90, 55, 0, 148, 0x80, 0, 0, 0x90, 64, 0, 148, 0x80, 0, 0,
  0x90, 48, 0x91, 60, 0, 153, 0x80, 0x81, 0xf0
};

const byte PROGMEM SOUND_EXTRABALL [] = {
  0, 1, 0x90, 76, 0, 152, 0x91, 79, 0, 5, 0x80, 0, 143, 0x90, 88, 0, 5, 0x81, 0, 142, 0x91, 84,
  0, 5, 0x80, 0, 142, 0x90, 86, 0, 5, 0x81, 0, 148, 0x91, 91, 0, 5, 0x80, 0, 149, 0x81, 0xf0
};

const byte PROGMEM SOUND_SCORE_AMATEUR [] = {
  0x90, 72, 0x91, 65, 0x92, 62, 1, 157, 1, 157, 1, 157, 0x81, 0x91, 74, 0, 206, 0x82, 0, 206, 0x80,
  0x81, 0x90, 62, 0x91, 53, 0x92, 62, 1, 157, 1, 157, 1, 157, 0, 206, 0x80, 0x82, 0, 17, 0, 34,
  0, 68, 0, 8, 0, 8, 0, 17, 0, 17, 0x81, 0, 34, 0x90, 57, 0x91, 84, 0x92, 76, 1, 157,
  1, 157, 0, 206, 0, 206, 0, 206, 0, 206, 0x80, 0x81, 0x82, 0x90, 76, 0x91, 64, 0x92, 76, 0, 206,
  0x80, 0x82, 0, 206, 0x90, 52, 1, 157, 0x92, 60, 0, 206, 0, 206, 0x82, 0x92, 64, 1, 157, 0x81, 0x82,
  0x80, 0x90, 76, 0x91, 72, 0x92, 65, 1, 157, 0x82, 1, 157, 0x80, 0x90, 65, 0x92, 57, 0, 206, 0x81, 0x91, 57,
  0, 206, 0x80, 0x81, 0x82, 0x90, 62, 0, 206, 0x80, 0, 206, 0x90, 57, 0x91, 65, 0x92, 62, 1, 157, 0x80,
  0x81, 1, 157, 0x90, 57, 0x91, 65, 1, 157, 0x80, 0x81, 0x90, 67, 0, 206, 0, 206, 0x82, 0x80, 0x90, 65,
  0x91, 84, 0x92, 76, 0, 206, 0x80, 0, 206, 0x90, 52, 2, 108, 0, 206, 0, 206, 0x80, 0, 206, 0x81,
  0x82, 0x90, 57, 0x91, 64, 0x92, 72, 1, 157, 1, 157, 1, 157, 1, 157, 0x80, 0x81, 0x82, 0xf0
};

const byte PROGMEM SOUND_TILT [] = {
  0x90, 63, 0, 225, 0x80, 0, 112, 0x90, 69, 0x91, 66, 0, 225, 0x80, 0x81, 0, 112, 0x90, 63, 0, 225,
  0x80, 0, 112, 0x90, 72, 0x91, 66, 0, 225, 0x80, 0x81, 0xf0
};

Player players[4];
int amountOfPlayers = 1;
int currentPlayer = 0;

//long score = 0;
long oldScore = 0;
byte balls = 1;
byte extraBalls = 0;
byte bonus = 0;
const byte MAX_BALLS = 6; //starts at 1 so you have 5 ba;ls
boolean tilted = false;
boolean isEndGame = false;
boolean hasScoredThisRound = false;

//Decare all functions
void setup(void);
void loop(void);
void scrollText(String msg);
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
void setPlayerLight(void);
void switchPlayersOrAdvanceBall(void);
void endOfBall(void);
void endGame(void);
String getRankingTitle(int score);
void tilt(void);
void timerBlinkExtraBallLeft(void);
void timerBlinkExtraBallRight(void);
void playSFX(byte* sfx);
void writeDisplay(long num);
void writeDisplay(String msg);
void writeDisplay(int place, char in);
void updateDisplay(void);
void clearDisplay(void);

void setup() {

  Serial.begin(57600);
  //Serial2.begin(115200); //Recieve ESP

  pinMode(OUT_SLINGSHOT_LEFT, OUTPUT);
  pinMode(OUT_SLINGSHOT_RIGHT, OUTPUT);
  pinMode(OUT_THUMPER_LEFT, OUTPUT);
  pinMode(OUT_THUMPER_RIGHT, OUTPUT);
  pinMode(OUT_BALL_RETURN, OUTPUT);

  pinMode(OUT_MX0, OUTPUT);
  pinMode(OUT_MX1, OUTPUT);
  pinMode(OUT_MX2, OUTPUT);
  pinMode(OUT_MX3, OUTPUT);
  pinMode(OUT_MX4, OUTPUT);
  pinMode(OUT_MX5, OUTPUT);
  pinMode(OUT_MX6, OUTPUT);
  pinMode(OUT_MX7, OUTPUT);
  pinMode(OUT_MX8, OUTPUT);

  pinMode(OUT_LT0, OUTPUT);
  pinMode(OUT_LT1, OUTPUT);
  pinMode(OUT_LT2, OUTPUT);
  pinMode(OUT_LT3, OUTPUT);

  pinMode(OUT_MPX_DATA, OUTPUT);
  pinMode(OUT_MPX_LATCH, OUTPUT);
  digitalWrite(OUT_MPX_LATCH, HIGH);
  pinMode(OUT_MPX_CLOCK, OUTPUT);

  pinMode(IN_SW0, INPUT);
  pinMode(IN_SW1, INPUT);
  pinMode(IN_SW2, INPUT);
  pinMode(IN_SW3, INPUT);

  pt.tune_initchan(NOTE_1);
  pt.tune_initchan(NOTE_2);
  pt.tune_initchan(NOTE_3);


  display1.begin(0x70);
  display2.begin(0x71);

  reset();
}



void loop() {
  runner();
  checkSwitchesAndLightLights();
  updateScore();
  updateMPXLeds();
  solinoids();
  //scrollTextTest();

  if (isEndGame) {
    scrollText("PRESS START TO PLAY AGAIN           ");
  }

  //serial();

}

//void serial(){
//  while(Serial2.available() > 0){
//    int c = Serial2.read();
//    Serial.write(c);
//  }
//}

long MSG_DELAY = 20;
long msgCount = 0;
int msgPos = 0;
void scrollText(String msg) {
  int len = msg.length();
  if ((msgCount % MSG_DELAY) == 0) {
    clearDisplay();
    writeDisplay(0, msg.charAt((msgPos) % len));
    writeDisplay(1, msg.charAt((msgPos + 1) % len));
    writeDisplay(2, msg.charAt((msgPos + 2) % len));
    writeDisplay(3, msg.charAt((msgPos + 3) % len));
    writeDisplay(4, msg.charAt((msgPos + 4) % len));
    writeDisplay(5, msg.charAt((msgPos + 5) % len));
    writeDisplay(6, msg.charAt((msgPos + 6) % len));
    writeDisplay(7, msg.charAt((msgPos + 7) % len));
    updateDisplay();

    msgPos++;
    if (msgPos == len) {
      msgPos = 0;
    }
  }

  msgCount++;

}

void solinoids() {

  unsigned long currentTime = millis();

  if (swBallReturn.sw && !isEndGame) {
    digitalWrite(OUT_BALL_RETURN, HIGH);
    timeRetractBallReturn = currentTime + /*SOLENOID_DELAY*/100;
  }

  if (!tilted) {

    // Fire solenoids that should be fired.
    if (swLeftSlingShot.sw) {
      digitalWrite(OUT_SLINGSHOT_LEFT, HIGH);
      timeRetractLeftSlingshot = currentTime + SOLENOID_DELAY;
    }

    if (swRightSlingShot.sw) {
      digitalWrite(OUT_SLINGSHOT_RIGHT, HIGH);
      timeRetractRightSlingshot = currentTime + SOLENOID_DELAY;
    }

    if (swLeftThumperBumper.sw) {
      digitalWrite(OUT_THUMPER_LEFT, HIGH);
      timeRetractLeftThumperBumper = currentTime + SOLENOID_DELAY;
    }

    if (swRightThumperBumper.sw) {
      digitalWrite(OUT_THUMPER_RIGHT, HIGH);
      timeRetractRightThumperBumper = currentTime + SOLENOID_DELAY;
    }
  }


  // Retract solenoids that should be retracted.
  if (currentTime > timeRetractLeftSlingshot) {
    digitalWrite(OUT_SLINGSHOT_LEFT, LOW);
  }

  if (currentTime > timeRetractRightSlingshot) {
    digitalWrite(OUT_SLINGSHOT_RIGHT, LOW);
  }

  if (currentTime > timeRetractLeftThumperBumper) {
    digitalWrite(OUT_THUMPER_LEFT, LOW);
  }

  if (currentTime > timeRetractRightThumperBumper) {
    digitalWrite(OUT_THUMPER_RIGHT, LOW);
  }

  if (currentTime > timeRetractBallReturn) {
    digitalWrite(OUT_BALL_RETURN, LOW);
  }

}

void checkSwitchesAndLightLights() {

  digitalWrite(OUT_MX0, HIGH);
  digitalWrite(OUT_LT0, mx0_lt0);
  digitalWrite(OUT_LT1, ltBonus8000);
  digitalWrite(OUT_LT2, ltBonus9000);
  digitalWrite(OUT_LT3, ltExtraBallRight);

  delayMicroseconds(SWITCH_READ_DELAY);

  swBallReturn.debounce(IN_SW0);
  swTilt.debounce(IN_SW1);
  swRightSpinner.debounce(IN_SW2);
  swRightExtraBallLane.debounce(IN_SW3);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX0, LOW);

  ///////////////////////////////

  digitalWrite(OUT_MX1, HIGH);

  digitalWrite(OUT_LT0, ltA);
  digitalWrite(OUT_LT1, ltBonus6000);
  digitalWrite(OUT_LT2, lt1);
  digitalWrite(OUT_LT3, ltBonus1000);

  delayMicroseconds(SWITCH_READ_DELAY);

  swA.debounce(IN_SW0);
  swStart.debounce(IN_SW1);
  swLeftTarget.debounce(IN_SW2);
  swLeftSlingShot.debounce(IN_SW3);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX1, LOW);

  //////////////////

  digitalWrite(OUT_MX2, HIGH);

  digitalWrite(OUT_LT0, ltB);
  digitalWrite(OUT_LT1, ltBonus5000);
  digitalWrite(OUT_LT2, ltDoubleBonus);
  digitalWrite(OUT_LT3, ltExtraBallLeft);

  delayMicroseconds(SWITCH_READ_DELAY);

  swB.debounce(IN_SW0);
  swLeftThumperBumper.debounce(IN_SW1);
  swLeftBumper.debounce(IN_SW2);
  swLeftExtraBallLane.debounce(IN_SW3);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX2, LOW);

  /////////////////////

  digitalWrite(OUT_MX3, HIGH);

  digitalWrite(OUT_LT0, ltC);
  digitalWrite(OUT_LT1, ltBonus7000);
  digitalWrite(OUT_LT2, ltBonus10000);
  digitalWrite(OUT_LT3, ltBonus2000);

  delayMicroseconds(SWITCH_READ_DELAY);

  swC.debounce(IN_SW0);
  swRightThumperBumper.debounce(IN_SW1);
  swLeftSpinner.debounce(IN_SW2);
  swLeftAdvanceLane.debounce(IN_SW3);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX3, LOW);

  /////////////////////////////

  digitalWrite(OUT_MX4, HIGH);

  digitalWrite(OUT_LT0, ltD);
  digitalWrite(OUT_LT1, ltBonus3000);
  digitalWrite(OUT_LT2, lt2);
  digitalWrite(OUT_LT3, ltSamePlayerShoots);

  delayMicroseconds(SWITCH_READ_DELAY);

  swD.debounce(IN_SW0);
  //mx4_sw1.debounce(IN_SW1);
  swRightTarget.debounce(IN_SW2);
  swRightSlingShot.debounce(IN_SW3);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX4, LOW);


  ///////////////////////////////

  digitalWrite(OUT_MX5, HIGH);

  digitalWrite(OUT_LT0, lt3);
  digitalWrite(OUT_LT1, ltBonus4000);
  digitalWrite(OUT_LT2, ltTrippleBonus);
  digitalWrite(OUT_LT3, mx5_lt3);

  delayMicroseconds(SWITCH_READ_DELAY);

  swCenterTarget.debounce(IN_SW0);
  //mx5_sw1.debounce(IN_SW1);
  swRightBumper.debounce(IN_SW2);
  swRightAdvanceLane.debounce(IN_SW3);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX5, LOW);

  ///////////////////////////////

  digitalWrite(OUT_MX6, HIGH);

  digitalWrite(OUT_LT0, ltBall2);
  digitalWrite(OUT_LT1, ltBall3);
  digitalWrite(OUT_LT2, ltBall4);
  digitalWrite(OUT_LT3, ltBall5);

  delayMicroseconds(SWITCH_READ_DELAY);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX6, LOW);

  ///////////////////////////////

  digitalWrite(OUT_MX7, HIGH);

  digitalWrite(OUT_LT0, ltPlayer1);
  digitalWrite(OUT_LT1, ltPlayer2);
  digitalWrite(OUT_LT2, ltPlayer3);
  digitalWrite(OUT_LT3, ltPlayer4);

  delayMicroseconds(SWITCH_READ_DELAY);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX7, LOW);

  ///////////////////////////////

  digitalWrite(OUT_MX8, HIGH);

  digitalWrite(OUT_LT0, mx8_lt0);
  digitalWrite(OUT_LT1, mx8_lt1);
  digitalWrite(OUT_LT2, mx8_lt2);
  digitalWrite(OUT_LT3, ltBall1);

  delayMicroseconds(SWITCH_READ_DELAY);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX8, LOW);


  if (swTilt.sw && !tilted) {
    tilted = true;
    tilt();
  }

}

void updateMPXLeds() {
  byte leds = 0;
  bitSet(leds, balls);
  //  bitSet(leds, 0);
  //  bitSet(leds, 2);
  //  bitSet(leds, 4);
  //  bitSet(leds, 6);
  digitalWrite(OUT_MPX_LATCH, LOW);
  shiftOut(OUT_MPX_DATA, OUT_MPX_CLOCK, MSBFIRST, leds);
  digitalWrite(OUT_MPX_LATCH, HIGH);
}

void updateScore() {

  oldScore = players[currentPlayer].score;

  if (swStart.pr) {
    startGame();
  }

  if (swBallReturn.pr && !isEndGame) {
    endOfBall();
  }

  if (tilted) {
    return;
  }

  //ThumperBumpers +100
  if (swLeftThumperBumper.pr || swRightThumperBumper.pr) {
    players[currentPlayer].score += 100;
  }

  //Spinners +100
  if (swLeftSpinner.sw || swRightSpinner.sw) {
    unsigned long currTime = millis();
    if (currTime > timeNextSpinnerScore) {
      players[currentPlayer].score += 100;
      timeNextSpinnerScore = currTime + SPINNER_SCORE_DELAY;
    }
  }

  //Extra ball lanes and advance lanes +500
  if (swLeftExtraBallLane.pr || swRightExtraBallLane.pr) {
    players[currentPlayer].score += 500;
  }

  if (swLeftExtraBallLane.pr && ltExtraBallLeft) {
    extraBall();
    ltExtraBallLeft = false;
    runDelay(timerBlinkExtraBallLeft, 100, 20);
  }

  if (swRightExtraBallLane.pr && ltExtraBallRight) {
    extraBall();
    ltExtraBallRight = false;
    runDelay(timerBlinkExtraBallRight, 100, 20);
  }

  if (swLeftAdvanceLane.pr || swRightAdvanceLane.pr) {
    advanceBonus();
    players[currentPlayer].score += 500;
  }

  //ABCD  +1000
  if (swA.pr || swB.pr || swC.pr || swD.pr) {
    players[currentPlayer].score += 1000;
  }

  //trigger lights for ABCD
  if (swA.pr && ltA) {
    ltA = false;
    advanceBonus();
  }
  if (swB.pr && ltB) {
    ltB = false;
    advanceBonus();
  }
  if (swC.pr && ltC) {
    ltC = false;
    advanceBonus();
  }
  if (swD.pr && ltD) {
    ltD = false;
    advanceBonus();
  }

  //Advance bonus if ABCD is all off
  if (!ltA && !ltB && !ltC && !ltD) {
    //TODO
    ltA = true;
    ltB = true;
    ltC = true;
    ltD = true;
    //runDelay(timerBlinkABCD, 100, 10);
    players[currentPlayer].score += 25000;
    ltExtraBallLeft = true;
    ltExtraBallRight = true;
    playSFX(SOUND_STARTUP);
  }

  //Left and Right Bumper +50
  if (swLeftBumper.pr || swRightBumper.pr) {
    players[currentPlayer].score += 50;
  }

  //Targets

  if (swLeftTarget.pr || swRightTarget.pr || swCenterTarget.pr) {
    players[currentPlayer].score += 500;
  }

  if (swLeftTarget.pr && lt1) {
    lt1 = false;
    advanceBonus();
    advanceBonus();
  }

  if (swRightTarget.pr && lt2) {
    lt2 = false;
    advanceBonus();
    advanceBonus();
  }

  if (swCenterTarget.pr && lt3) {
    players[currentPlayer].score += 500; //Accounting fo 500 of above function
    lt3 = false;
    advanceBonus();
    advanceBonus();
  }

  if (!lt1 && !lt2) {
    if (!lt3) {
      ltDoubleBonus = false;
      ltTrippleBonus = true;
    }
    else {
      ltDoubleBonus = true;
    }
  }

  if (bonus >= 1) {
    ltBonus1000 = true;
  }
  if (bonus >= 2) {
    ltBonus2000 = true;
  }
  if (bonus >= 3) {
    ltBonus3000 = true;
  }
  if (bonus >= 4) {
    ltBonus4000 = true;
  }
  if (bonus >= 5) {
    ltBonus5000 = true;
  }
  if (bonus >= 6) {
    ltBonus6000 = true;
  }
  if (bonus >= 7) {
    ltBonus7000 = true;
  }
  if (bonus >= 8) {
    ltBonus8000 = true;
  }
  if (bonus >= 9) {
    ltBonus9000 = true;
  }
  if (bonus >= 10) {
    ltBonus10000 = true;
  }


  //////////////////////////////////
  if (oldScore != players[currentPlayer].score) {
    writeScore(players[currentPlayer].score);
    hasScoredThisRound = true;
    setPlayerLight();
  }
}

void writeScore(long score) {
  writeScore(score, true);
}

void writeScore(long score, bool sound) {
  writeDisplay(score);
  if(sound){playSFX(SOUND_POINT);}
}

void advanceBonus() {
  bonus++;
  if (bonus > 10) {
    bonus = 10;
  }
}

void extraBall() {
  ltSamePlayerShoots = true;
  extraBalls++;
  playSFX(SOUND_EXTRABALL);
}

// reset the game back to the initial state.
void reset() {
  for (int p = 0; p < 4; ++p) {
    players[p].score = 0;
    players[p].lightA = true;
    players[p].lightB = true;
    players[p].lightC = true;
    players[p].lightD = true;
  }

  amountOfPlayers = 1;
  currentPlayer = 0;

  players[currentPlayer].score = 0;
  oldScore = 0;
  balls = 1;
  extraBalls = 0;
  bonus = 0;
  tilted = false;
  isEndGame = false;
  hasScoredThisRound = false;

  ltA = ltB = ltC = ltD = true;
  lt1 = lt2 = lt3 = true;
  ltBonus1000 = ltBonus2000 = ltBonus3000 = ltBonus4000 = ltBonus5000 = false;
  ltBonus6000 = ltBonus7000 = ltBonus8000 = ltBonus9000 = ltBonus10000 = false;
  ltExtraBallRight = ltDoubleBonus = ltExtraBallLeft = ltSamePlayerShoots = ltTrippleBonus = false;
  ltBall1 = true;
  ltBall2 = ltBall3 = ltBall4 = ltBall5 = false;
  ltPlayer1 = true;
  ltPlayer2 = ltPlayer3 = ltPlayer4 = false;

  writeDisplay("NO SCORE");

  playSFX(SOUND_STARTUP);
}

void startGame() {

  if (isEndGame) {
    reset();
  }
  else if (!hasScoredThisRound) {
    amountOfPlayers++;
    if (amountOfPlayers > 4) {
      amountOfPlayers = 1;
    }

    if (amountOfPlayers == 1) {
      ltPlayer1 = true;
      ltPlayer4 = false;
    }
    else if (amountOfPlayers == 2) {
      ltPlayer2 = true;
      ltPlayer1 = false;
    }
    else if (amountOfPlayers == 3) {
      ltPlayer3 = true;
      ltPlayer2 = false;
    }
    else if (amountOfPlayers == 4) {
      ltPlayer4 = true;
      ltPlayer3 = false;
    }
  }


}

void setPlayerLight()
{
  ltPlayer1 = (currentPlayer == 0);
  ltPlayer2 = (currentPlayer == 1);
  ltPlayer3 = (currentPlayer == 2);
  ltPlayer4 = (currentPlayer == 3);
}

// The current player has no extra balls hit end of ball.
// Advance to the next player and/or next ball.
void switchPlayersOrAdvanceBall()
{
  players[currentPlayer].lightA = ltA;
  players[currentPlayer].lightB = ltB;
  players[currentPlayer].lightC = ltC;
  players[currentPlayer].lightD = ltD;

  ++currentPlayer;
  if (currentPlayer >= amountOfPlayers) {
    balls++;
    currentPlayer = 0;
  }

  ltA = players[currentPlayer].lightA;
  ltB = players[currentPlayer].lightB;
  ltC = players[currentPlayer].lightC;
  ltD = players[currentPlayer].lightD;
  oldScore = players[currentPlayer].score;

  setPlayerLight();
}

void endOfBall() {

  if (!hasScoredThisRound) {
    return; //Ball fireer has failed, so we should not count this as a new ball, we should keep trying to push the ball out of the shooter
  }

  long bonusAmount = 0;

  playSFX(SOUND_NEWBALL);

  if (!tilted) {

    if (ltDoubleBonus) {
      bonusAmount = bonus * 1000 * 2;
    }
    else if (ltTrippleBonus) {
      bonusAmount = bonus * 1000 * 3;
    }
    else {
      bonusAmount = bonus * 1000;
    }
  }


  lt1 = true;
  lt2 = true;
  lt3 = true;

  bonus = 0;

  ltBonus1000 = false;
  ltBonus2000 = false;
  ltBonus3000 = false;
  ltBonus4000 = false;
  ltBonus5000 = false;
  ltBonus6000 = false;
  ltBonus7000 = false;
  ltBonus8000 = false;
  ltBonus9000 = false;
  ltBonus10000 = false;

  ltExtraBallLeft = false;
  ltExtraBallRight = false;

  ltDoubleBonus = false;
  ltTrippleBonus = false;
  ltSamePlayerShoots = false;

  ltBall1 = false;
  ltBall2 = false;
  ltBall3 = false;
  ltBall4 = false;
  ltBall5 = false;


  while (pt.tune_playing) {
    /*do nothing*/
  }

  while (bonusAmount > 0) {
    delay(500);
    bonusAmount -= 1000;
    players[currentPlayer].score += 1000;
    writeScore(players[currentPlayer].score);
  }

  delay(1000);

  // Move to next ball or player.
  if (extraBalls == 0) {
    switchPlayersOrAdvanceBall();
  } else {
    extraBalls--;
  }

  switch (balls) {
    case 1: ltBall1  = true; break;
    case 2: ltBall2  = true; break;
    case 3: ltBall3  = true; break;
    case 4: ltBall4  = true; break;
    case 5: ltBall5  = true; break;
  }

  if (balls >= MAX_BALLS) {
    endGame();
    return;
  }
  hasScoredThisRound = false;
  digitalWrite(OUT_BALL_RETURN, HIGH);
  timeRetractBallReturn = millis() + /*SOLENOID_DELAY*/100;
  writeScore(players[currentPlayer].score);
  tilted = false;


}

void endGame(){

  isEndGame = true;
  playSFX(SOUND_SCORE_AMATEUR);

  while (pt.tune_playing) {
    //Do display animation
    for(int i = 0; i < amountOfPlayers; i++){
      Player p = players[i];
      writeScore(p.score, false);
      delay(1000);
      writeDisplay(getRankingTitle(p.score));
      delay(1000);
    }
  }
  delay(3000);
}

String getRankingTitle(int score) {
  if (players[currentPlayer].score < 40000) {
    return "AMATEUR";
  }
  else if (players[currentPlayer].score < 75000) {
    return"SUPER";
  }
  else if (players[currentPlayer].score < 100000) {
    return"CHAMP";
  }
  else if (players[currentPlayer].score < 125000) {
    return"FANTASTIC";
  }
  else if (players[currentPlayer].score < 150000) {
    return"WIZARD";
  }
  else if (players[currentPlayer].score > 150000) {
    return"FIREBALL";
  }
}

void tilt() {
  playSFX(SOUND_TILT);
  writeDisplay("TILTED");
}

void timerBlinkExtraBallLeft(void) {
  ltExtraBallLeft = !ltExtraBallLeft;
}

void timerBlinkExtraBallRight(void) {
  ltExtraBallRight = !ltExtraBallRight;
}

void playSFX(byte* sfx) {
  if (!pt.tune_playing) {
    pt.tune_playscore(sfx);
  }
}

void writeDisplay(long num) {
  char buffer[9];
  long2text(num, buffer);
  writeDisplay(buffer);
}

void writeDisplay(String msg) {
  clearDisplay(); //Not sure if I need to clear every time, Long run?
  for (int i = 0; i < 8; i++) {
    if (i < msg.length()) {
      writeDisplay(i, msg.charAt(i));
    }
  }
  updateDisplay(); //Just like clear, should we auto update in long run?
}


//0-7
void writeDisplay(int place, char in) {
  if (isprint(in)) {
    if (place > 3) {
      display2.writeDigitAscii(place - 4, in);
    }
    else {
      display1.writeDigitAscii(place, in);
    }
  }
}

void updateDisplay() {
  display1.writeDisplay();
  display2.writeDisplay();
}

void clearDisplay() {
  display1.clear();
  display2.clear();
  updateDisplay();
}

