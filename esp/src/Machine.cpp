#include "Machine.h"

namespace
{
    struct RuleDef
    {
        SwitchId sw;
        CoilId coil;
        uint8_t ms;
    };

    // Index i here is AutofireRule bit i. 50 ms is the original SOLENOID_DELAY.
    const RuleDef AUTOFIRE_RULES[] = {
        {SW_LEFT_SLINGSHOT, COIL_SLINGSHOT_LEFT, 50},
        {SW_RIGHT_SLINGSHOT, COIL_SLINGSHOT_RIGHT, 50},
        {SW_LEFT_THUMPER_BUMPER, COIL_THUMPER_LEFT, 50},
        {SW_RIGHT_THUMPER_BUMPER, COIL_THUMPER_RIGHT, 50},
    };

    const uint32_t SERVICE_PERIOD_MS = 10;
}

void Machine::begin(MegaLink &link, SoundPlayer &sound)
{
    link_ = &link;
    sound_ = &sound;
    events_ = xQueueCreate(64, sizeof(SwitchEvent));
    xTaskCreate(serviceTask, "machine", 4096, this, 3, nullptr);
}

// ------------------------------------------------------------------ Switches

bool Machine::nextEvent(SwitchEvent &ev, uint32_t timeoutMs)
{
    return xQueueReceive(events_, &ev, pdMS_TO_TICKS(timeoutMs)) == pdTRUE;
}

bool Machine::isClosed(SwitchId id)
{
    std::lock_guard<std::mutex> lock(lock_);
    return bitGet(switches_, id);
}

void Machine::sleep(uint32_t ms)
{
    sleepUntilPressed(ms, (SwitchId)0xFF);
}

bool Machine::sleepUntilPressed(uint32_t ms, SwitchId id)
{
    uint32_t end = millis() + ms;
    SwitchEvent ev;
    for (;;)
    {
        int32_t left = end - millis();
        if (left <= 0)
            return false;
        if (nextEvent(ev, left) && ev.pressed && ev.id == id)
            return true;
    }
}

void Machine::clearEvents()
{
    xQueueReset(events_);
}

void Machine::onSwitch(uint8_t id, bool closed)
{
    SwitchEvent ev = {(SwitchId)id, closed, false, millis()};
    {
        std::lock_guard<std::mutex> lock(lock_);
        bitPut(switches_, id, closed);
        if (closed)
        {
            ev.pressed = ev.time - lastClose_[id] > DEBOUNCE_MS;
            lastClose_[id] = ev.time;
        }
    }
    Serial.printf("[switch] %2u %s\n", id, closed ? "closed" : "open");
    xQueueSend(events_, &ev, 0);
}

void Machine::onSwitchState(const uint8_t *bits)
{
    std::lock_guard<std::mutex> lock(lock_);
    memcpy(switches_, bits, SWITCH_BYTES);
}

void Machine::onHello(uint8_t)
{
    Serial.println("[link] Mega connected");
    resyncNeeded_ = true;
}

// --------------------------------------------------------------------- Lamps

void Machine::setLamp(LampId id, bool on)
{
    if (lockedOut_)
        return;
    std::lock_guard<std::mutex> lock(lock_);
    blinks_[id].remaining = 0;
    if (bitGet(lamps_, id) != on)
    {
        bitPut(lamps_, id, on);
        lampsDirty_ = true;
    }
}

bool Machine::lamp(LampId id)
{
    std::lock_guard<std::mutex> lock(lock_);
    return bitGet(lamps_, id);
}

void Machine::blinkLamp(LampId id, uint16_t intervalMs, uint16_t toggles)
{
    if (lockedOut_)
        return;
    std::lock_guard<std::mutex> lock(lock_);
    blinks_[id] = {intervalMs, toggles, millis() + intervalMs};
}

// ------------------------------------------------------------------- Display

void Machine::setText(const char *text)
{
    size_t i = 0;
    for (; i < DISPLAY_CHARS && text[i]; i++)
        text_[i] = text[i];
    for (; i < DISPLAY_CHARS; i++)
        text_[i] = ' ';
    textDirty_ = true;
}

void Machine::showText(const char *text)
{
    if (lockedOut_)
        return;
    std::lock_guard<std::mutex> lock(lock_);
    scrolling_ = false;
    setText(text);
}

void Machine::showNumber(long n)
{
    char buf[DISPLAY_CHARS + 1];
    snprintf(buf, sizeof(buf), "%8ld", n);
    showText(buf);
}

void Machine::scrollText(const String &msg, uint16_t stepMs)
{
    if (lockedOut_ || msg.length() == 0)
        return;
    std::lock_guard<std::mutex> lock(lock_);
    scrollStepMs_ = stepMs;
    if (scrolling_ && msg == scroll_)
        return;
    scroll_ = msg;
    scrollPos_ = 0;
    scrollNext_ = millis();
    scrolling_ = true;
}

// --------------------------------------------------------------------- Sound

void Machine::playSound(const uint8_t *score, SoundPriority priority)
{
    if (!lockedOut_)
        sound_->play(score, priority);
}

void Machine::stopSound() { sound_->stop(); }

bool Machine::soundPlaying() { return sound_->playing(); }

void Machine::waitForSound()
{
    while (sound_->playing())
        sleep(10);
}

// --------------------------------------------------------------------- Coils

void Machine::pulseCoil(CoilId id, uint8_t ms)
{
    if (lockedOut_)
        return;
    uint8_t payload[2] = {id, ms};
    link_->send(MSG_COIL_PULSE, payload, 2);
}

void Machine::setAutofire(uint8_t rules)
{
    if (lockedOut_)
        return;
    autofire_ = rules;
    link_->send(MSG_AUTOFIRE_ENABLE, &autofire_, 1);
}

// -------------------------------------------------------------------- Safety

void Machine::lockOut(const char *message)
{
    lockedOut_ = true;
    autofire_ = 0;
    link_->send(MSG_AUTOFIRE_ENABLE, &autofire_, 1);
    sound_->stop();
    std::lock_guard<std::mutex> lock(lock_);
    memset(lamps_, 0, sizeof(lamps_));
    for (Blink &b : blinks_)
        b.remaining = 0;
    lampsDirty_ = true;
    scrolling_ = false;
    setText(message);
}

// ---------------------------------------------------------- Background work

void Machine::serviceTask(void *self)
{
    TickType_t wake = xTaskGetTickCount();
    for (;;)
    {
        static_cast<Machine *>(self)->service();
        vTaskDelayUntil(&wake, pdMS_TO_TICKS(SERVICE_PERIOD_MS));
    }
}

void Machine::service()
{
    if (resyncNeeded_)
    {
        resyncNeeded_ = false;
        resync();
    }

    std::lock_guard<std::mutex> lock(lock_);
    uint32_t now = millis();

    for (uint8_t i = 0; i < LAMP_COUNT; i++)
    {
        Blink &b = blinks_[i];
        if (b.remaining && (int32_t)(now - b.next) >= 0)
        {
            bitPut(lamps_, i, !bitGet(lamps_, i));
            lampsDirty_ = true;
            b.remaining--;
            b.next += b.interval;
        }
    }

    if (scrolling_ && (int32_t)(now - scrollNext_) >= 0)
    {
        size_t len = scroll_.length();
        for (uint8_t i = 0; i < DISPLAY_CHARS; i++)
            text_[i] = scroll_[(scrollPos_ + i) % len];
        textDirty_ = true;
        scrollPos_ = (scrollPos_ + 1) % len;
        scrollNext_ += scrollStepMs_;
    }

    if (textDirty_)
        sendDisplay();

    // The lamp frame doubles as the heartbeat that keeps the Mega out of safe mode.
    if (lampsDirty_ || now - lastLampSend_ >= HEARTBEAT_MS)
        sendLamps();
}

void Machine::sendDisplay()
{
    uint8_t payload[DISPLAY_CHARS + 1];
    memcpy(payload, text_, DISPLAY_CHARS);
    payload[DISPLAY_CHARS] = 0; // no decimal points
    link_->send(MSG_DISPLAY_TEXT, payload, sizeof(payload));
    textDirty_ = false;
}

void Machine::sendLamps()
{
    link_->send(MSG_LAMPS, lamps_, LAMP_BYTES);
    lampsDirty_ = false;
    lastLampSend_ = millis();
}

// The Mega just (re)connected and forgot everything: tell it again.
void Machine::resync()
{
    for (uint8_t i = 0; i < sizeof(AUTOFIRE_RULES) / sizeof(AUTOFIRE_RULES[0]); i++)
    {
        const RuleDef &r = AUTOFIRE_RULES[i];
        uint8_t payload[4] = {i, r.sw, r.coil, r.ms};
        link_->send(MSG_AUTOFIRE_RULE, payload, 4);
    }
    link_->send(MSG_AUTOFIRE_ENABLE, &autofire_, 1);

    std::lock_guard<std::mutex> lock(lock_);
    lampsDirty_ = true;
    textDirty_ = true;
}
