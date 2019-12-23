//in VS CODE, set '"C_Cpp.intelliSenseEngine": "Tag Parser",' in the settings json file in appdata
//https://github.com/microsoft/vscode-arduino/issues/438#issuecomment-343820369

//imports
#include "Playtune.h" //Music http://blog.bentgeorge.com/?p=119
#include "Adafruit_LEDBackpack.h" //Display
#include "long2text.h" //Peter
#include "run.h" //Modified version of https://github.com/billroy/run
#include <EEPROM.h>

//Constants for delays ahs such. 
//TODO: DOcument more
const int SOLENOID_DELAY = 50; //60
const int SWITCH_READ_DELAY = 800; //680
const int ANALOG_THRESHOLD = 200;
const int SPINNER_SCORE_DELAY = 75;
const unsigned int DEBOUNCE_DELAY = 20;

//Constants for pins
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

const int IN_SW0 = A12;
const int IN_SW1 = A13;
const int IN_SW2 = A14;
const int IN_SW3 = A15;

const int IN_FLIPPER_LEFT = 2;
const int IN_FLIPPER_RIGHT = 3;

const int NOTE_1 = 44;
const int NOTE_2 = 45;
const int NOTE_3 = 46;

//What state is the game in
typedef enum GameState {
  GS_BEFORE,
  GS_PLAYING,
  GS_GAMEOVER,
  GS_HIGHSCORE
};

//defines a button on the pinball machine
typedef struct Button {
  boolean sw = false; //Switch
  boolean pr = false; //Pressed
  long tm = 0; //Time

  /**
   * magical debouncing code that works and should never be touched
   * 
   * @param pin pin to debounce
   */
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

  /**
    * magical debouncing code that works and should never be touched
    * 
    * @param pin pin to debounce
    */
  void debounceDigital(int pin) {
    bool oldSw = sw;
    bool newSw;
    newSw = digitalRead(pin) == LOW;
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

//Defines a player. 1,2,3,4
typedef struct Player {
  bool lightA;
  bool lightB;
  bool lightC;
  bool lightD;
  long score;
} Player;

//Every button on the pinball machine
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
Button swFlipperLeft;
Button swFlipperRight;

//all the lights on the pinball machine
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

//The matrix display
Adafruit_AlphaNum4 display1 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 display2 = Adafruit_AlphaNum4();

//Music player
Playtune pt;

//Current game state we are in
GameState state = GS_BEFORE;

///////////////////////////////// [ MUSIC / SFX ] /////////////////////////////////
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

const byte PROGMEM SOUND_ENDING_SONG [] = {
  0, 2, 0x90, 62, 0x91, 62, 1, 26, 0x92, 64, 0, 10, 0x80, 0x81, 1, 9, 0x90, 66, 0x91, 60, 0, 10,
  0x82, 1, 7, 0x92, 67, 0, 9, 0x80, 0x81, 1, 18, 0x90, 69, 0x91, 60, 0, 10, 0x82, 1, 9, 0x92, 70,
  0, 10, 0x80, 0x81, 1, 7, 0x90, 71, 0x91, 62, 0, 9, 0x82, 0, 136, 0x80, 0x81, 0, 0, 0x90, 71,
  0x91, 62, 0, 137, 0x80, 0x81, 0, 0, 0x90, 71, 0x91, 62, 0, 108, 0x80, 0, 23, 0, 24, 0x81, 0, 25,
  0, 49, 0, 45, 0x90, 64, 0x91, 55, 0x92, 71, 0, 4, 0, 49, 0, 49, 0, 3, 0x82, 0x81, 0, 47,
  0, 0, 0x80, 0, 49, 0, 72, 0, 1, 0x90, 65, 0x91, 55, 0x92, 71, 1, 159, 0, 49, 0, 49,
  0, 45, 0x80, 0x82, 0, 0, 0x90, 67, 0, 4, 0, 50, 0, 50, 0, 50, 0, 50, 0, 73, 0x80,
  0x81, 0, 1, 0x90, 67, 0x91, 48, 0x92, 76, 0, 70, 0, 95, 0x81, 0, 119, 0x91, 55, 0, 155, 0x81,
  0, 119, 0x91, 60, 0, 106, 0x81, 0, 166, 0x82, 0, 1, 0x91, 47, 0x92, 66, 0, 31, 0x80, 0, 132,
  0x81, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 59, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 48,
  0x91, 67, 0, 31, 0x82, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106,
  0x80, 0, 144, 0x81, 0, 21, 0, 1, 0x90, 52, 0, 146, 0x91, 67, 0, 17, 0x80, 0, 119, 0x90, 69,
  0x92, 55, 0, 16, 0x81, 0, 121, 0x91, 71, 0, 16, 0x80, 0, 1, 0x82, 0, 119, 0x90, 72, 0x92, 60,
  0, 16, 0x81, 0, 89, 0x82, 0, 29, 0x91, 74, 0, 16, 0x80, 0, 121, 0x90, 48, 0x92, 67, 0, 15,
  0x81, 0, 148, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 166, 0, 1,
  0x90, 47, 0x91, 66, 0, 31, 0x82, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0, 0,
  0x90, 59, 0x92, 69, 0, 21, 0x81, 0, 84, 0x80, 0, 166, 0, 1, 0x90, 48, 0x91, 67, 0, 9, 0x82,
  0, 154, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 144, 0x81, 0, 23,
  0x90, 52, 0, 164, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 29,
  0x90, 67, 0, 138, 0x91, 50, 0x92, 65, 0, 15, 0x80, 0, 148, 0x81, 0, 119, 0x90, 55, 0, 155, 0x80,
  0, 119, 0x90, 59, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 49, 0x91, 64, 0, 31, 0x82, 0, 132, 0x80,
  0, 119, 0x90, 54, 0, 155, 0x80, 0, 119, 0x90, 58, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 50, 0x92, 65,
  0, 31, 0x81, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 59, 0, 106, 0x80, 0, 144,
  0x82, 0, 21, 0, 1, 0x90, 47, 0, 146, 0x91, 67, 0, 17, 0x80, 0, 119, 0x90, 69, 0x92, 55, 0, 16,
  0x81, 0, 121, 0x91, 71, 0, 16, 0x80, 0, 1, 0x82, 0, 119, 0x90, 72, 0x92, 59, 0, 16, 0x81, 0, 89,
  0x82, 0, 29, 0x91, 73, 0, 16, 0x80, 0, 121, 0x90, 50, 0x92, 65, 0, 15, 0x81, 0, 148, 0x80, 0, 119,
  0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 59, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 47, 0x91, 59, 0, 31,
  0x82, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0, 0, 0x90, 59, 0x92, 69, 0, 21,
  0x81, 0, 84, 0x80, 0, 166, 0, 1, 0x90, 48, 0x91, 67, 0, 9, 0x82, 0, 154, 0x80, 0, 119, 0x90, 55,
  0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 144, 0x81, 0, 23, 0x90, 43, 0, 164, 0x80, 0, 119,
  0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 29, 0x90, 67, 0, 138, 0x91, 48, 0x92, 76,
  0, 15, 0x80, 0, 148, 0x81, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 64, 0, 106, 0x80, 0, 166,
  0, 1, 0x90, 47, 0x91, 74, 0, 31, 0x82, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119,
  0x90, 62, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 46, 0x92, 73, 0, 31, 0x81, 0, 132, 0x80, 0, 119,
  0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 61, 0, 106, 0x80, 0, 144, 0x82, 0, 21, 0, 1, 0x90, 49,
  0x91, 79, 0, 164, 0x80, 0, 119, 0x90, 81, 0x92, 55, 0, 10, 0x81, 0, 145, 0x82, 0x80, 0, 119, 0x90, 64,
  0, 106, 0x80, 0, 29, 0x90, 79, 0, 138, 0x91, 50, 0x92, 74, 0, 15, 0x80, 0, 148, 0x81, 0, 119,
  0x90, 57, 0, 155, 0x80, 0, 119, 0x90, 65, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 49, 0x91, 73, 0, 31,
  0x82, 0, 132, 0x80, 0, 119, 0x90, 57, 0, 155, 0x80, 0, 119, 0x90, 65, 0, 106, 0x80, 0, 166, 0, 1,
  0x90, 48, 0x92, 72, 0, 31, 0x81, 0, 132, 0x80, 0, 119, 0x90, 57, 0, 155, 0x80, 0, 119, 0x90, 65,
  0, 106, 0x80, 0, 166, 0x82, 0, 1, 0x90, 47, 0x91, 77, 0, 164, 0x80, 0, 119, 0x90, 79, 0x92, 55,
  0, 10, 0x81, 0, 145, 0x82, 0x80, 0, 119, 0x90, 65, 0, 106, 0x80, 0, 29, 0x90, 77, 0, 138, 0x91, 36,
  0x92, 60, 0, 15, 0x80, 0, 148, 0x81, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 64, 0, 106,
  0x80, 0, 166, 0, 1, 0x90, 43, 0x91, 65, 0, 31, 0x82, 0, 132, 0x80, 0, 119, 0, 0, 0x90, 55,
  0x92, 67, 0, 10, 0x81, 0, 98, 0x80, 0, 23, 0, 49, 0, 49, 0, 45, 0, 0, 0x90, 55, 0x91, 71,
  0, 4, 0, 6, 0x82, 0, 43, 0, 50, 0, 50, 0, 0, 0x80, 0, 50, 0, 73, 0x81, 0, 1,
  0x90, 53, 0x91, 71, 0x92, 76, 0, 70, 0, 77, 0x81, 0x82, 0x80, 0, 0, 0x90, 71, 0x91, 53, 0x92, 76,
  0, 137, 0x81, 0x80, 0x82, 0, 0, 0x90, 53, 0x91, 71, 0x92, 76, 1, 155, 0x81, 0x82, 0, 0, 0x91, 47,
  0x92, 65, 0, 16, 0x80, 0, 120, 0, 1, 0x90, 48, 0, 15, 0x81, 0x82, 3, 48, 0x80, 0, 0, 0, 2,
  0x90, 62, 0x91, 62, 1, 26, 0x92, 64, 0, 10, 0x80, 0x81, 1, 9, 0x90, 66, 0x91, 60, 0, 10, 0x82,
  1, 7, 0x92, 67, 0, 9, 0x80, 0x81, 1, 18, 0x90, 69, 0x91, 60, 0, 10, 0x82, 1, 9, 0x92, 70,
  0, 10, 0x80, 0x81, 1, 7, 0x90, 71, 0x91, 62, 0, 9, 0x82, 0, 136, 0x80, 0x81, 0, 0, 0x90, 71,
  0x91, 62, 0, 137, 0x80, 0x81, 0, 0, 0x90, 71, 0x91, 62, 0, 108, 0x80, 0, 23, 0, 24, 0x81, 0, 25,
  0, 49, 0, 45, 0x90, 64, 0x91, 55, 0x92, 71, 0, 4, 0, 49, 0, 49, 0, 3, 0x82, 0x81, 0, 47,
  0, 0, 0x80, 0, 49, 0, 72, 0, 1, 0x90, 65, 0x91, 55, 0x92, 71, 1, 159, 0, 49, 0, 49,
  0, 45, 0x80, 0x82, 0, 0, 0x90, 67, 0, 4, 0, 50, 0, 50, 0, 50, 0, 50, 0, 73, 0x80,
  0x81, 0, 1, 0x90, 67, 0x91, 48, 0x92, 76, 0, 70, 0, 95, 0x81, 0, 119, 0x91, 55, 0, 155, 0x81,
  0, 119, 0x91, 60, 0, 106, 0x81, 0, 166, 0x82, 0, 1, 0x91, 47, 0x92, 66, 0, 31, 0x80, 0, 132,
  0x81, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 59, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 48,
  0x91, 67, 0, 31, 0x82, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106,
  0x80, 0, 144, 0x81, 0, 21, 0, 1, 0x90, 52, 0, 146, 0x91, 67, 0, 17, 0x80, 0, 119, 0x90, 69,
  0x92, 55, 0, 16, 0x81, 0, 121, 0x91, 71, 0, 16, 0x80, 0, 1, 0x82, 0, 119, 0x90, 72, 0x92, 60,
  0, 16, 0x81, 0, 89, 0x82, 0, 29, 0x91, 74, 0, 16, 0x80, 0, 121, 0x90, 48, 0x92, 67, 0, 15,
  0x81, 0, 148, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 166, 0, 1,
  0x90, 47, 0x91, 66, 0, 31, 0x82, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0, 0,
  0x90, 59, 0x92, 69, 0, 21, 0x81, 0, 84, 0x80, 0, 166, 0, 1, 0x90, 48, 0x91, 67, 0, 9, 0x82,
  0, 154, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 144, 0x81, 0, 23,
  0x90, 52, 0, 164, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 29,
  0x90, 67, 0, 138, 0x91, 50, 0x92, 65, 0, 15, 0x80, 0, 148, 0x81, 0, 119, 0x90, 55, 0, 155, 0x80,
  0, 119, 0x90, 59, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 49, 0x91, 64, 0, 31, 0x82, 0, 132, 0x80,
  0, 119, 0x90, 54, 0, 155, 0x80, 0, 119, 0x90, 58, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 50, 0x92, 65,
  0, 31, 0x81, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 59, 0, 106, 0x80, 0, 144,
  0x82, 0, 21, 0, 1, 0x90, 47, 0, 146, 0x91, 67, 0, 17, 0x80, 0, 119, 0x90, 69, 0x92, 55, 0, 16,
  0x81, 0, 121, 0x91, 71, 0, 16, 0x80, 0, 1, 0x82, 0, 119, 0x90, 72, 0x92, 59, 0, 16, 0x81, 0, 89,
  0x82, 0, 29, 0x91, 73, 0, 16, 0x80, 0, 121, 0x90, 50, 0x92, 65, 0, 15, 0x81, 0, 148, 0x80, 0, 119,
  0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 59, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 47, 0x91, 59, 0, 31,
  0x82, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0, 0, 0x90, 59, 0x92, 69, 0, 21,
  0x81, 0, 84, 0x80, 0, 166, 0, 1, 0x90, 48, 0x91, 67, 0, 9, 0x82, 0, 154, 0x80, 0, 119, 0x90, 55,
  0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 144, 0x81, 0, 23, 0x90, 43, 0, 164, 0x80, 0, 119,
  0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 29, 0x90, 67, 0, 138, 0x91, 48, 0x92, 76,
  0, 15, 0x80, 0, 148, 0x81, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 64, 0, 106, 0x80, 0, 166,
  0, 1, 0x90, 47, 0x91, 74, 0, 31, 0x82, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119,
  0x90, 62, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 46, 0x92, 73, 0, 31, 0x81, 0, 132, 0x80, 0, 119,
  0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 61, 0, 106, 0x80, 0, 144, 0x82, 0, 21, 0, 1, 0x90, 49,
  0x91, 79, 0, 164, 0x80, 0, 119, 0x90, 81, 0x92, 55, 0, 10, 0x81, 0, 145, 0x82, 0x80, 0, 119, 0x90, 64,
  0, 106, 0x80, 0, 29, 0x90, 79, 0, 138, 0x91, 50, 0x92, 74, 0, 15, 0x80, 0, 148, 0x81, 0, 119,
  0x90, 57, 0, 155, 0x80, 0, 119, 0x90, 65, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 49, 0x91, 73, 0, 31,
  0x82, 0, 132, 0x80, 0, 119, 0x90, 57, 0, 155, 0x80, 0, 119, 0x90, 65, 0, 106, 0x80, 0, 166, 0, 1,
  0x90, 48, 0x92, 72, 0, 31, 0x81, 0, 132, 0x80, 0, 119, 0x90, 57, 0, 155, 0x80, 0, 119, 0x90, 65,
  0, 106, 0x80, 0, 166, 0x82, 0, 1, 0x90, 47, 0x91, 77, 0, 164, 0x80, 0, 119, 0x90, 79, 0x92, 55,
  0, 10, 0x81, 0, 145, 0x82, 0x80, 0, 119, 0x90, 65, 0, 106, 0x80, 0, 29, 0x90, 77, 0, 138, 0x91, 36,
  0x92, 60, 0, 15, 0x80, 0, 148, 0x81, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 64, 0, 106,
  0x80, 0, 166, 0, 1, 0x90, 43, 0x91, 65, 0, 31, 0x82, 0, 132, 0x80, 0, 119, 0, 0, 0x90, 55,
  0x92, 67, 0, 10, 0x81, 0, 98, 0x80, 0, 23, 0, 49, 0, 49, 0, 45, 0, 0, 0x90, 55, 0x91, 71,
  0, 4, 0, 6, 0x82, 0, 43, 0, 50, 0, 50, 0, 0, 0x80, 0, 50, 0, 73, 0x81, 0, 1,
  0x90, 53, 0x91, 71, 0x92, 76, 0, 70, 0, 77, 0x81, 0x82, 0x80, 0, 0, 0x90, 71, 0x91, 53, 0x92, 76,
  0, 137, 0x81, 0x80, 0x82, 0, 0, 0x90, 53, 0x91, 71, 0x92, 76, 1, 155, 0x81, 0x82, 0, 0, 0x91, 47,
  0x92, 65, 0, 16, 0x80, 0, 120, 0, 1, 0x90, 48, 0, 15, 0x81, 0x82, 3, 48, 0x80, 0xf0
};

const byte PROGMEM SOUND_TILT [] = {
  0x90, 63, 0, 225, 0x80, 0, 112, 0x90, 69, 0x91, 66, 0, 225, 0x80, 0x81, 0, 112, 0x90, 63, 0, 225,
  0x80, 0, 112, 0x90, 72, 0x91, 66, 0, 225, 0x80, 0x81, 0xf0
};

///////////////////////////////// [ END MUSIC / SFX ] /////////////////////////////////

//player variables
Player players[4];
int amountOfPlayers = 0;
int currentPlayer = 0;

long oldScore = 0;
byte balls = 1;
byte extraBalls = 0;
byte bonus = 0;
const byte MAX_BALLS = 6; //starts at 1 so you have 5 balls
boolean tilted = false;
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
void lightPlayerLights(int num);
void setPlayerLight(void);
void switchPlayersOrAdvanceBall(void);
void endOfBall(void);
bool delayWithLights(int delayTime);
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

/**
 * Arduino setup function
 * TODO:
 */
void setup() {

  Serial.begin(57600);
  //Serial2.begin(115200); //Recieve ESP

  //init al the matrix's and the misc outputs
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

  //inputs
  pinMode(IN_FLIPPER_LEFT, INPUT_PULLUP);
  pinMode(IN_FLIPPER_RIGHT, INPUT_PULLUP);

  pinMode(IN_SW0, INPUT);
  pinMode(IN_SW1, INPUT);
  pinMode(IN_SW2, INPUT);
  pinMode(IN_SW3, INPUT);

  //init music generators voices's
  pt.tune_initchan(NOTE_1);
  pt.tune_initchan(NOTE_2);
  pt.tune_initchan(NOTE_3);

  //init the two displays
  display1.begin(0x70);
  display2.begin(0x71);

  reset();

  //clearDisplay(); //debug
}

char theChar = 'A';
int posToWriteAt = 0;

/**
 * Arduino loop
 * TODO: dd
 */
void loop() {
  runner();
  checkSwitchesAndLightLights();
  updateScore();
  solinoids();
  //scrollTextTest();

  // if (state == GS_BEFORE) {
  //   scrollText("PRESS START TO PLAY AGAIN           HIGH SCORE - (PLAYER) - (SCORE)            " );
  // }

   if(swFlipperLeft.pr){
     doTheDisplayThing();
     theChar--;
   }
  
   if(swFlipperRight.pr){
     doTheDisplayThing();
     theChar++;
   }

  //serial();

}

/**
 * WIP code to insert your name
 * 
 */
void doTheDisplayThing() {

  if (theChar > 'Z') {
    theChar = 'Z';
  }
  if (theChar < 'A') {
    theChar = 'A';
  }

  long place = (long) ((int)theChar);
  writeDisplay(place);
  writeDisplay(posToWriteAt, theChar); 
  updateDisplay();
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

/**
 * Scroll text accross the displadisplay
 * 
 * @param msg Text to display
 */
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
/**
 * Updates the solinoids states
 * 
 */
void solinoids() {

  unsigned long currentTime = millis();

  if (swBallReturn.sw && state != GS_GAMEOVER && state != GS_BEFORE) {
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

/**
 * Check and update the giant matrix's for all the lights and buttons
 * 
 */
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



  swFlipperLeft.debounceDigital(IN_FLIPPER_LEFT);
  swFlipperRight.debounceDigital(IN_FLIPPER_RIGHT);


  if (swTilt.sw && !tilted) {
    tilted = true;
    tilt();
  }

}

/**
 * update the currently playing player score
 * TODO
 */
void updateScore() {

  oldScore = players[currentPlayer].score;

  if (swStart.pr) {
    startGame();
  }

  if (swBallReturn.pr && state != GS_GAMEOVER) {
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

/**
 * Writes the score to the screen and plays the score sound
 * 
 * @param score 
 */
void writeScore(long score) {
  writeScore(score, true);
}

/**
 * Writes the score to the screen
 * 
 * @param score players score
 * @param sound should we play the score sound?
 */
void writeScore(long score, bool sound) {
  writeDisplay(score);
  if (sound) {
    playSFX(SOUND_POINT);
  }
}

/**
 * Advance the bonus up by one
 * 
 */
void advanceBonus() {
  bonus++;
  if (bonus > 10) {
    bonus = 10;
  }
}

/**
 * Give the player a extra ball
 * 
 */
void extraBall() {
  ltSamePlayerShoots = true;
  extraBalls++;
  playSFX(SOUND_EXTRABALL);
}

/**
 * reset the game back to the initial state.
 * 
 */
void reset() {
  for (int p = 0; p < 4; ++p) {
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

  ltA = ltB = ltC = ltD = true;
  lt1 = lt2 = lt3 = true;
  ltBonus1000 = ltBonus2000 = ltBonus3000 = ltBonus4000 = ltBonus5000 = false;
  ltBonus6000 = ltBonus7000 = ltBonus8000 = ltBonus9000 = ltBonus10000 = false;
  ltExtraBallRight = ltDoubleBonus = ltExtraBallLeft = ltSamePlayerShoots = ltTrippleBonus = false;
  ltBall1 = false;
  ltBall2 = ltBall3 = ltBall4 = ltBall5 = false;
  ltPlayer1 = false;
  ltPlayer2 = ltPlayer3 = ltPlayer4 = false;

  state = GS_BEFORE;

  playSFX(SOUND_STARTUP);
}

/**
 * Start a brand new game
 * 
 */
void startGame() {

  if (state == GS_BEFORE) {
    state = GS_PLAYING;
    ltBall1 = true;
    writeDisplay(0);
  }

  if (state == GS_GAMEOVER) {
    reset();
  }
  else if (!hasScoredThisRound && players[0].score == 0) {
    amountOfPlayers++;
    if (amountOfPlayers > 4) {
      amountOfPlayers = 1;
    }
    playSFX(SOUND_POINT);
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
void lightPlayerLights(int num) {
  ltPlayer1 = (num == 1);
  ltPlayer2 = (num == 2);
  ltPlayer3 = (num == 3);
  ltPlayer4 = (num == 4);
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

/**
 * When the ball goes into the "gutter"
 * 
 */
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


  //Wait until the music finishes playing
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

bool delayWithLights(int delayTime)
{
  long startTime = millis();
  long endTime = startTime + delayTime;
  while (millis() < endTime) {
    checkSwitchesAndLightLights();
    if (swStart.pr) {
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
void endGame() {
  bool endNow = false;
  state = GS_GAMEOVER;
  playSFX(SOUND_ENDING_SONG);

  ltBall1 = ltBall2 = ltBall3 = ltBall4 = ltBall5 = false;
  ltA = ltB = ltC = ltD = true;
  lt1 = lt2 = lt3 = true;

  while (pt.tune_playing && !endNow) {
    //Do display animation
    int animationDelay = 2000;
    for (int i = 0; i < amountOfPlayers; i++) {
      Player p = players[i];
      lightPlayerLights(i + 1);
      writeScore(p.score, false);
      endNow = delayWithLights(animationDelay);
      if (endNow)
        break;
      writeDisplay(getRankingTitle(p.score));
      endNow = delayWithLights(animationDelay);
      if (endNow)
        break;
    }
  }

  
  pt.tune_stopscore();
  lightPlayerLights(0);
  reset();

}

/**
 * Get the Ranking Title based off of your score
 * 
 * @param score players score
 * @return String their rank
 */
String getRankingTitle(long score) {
  if (score < 40000) {
    return "AMATEUR";
  }
  else if (score < 75000) {
    return"SUPER";
  }
  else if (score < 100000) {
    return"CHAMP";
  }
  else if (score < 125000) {
    return"FANTASTIC";
  }
  else if (score < 150000) {
    return"WIZARD";
  }
  else if (score > 150000) {
    return"FIREBALL";
  }
}

/**
 * Called if the machine is tilted during game play
 * 
 */
void tilt() {
  playSFX(SOUND_TILT);
  writeDisplay("TILTED");
}

/**
 * Blink the extra ball left light
 * 
 */
void timerBlinkExtraBallLeft(void) {
  ltExtraBallLeft = !ltExtraBallLeft;
}

/**
 * Blink the extra ball right light
 * 
 */
void timerBlinkExtraBallRight(void) {
  ltExtraBallRight = !ltExtraBallRight;
}

/**
 * Play the music / SFX music. Defined in the arrays above
 * 
 * @param sfx Array of music to play
 */
void playSFX(byte* sfx) {
  if (!pt.tune_playing) {
    pt.tune_playscore(sfx);
  }
}

/**
 * Write a long to the display
 * 
 * @param num number to display on screen
 */
void writeDisplay(long num) {
  char buffer[9];
  long2text(num, buffer);
  writeDisplay(buffer);
}

/**
 * Writes a string to the display(s)
 * 
 * @param msg String to write
 */
void writeDisplay(String msg) {
  clearDisplay(); //Not sure if I need to clear every time, Long run?
  for (int i = 0; i < 8; i++) {
    if (i < msg.length()) {
      writeDisplay(i, msg.charAt(i));
    }
  }
  updateDisplay(); //Just like clear, should we auto update in long run?
}

/**
 * Writes a character to a certian place on the display(s)
 * 
 * @param place Where to write it 0-7. Each display has 4 characters
 * @param in Character to display. Displays all ASCII and some unicode characters
 */
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

/**
 * Updates both display units as one display
 * 
 */
void updateDisplay() {
  display1.writeDisplay();
  display2.writeDisplay();
}

/**
 * Clears both display units as one display
 * 
 */
void clearDisplay() {
  display1.clear();
  display2.clear();
  updateDisplay();
}

