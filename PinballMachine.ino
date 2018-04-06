#include <Playtune.h> //Music http://blog.bentgeorge.com/?p=119
#include "Adafruit_LEDBackpack.h" //Display
#include "long2text.h"

const int SOLENOID_DELAY = 50; //60
const int SWITCH_READ_DELAY = 500;
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

const int OUT_LT0 = 36;
const int OUT_LT1 = 37;
const int OUT_LT2 = 38;
const int OUT_LT3 = 39;


const int IN_SW0 = A12; //36
const int IN_SW1 = A13; //37
const int IN_SW2 = A14; //38
const int IN_SW3 = A15; //39

const int NOTE_1 = 44;
const int NOTE_2 = 45;
const int NOTE_3 = 46;

byte bonus = 0;

typedef struct Button {
  public:
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


// Time (as returned by millis()) that solenoid should retract.
unsigned long timeRetractLeftSlingshot = 0, timeRetractRightSlingshot = 0;
unsigned long timeRetractLeftThumperBumper = 0, timeRetractRightThumperBumper = 0;
unsigned long timeRetractBallReturn = 0;
unsigned long timeNextSpinnerScore = 0;


Adafruit_AlphaNum4 display1 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 display2 = Adafruit_AlphaNum4();
Playtune pt;



const byte PROGMEM SOUND_STARTUP [] = {
  0x91, 58, 0x92, 70, 0, 160, 0x81, 0x82, 0, 60, 0x91, 58, 0x92, 70, 0, 160, 0x81, 0x82, 0, 64, 0x91, 60,
  0x92, 72, 0, 160, 0x81, 0x82, 0, 64, 0x91, 65, 0x92, 77, 5, 192, 0x80, 0x81, 0x82, 0xf0
};

//const byte PROGMEM SOUND_POINT [] = {
//  0x92, 72, 0, 160, 0x81, 0x80, 0x81, 0x82, 0xf0
//};

long score = 0;

void setup() {

  Serial.begin(57600);

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

  pinMode(OUT_LT0, OUTPUT);
  pinMode(OUT_LT1, OUTPUT);
  pinMode(OUT_LT2, OUTPUT);
  pinMode(OUT_LT3, OUTPUT);

  pinMode(IN_SW0, INPUT);
  pinMode(IN_SW1, INPUT);
  pinMode(IN_SW2, INPUT);
  pinMode(IN_SW3, INPUT);

  pt.tune_initchan (NOTE_1);
  pt.tune_initchan (NOTE_2);
  pt.tune_initchan (NOTE_3);

  display1.begin(0x70);
  display2.begin(0x71);
  writeDisplay("NO SCORE");

  pt.tune_playscore(SOUND_STARTUP);
}



void loop() {
  if (!pt.tune_playing) {
    //pt.tune_playscore(score);
  }
  checkSwitchesAndLightLights();
  solinoids();
  updateScore();

  //scrollTextTest();
}

//long MSG_DELAY = 20;
//long msgCount = 0;
//int msgPos = 0;
//String msg = "He's a pin ball wizard          There has got to be a twist         A pin ball wizard,           S'got such a supple wrist        How do you think he does it? I don't know!       What makes him so good?     # Ok Please Press Start #       ";
//void scrollTextTest() {
//  int len = msg.length();
//  if ((msgCount % MSG_DELAY) == 0) {
//    clearDisplay();
//    writeDisplay(0, msg.charAt((msgPos) % len));
//    writeDisplay(1, msg.charAt((msgPos + 1) % len));
//    writeDisplay(2, msg.charAt((msgPos + 2) % len));
//    writeDisplay(3, msg.charAt((msgPos + 3) % len));
//    writeDisplay(4, msg.charAt((msgPos + 4) % len));
//    writeDisplay(5, msg.charAt((msgPos + 5) % len));
//    writeDisplay(6, msg.charAt((msgPos + 6) % len));
//    writeDisplay(7, msg.charAt((msgPos + 7) % len));
//    updateDisplay();
//
//    msgPos++;
//    if (msgPos == len) {
//      msgPos = 0;
//    }
//  }
//
//  msgCount++;
//
//}

void solinoids() {
  unsigned long currentTime = millis();

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

  if (swBallReturn.sw) {
    digitalWrite(OUT_BALL_RETURN, HIGH);
    timeRetractBallReturn = currentTime + /*SOLENOID_DELAY*/100;
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

}

void updateScore() {
  long oldScore = score;

  if (swStart.pr) {
    asm volatile ("  jmp 0"); //Restarts the arduino. Not the most elegant, but it works for now
  }

  if (swBallReturn.pr) {
    endOfBall();
  }

  //ThumperBumpers +100
  if (swLeftThumperBumper.pr || swRightThumperBumper.pr) {
    score += 100;
  }

  //Spinners +100
  if (swLeftSpinner.sw || swRightSpinner.sw) {
    unsigned long currTime = millis();
    if (currTime > timeNextSpinnerScore) {
      score += 100;
      timeNextSpinnerScore = currTime + SPINNER_SCORE_DELAY;
    }
  }

  //Extra ball lanes and advance lanes +500
  if (swLeftExtraBallLane.pr || swRightExtraBallLane.pr) {
    score += 500;
  }

  if (swLeftExtraBallLane.pr && ltExtraBallLeft) {
    extraBall();
    ltExtraBallLeft = false;
  }

  if (swRightExtraBallLane.pr && ltExtraBallRight) {
    extraBall();
    ltExtraBallRight = false;
  }

  if (swLeftAdvanceLane.pr || swRightAdvanceLane.pr) {
    advanceBonus();
    score += 500;
  }

  //ABCD  +1000
  if (swA.pr || swB.pr || swC.pr || swD.pr) {
    score += 1000;
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
    ltA = true;
    ltB = true;
    ltC = true;
    ltD = true;
    score += 25000;
    ltExtraBallLeft = true;
    ltExtraBallRight = true;
  }

  //Left and Right Bumper +50
  if (swLeftBumper.pr || swRightBumper.pr) {
    score += 50;
  }

  //Targets

  if (swLeftTarget.pr || swRightTarget.pr || swCenterTarget.pr) {
    score += 500;
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
    score += 500; //Accounting fo 500 of above function
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
  if (oldScore != score) {
    writeDisplay(score);
  }
}

void advanceBonus() {
  bonus++;
  if (bonus > 10) {
    bonus = 10;
  }
}

void extraBall() {
  ltSamePlayerShoots = true;
}

void endOfBall() {
  if (ltDoubleBonus) {
    score += bonus * 1000 * 2;
  }
  else if (ltTrippleBonus) {
    score += bonus * 1000 * 3;
  }
  else {
    score += bonus * 1000;
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

