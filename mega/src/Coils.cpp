#include "Coils.h"
#include "Pins.h"

void Coils::begin()
{
    for (uint8_t i = 0; i < pb::COIL_COUNT; i++)
    {
        pinMode(COIL_PINS[i], OUTPUT);
        digitalWrite(COIL_PINS[i], LOW);
        coils_[i] = {false, 0, 0};
    }
    for (uint8_t i = 0; i < pb::MAX_AUTOFIRE_RULES; i++)
        rules_[i].used = false;
    enabledRules_ = 0;
}

void Coils::update()
{
    uint32_t now = millis();
    for (uint8_t i = 0; i < pb::COIL_COUNT; i++)
    {
        Coil &c = coils_[i];
        if (c.on && (int32_t)(now - c.offAt) >= 0)
        {
            digitalWrite(COIL_PINS[i], LOW);
            c.on = false;
            c.restUntil = now + MIN_REST_MS;
        }
    }
}

void Coils::pulse(uint8_t coil, uint8_t ms)
{
    if (coil >= pb::COIL_COUNT || ms == 0)
        return;

    Coil &c = coils_[coil];
    uint32_t now = millis();
    if (c.on || (int32_t)(now - c.restUntil) < 0)
        return;

    if (ms > MAX_PULSE_MS)
        ms = MAX_PULSE_MS;
    digitalWrite(COIL_PINS[coil], HIGH);
    c.on = true;
    c.offAt = now + ms;
}

void Coils::allOff()
{
    enabledRules_ = 0;
    for (uint8_t i = 0; i < pb::COIL_COUNT; i++)
    {
        digitalWrite(COIL_PINS[i], LOW);
        coils_[i].on = false;
    }
}

void Coils::setRule(uint8_t index, uint8_t switchId, uint8_t coil, uint8_t ms)
{
    if (index >= pb::MAX_AUTOFIRE_RULES)
        return;
    rules_[index] = {true, switchId, coil, ms};
}

void Coils::onSwitchClosed(uint8_t switchId)
{
    for (uint8_t i = 0; i < pb::MAX_AUTOFIRE_RULES; i++)
    {
        const Rule &r = rules_[i];
        if (r.used && (enabledRules_ & (1 << i)) && r.switchId == switchId)
            pulse(r.coil, r.ms);
    }
}
