#ifndef PINBALL_MACHINE__H_
#define PINBALL_MACHINE__H_

#include "Pinouts.h"
#include "Button.h"

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

    void setup();
    void loop();
};

#endif // PINBALL_MACHINE__H_