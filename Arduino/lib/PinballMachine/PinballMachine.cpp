#include "PinballMachine.h"

// Constructor
PinballMachine::PinballMachine()
{
}

void PinballMachine::setup()
{
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

    // inputs
    pinMode(IN_FLIPPER_LEFT, INPUT_PULLUP);
    pinMode(IN_FLIPPER_RIGHT, INPUT_PULLUP);

    pinMode(IN_SW0, INPUT);
    pinMode(IN_SW1, INPUT);
    pinMode(IN_SW2, INPUT);
    pinMode(IN_SW3, INPUT);

    //  init music generators voices's
    pt.tune_initchan(NOTE_1);
    pt.tune_initchan(NOTE_2);
    pt.tune_initchan(NOTE_3);

    // init the two displays
    display1.begin(0x70);
    display2.begin(0x71);
}

void PinballMachine::loop()
{
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
    // mx4_sw1.debounce(IN_SW1);
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
    // mx5_sw1.debounce(IN_SW1);
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
}

void PinballMachine::playSFX(const byte *sfx)
{
    if (!PinballMachine::isSFXPlaying())
    {
        pt.tune_playscore((byte *)sfx);
    }
}

void PinballMachine::stopSFX()
{
    pt.tune_stopscore();
}

bool PinballMachine::isSFXPlaying()
{
    return pt.tune_playing;
}

void PinballMachine::writeDisplay(long num)
{
    char buffer[9];
    long2text(num, buffer);
    PinballMachine::writeDisplay(buffer);
}

void PinballMachine::writeDisplay(String msg)
{
    PinballMachine::clearDisplay(); // Not sure if I need to clear every time, Long run?
    for (unsigned int i = 0; i < 8; i++)
    {
        if (i < msg.length())
        {
            PinballMachine::writeDisplay(i, msg.charAt(i));
        }
    }
    PinballMachine::updateDisplay(); // Just like clear, should we auto update in long run?
}

void PinballMachine::writeDisplay(int place, char in)
{
    if (isprint(in))
    {
        if (place > 3)
        {
            display2.writeDigitAscii(place - 4, in);
        }
        else
        {
            display1.writeDigitAscii(place, in);
        }
    }
}

void PinballMachine::updateDisplay()
{
    display1.writeDisplay();
    display2.writeDisplay();
}

void PinballMachine::clearDisplay()
{
    display1.clear();
    display2.clear();
    PinballMachine::updateDisplay();
}

void PinballMachine::scrollText(String msg)
{
    int len = msg.length();
    if ((msgCount % MSG_DELAY) == 0)
    {
        PinballMachine::clearDisplay();
        PinballMachine::writeDisplay(0, msg.charAt((msgPos) % len));
        PinballMachine::writeDisplay(1, msg.charAt((msgPos + 1) % len));
        PinballMachine::writeDisplay(2, msg.charAt((msgPos + 2) % len));
        PinballMachine::writeDisplay(3, msg.charAt((msgPos + 3) % len));
        PinballMachine::writeDisplay(4, msg.charAt((msgPos + 4) % len));
        PinballMachine::writeDisplay(5, msg.charAt((msgPos + 5) % len));
        PinballMachine::writeDisplay(6, msg.charAt((msgPos + 6) % len));
        PinballMachine::writeDisplay(7, msg.charAt((msgPos + 7) % len));
        PinballMachine::updateDisplay();

        msgPos++;
        if (msgPos == len)
        {
            msgPos = 0;
        }
    }

    // Should we ever reset the msgCount?
    // TODO: Reset the count in the if statement above??
    msgCount++;
}

// If we switch to a different message, we need to reset the position
// Would be handy to do this Automatically
void PinballMachine::resetScrollTextPosition()
{
    msgPos = 0;
    msgCount = 0;
}