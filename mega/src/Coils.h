// Coil driver with hard safety limits.
//
// Whatever the ESP asks for, a coil is never on longer than MAX_PULSE_MS and
// always rests MIN_REST_MS between pulses, so a bug, a crash or a stuck
// switch cannot cook a coil.
//
// Autofire rules ("when this switch closes, pulse that coil") run here on the
// Mega so slingshots and bumpers react instantly. The ESP only decides which
// rules exist and when they are enabled.
#pragma once

#include <Arduino.h>
#include <Protocol.h>

class Coils
{
public:
    void begin();
    void update();

    void pulse(uint8_t coil, uint8_t ms);
    void allOff();

    void setRule(uint8_t index, uint8_t switchId, uint8_t coil, uint8_t ms);
    void setRulesEnabled(uint8_t mask) { enabledRules_ = mask; }
    void onSwitchClosed(uint8_t switchId);

private:
    static const uint8_t MAX_PULSE_MS = 150;
    static const uint8_t MIN_REST_MS = 25;

    struct Coil
    {
        bool on;
        uint32_t offAt;
        uint32_t restUntil;
    };

    struct Rule
    {
        bool used;
        uint8_t switchId;
        uint8_t coil;
        uint8_t ms;
    };

    Coil coils_[pb::COIL_COUNT];
    Rule rules_[pb::MAX_AUTOFIRE_RULES];
    uint8_t enabledRules_ = 0;
};
