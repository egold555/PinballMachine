// Lamp and switch matrix scanner.
//
// Each column is driven HIGH with its four lamp rows set, left on for
// DWELL_US, then its four switch rows are read and everything is turned off
// before the next column. This is the same sequence and timing as the
// original code, but it never blocks: update() returns right away while a
// column is dwelling, so the rest of the loop keeps running.
#pragma once

#include <Arduino.h>
#include <MachineMap.h>

class Matrix
{
public:
    typedef void (*SwitchHandler)(uint8_t id, bool closed);
    typedef void (*FrameHandler)();

    void begin(SwitchHandler onSwitch, FrameHandler onFrameGap);
    void update();

    void setLamps(const uint8_t *bits); // LAMP_BYTES bitmap
    void clearLamps();

    bool isClosed(uint8_t id) const;
    const uint8_t *switchBits() const { return switches_; }

private:
    void startColumn();
    void finishColumn();
    void setSwitch(uint8_t id, bool closed);

    static const uint16_t DWELL_US = 800;

    SwitchHandler onSwitch_ = nullptr;
    FrameHandler onFrameGap_ = nullptr;
    uint8_t lamps_[pb::LAMP_BYTES] = {0};
    uint8_t switches_[pb::SWITCH_BYTES] = {0};
    uint8_t column_ = 0;
    uint32_t columnStart_ = 0;
};
