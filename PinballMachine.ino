#include <Playtune.h> //Music http://blog.bentgeorge.com/?p=119
#include "Adafruit_LEDBackpack.h" //Display

const int SOLENOID_DELAY = 50; //60
const int SWITCH_READ_DELAY = 500;
const int ANALOG_THRESHOLD = 200;

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

const unsigned long DEBOUNCE_DELAY = 90;


bool swBallReturn = false;
bool swTilt = false;
bool swRightSpinner = false;
bool swRightExtraBallLane = false;
bool swA = false;
bool swStart = false;
bool swLeftTarget = false;
bool swLeftSlingShot = false;
bool swB = false;
bool swLeftThumperBumper = false;
bool swLeftBumper = false;
bool swLeftExtraBallLane = false;
bool swC = false;
bool swRightThumperBumper = false;
bool swLeftSpinner = false;
bool swLeftAdvanceLane = false;
bool swD = false;
//bool mx4_sw1 = false;
bool swRightTarget = false;
bool swRightSlingShot = false;
bool swCenterTarget = false;
//bool mx5_sw1 = false;
bool swRightBumper = false;
bool swRightAdvanceLane = false;

bool prBallReturn = false;
bool prTilt = false;
bool prRightSpinner = false;
bool prRightExtraBallLane = false;
bool prA = false;
bool prStart = false;
bool prLeftTarget = false;
bool prLeftSlingShot = false;
bool prB = false;
bool prLeftThumperBumper = false;
bool prLeftBumper = false;
bool prLeftExtraBallLane = false;
bool prC = false;
bool prRightThumperBumper = false;
bool prLeftSpinner = false;
bool prLeftAdvanceLane = false;
bool prD = false;
//bool mx4_sw1 = false;
bool prRightTarget = false;
bool prRightSlingShot = false;
bool prCenterTarget = false;
//bool mx5_sw1 = false;
bool prRightBumper = false;
bool prRightAdvanceLane = false;

unsigned long tmBallReturn;
unsigned long tmTilt;
unsigned long tmRightSpinner;
unsigned long tmRightExtraBallLane;
unsigned long tmA;
unsigned long tmStart;
unsigned long tmLeftTarget;
unsigned long tmLeftSlingShot;
unsigned long tmB;
unsigned long tmLeftThumperBumper;
unsigned long tmLeftBumper;
unsigned long tmLeftExtraBallLane;
unsigned long tmC;
unsigned long tmRightThumperBumper;
unsigned long tmLeftSpinner;
unsigned long tmLeftAdvanceLane;
unsigned long tmD;
unsigned long tmRightTarget;
unsigned long tmRightSlingShot;
unsigned long tmCenterTarget;
unsigned long tmRightBumper;
unsigned long tmRightAdvanceLane;

bool mx0_lt0 = false; //NONE
bool ltBonus8000 = false;
bool ltBonus9000 = false;
bool ltExtraBallRight = false;
bool ltA = true;
bool ltBonus6000 = false;
bool lt1 = false;
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
bool lt2 = false;
bool ltSamePlayerShoots = false;
bool lt3 = false;
bool ltBonus4000 = false;
bool ltTrippleBonus = false;
bool mx5_lt3 = false; //NONE


// Time (as returned by millis()) that solenoid should retract.
unsigned long timeRetractLeftSlingshot = 0, timeRetractRightSlingshot = 0;
unsigned long timeRetractLeftThumperBumper = 0, timeRetractRightThumperBumper = 0;
unsigned long timeRetractBallReturn = 0;

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
  if (swLeftSlingShot) {
    digitalWrite(OUT_SLINGSHOT_LEFT, HIGH);
    timeRetractLeftSlingshot = currentTime + SOLENOID_DELAY;
  }

  if (swRightSlingShot) {
    digitalWrite(OUT_SLINGSHOT_RIGHT, HIGH);
    timeRetractRightSlingshot = currentTime + SOLENOID_DELAY;
  }

  if (swLeftThumperBumper) {
    digitalWrite(OUT_THUMPER_LEFT, HIGH);
    timeRetractLeftThumperBumper = currentTime + SOLENOID_DELAY;
  }

  if (swRightThumperBumper) {
    digitalWrite(OUT_THUMPER_RIGHT, HIGH);
    timeRetractRightThumperBumper = currentTime + SOLENOID_DELAY;
  }

  if (swBallReturn) {
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

  debounceSwitch(IN_SW0, &swBallReturn, &prBallReturn, &tmBallReturn);
  debounceSwitch(IN_SW1, &swTilt, &prTilt, &tmTilt);
  debounceSwitch(IN_SW2, &swRightSpinner, &prRightSpinner, &tmRightSpinner);
  debounceSwitch(IN_SW3, &swRightExtraBallLane, &prRightExtraBallLane, &tmRightExtraBallLane);

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

  debounceSwitch(IN_SW0, &swA, &prA, &tmA);
  debounceSwitch(IN_SW1, &swStart, &prStart, &tmStart);
  debounceSwitch(IN_SW2, &swLeftTarget, &prLeftTarget, &tmLeftTarget);
  debounceSwitch(IN_SW3, &swLeftSlingShot, &prLeftSlingShot, &tmLeftSlingShot);

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

  debounceSwitch(IN_SW0, &swB, &prB, &tmB);
  debounceSwitch(IN_SW1, &swLeftThumperBumper, &prLeftThumperBumper, &tmLeftThumperBumper);
  debounceSwitch(IN_SW2, &swLeftBumper, &prLeftBumper, &tmLeftBumper);
  debounceSwitch(IN_SW3, &swLeftExtraBallLane, &prLeftExtraBallLane, &tmLeftExtraBallLane);

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

  debounceSwitch(IN_SW0, &swC, &prC, &tmC);
  debounceSwitch(IN_SW1, &swRightThumperBumper, &prRightThumperBumper, &tmRightThumperBumper);
  debounceSwitch(IN_SW2, &swLeftSpinner, &prLeftSpinner, &tmLeftSpinner);
  debounceSwitch(IN_SW3, &swLeftAdvanceLane, &prLeftAdvanceLane, &tmLeftAdvanceLane);

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

  debounceSwitch(IN_SW0, &swD, &prD, &tmD);
  //debounceSwitch(IN_SW1, &mx4_sw1, &prmx4_sw1);
  debounceSwitch(IN_SW2, &swRightTarget, &prRightTarget, &tmRightTarget);
  debounceSwitch(IN_SW3, &swRightSlingShot, &prRightSlingShot, &tmRightSlingShot);

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

  debounceSwitch(IN_SW0, &swCenterTarget, &prCenterTarget, &tmCenterTarget);
  //debounceSwitch(IN_SW1, &mx5_sw1, &prmx5_sw1);
  debounceSwitch(IN_SW2, &swRightBumper, &prRightBumper, &tmRightBumper);
  debounceSwitch(IN_SW3, &swRightAdvanceLane, &prRightAdvanceLane, &tmRightAdvanceLane);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX5, LOW);

}

void updateScore() {
  long oldScore = score;

  if (prStart) {
    score = 0;
  }

  //ThumperBumpers +100
  if (prLeftThumperBumper || prRightThumperBumper) {
    score += 100;
  }

  //Spinners +100
  if (prLeftSpinner || prRightSpinner) {
    score += 100;
  }

  //Extra ball lanes and advance lanes +500
  if (prLeftExtraBallLane || prLeftAdvanceLane || prRightExtraBallLane || prRightAdvanceLane) {
    score += 500;
  }

  //ABCD  +1000
  if (prA || prB || prC || prD) {
    score += 1000;
  }

  //trigger lights for ABCD
  if (prA) {
    ltA = !ltA;
  }
  if (prB) {
    ltB = !ltB;
  }
  if (prC) {
    ltC = !ltC;
  }
  if (prD) {
    ltD = !ltD;
  }

  //Advance bonus if ABCD is all off
  if (!ltA && !ltB && !ltC && !ltD) {
    advanceBonus();
    ltA = true;
    ltB = true;
    ltC = true;
    ltD = true;
  }

  //Left & Right Targer +500
  if (prLeftTarget || prRightTarget) {
    score += 500;
    advanceBonus();
    advanceBonus();
  }

  //Left and Right Bumper +50
  if (prLeftBumper || prRightBumper) {
    score += 50;
    advanceBonus();
  }

  if (prCenterTarget) {
    score += 1000;
    advanceBonus();
    advanceBonus();
  }




  //////////////////////////////////
  if (oldScore != score) {
    writeDisplay(score);
  }
}

void advanceBonus() {

}



void debounceSwitch(int switchNumber, bool * sw, bool * pr, unsigned long * lasttm) {

  bool oldSw = *sw;
  bool newSw;
  newSw = analogRead(switchNumber) > ANALOG_THRESHOLD;
  *sw = newSw;

  *pr = false;
  if (newSw && !oldSw) {
    unsigned long currtm = millis();
    if (currtm - *lasttm > DEBOUNCE_DELAY) {
      *pr = true;
    }
    *lasttm = currtm;
  }
}
void writeDisplay(long num) {
  writeDisplay(String(num));
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

