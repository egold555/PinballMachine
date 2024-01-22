#ifndef PINBALL_MACHINE__H_
#define PINBALL_MACHINE__H_

#include "Constants.h"
#include "Button.h"

// Music
#include <Playtune.h>

// Display
#include <SPI.h>                  //Display
#include <Wire.h>                 //Display
#include <Adafruit_GFX.h>         //Display
#include <Adafruit_LEDBackpack.h> //Display
#include <long2text.h>            //Peter

class PinballMachine
{
public:
    PinballMachine();     // constructor
    bool mx0_lt0 = false; // NONE
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
    bool mx5_lt3 = false;   // NONE
    bool ltBall2 = false;   // b2
    bool ltBall3 = false;   // b3
    bool ltBall4 = false;   // b4
    bool ltBall5 = false;   // b5
    bool ltPlayer1 = false; // p1
    bool ltPlayer2 = false; // p2
    bool ltPlayer3 = false; // p3
    bool ltPlayer4 = false; // p4
    bool mx8_lt0 = false;
    bool mx8_lt1 = false;
    bool mx8_lt2 = false;
    bool ltBall1 = true; // b1

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
    // Button mx4_sw1;
    Button swRightTarget;
    Button swRightSlingShot;
    Button swCenterTarget;
    // Button mx5_sw1;
    Button swRightBumper;
    Button swRightAdvanceLane;
    Button swFlipperLeft;
    Button swFlipperRight;

    void setup();
    void loop();
    void playSFX(const byte *sfx);
    void stopSFX();
    bool isSFXPlaying();

    void writeDisplay(long num);
    void writeDisplay(String msg);
    void writeDisplay(int place, char in);
    void updateDisplay();
    void clearDisplay();

    void scrollText(String msg);
    void resetScrollTextPosition();

private:
    Playtune pt;
    // The matrix display
    Adafruit_AlphaNum4 display1 = Adafruit_AlphaNum4();
    Adafruit_AlphaNum4 display2 = Adafruit_AlphaNum4();

    long msgCount = 0;
    int msgPos = 0;
};

#endif // PINBALL_MACHINE__H_