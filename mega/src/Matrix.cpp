#include "Matrix.h"
#include "Pins.h"
#include <Protocol.h>

void Matrix::begin(SwitchHandler onSwitch, FrameHandler onFrameGap)
{
    onSwitch_ = onSwitch;
    onFrameGap_ = onFrameGap;

    for (uint8_t c = 0; c < pb::LAMP_COLUMNS; c++)
    {
        pinMode(COLUMN_PINS[c], OUTPUT);
        digitalWrite(COLUMN_PINS[c], LOW);
    }
    for (uint8_t r = 0; r < 4; r++)
    {
        pinMode(LAMP_ROW_PINS[r], OUTPUT);
        digitalWrite(LAMP_ROW_PINS[r], LOW);
        pinMode(SWITCH_ROW_PINS[r], INPUT);
    }
    pinMode(FLIPPER_LEFT_PIN, INPUT_PULLUP);
    pinMode(FLIPPER_RIGHT_PIN, INPUT_PULLUP);

    column_ = 0;
    startColumn();
}

void Matrix::update()
{
    if (micros() - columnStart_ < DWELL_US)
        return;

    finishColumn();

    column_++;
    if (column_ == pb::LAMP_COLUMNS)
    {
        column_ = 0;
        setSwitch(pb::SW_FLIPPER_LEFT, digitalRead(FLIPPER_LEFT_PIN) == LOW);
        setSwitch(pb::SW_FLIPPER_RIGHT, digitalRead(FLIPPER_RIGHT_PIN) == LOW);

        // All lamps are off right now, so slow work (I2C) here dims every
        // lamp equally instead of making one column flicker brighter.
        if (onFrameGap_)
            onFrameGap_();
    }

    startColumn();
}

void Matrix::startColumn()
{
    digitalWrite(COLUMN_PINS[column_], HIGH);
    for (uint8_t r = 0; r < 4; r++)
        digitalWrite(LAMP_ROW_PINS[r], pb::bitGet(lamps_, column_ * 4 + r) ? HIGH : LOW);
    columnStart_ = micros();
}

void Matrix::finishColumn()
{
    // Switches are read while the column is still driven, like the original.
    if (column_ < pb::SWITCH_MATRIX_COLUMNS)
    {
        for (uint8_t r = 0; r < 4; r++)
        {
            uint8_t id = column_ * 4 + r;
            if (!pb::switchIsUnused(id))
                setSwitch(id, analogRead(SWITCH_ROW_PINS[r]) > SWITCH_THRESHOLD);
        }
    }

    for (uint8_t r = 0; r < 4; r++)
        digitalWrite(LAMP_ROW_PINS[r], LOW);
    digitalWrite(COLUMN_PINS[column_], LOW);
}

void Matrix::setSwitch(uint8_t id, bool closed)
{
    if (pb::bitGet(switches_, id) == closed)
        return;
    pb::bitPut(switches_, id, closed);
    if (onSwitch_)
        onSwitch_(id, closed);
}

void Matrix::setLamps(const uint8_t *bits)
{
    memcpy(lamps_, bits, pb::LAMP_BYTES);
}

void Matrix::clearLamps()
{
    memset(lamps_, 0, pb::LAMP_BYTES);
}

bool Matrix::isClosed(uint8_t id) const
{
    return pb::bitGet(switches_, id);
}
