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
}

void PinballMachine::loop()
{
}