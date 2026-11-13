// The pinball machine as game code sees it.
//
// Game modes only talk to this class: read switch events, set lamps, write
// the display, play sounds, fire coils. It is safe to block (sleep()) in game
// code. Lamps, blinking, scrolling text, music and the link heartbeat all keep
// running in background tasks.
#pragma once

#include <Arduino.h>
#include <mutex>
#include <MachineMap.h>
#include "MegaLink.h"
#include "SoundPlayer.h"

using namespace pb;

struct SwitchEvent
{
    SwitchId id;
    bool closed;   // the switch closed (true) or opened (false)
    bool pressed;  // closed, and not a bounce: same rule as the original code
    uint32_t time; // millis() when it arrived
};

// Standard autofire rules, set up on the Mega whenever it connects.
enum AutofireRule : uint8_t
{
    AUTOFIRE_SLINGSHOT_LEFT = 1 << 0,
    AUTOFIRE_SLINGSHOT_RIGHT = 1 << 1,
    AUTOFIRE_THUMPER_LEFT = 1 << 2,
    AUTOFIRE_THUMPER_RIGHT = 1 << 3,
    AUTOFIRE_ALL = 0x0F,
};

class Machine : public MegaLink::Listener
{
public:
    void begin(MegaLink &link, SoundPlayer &sound);

    // ---- Switches
    // Waits up to timeoutMs for the next switch event. Returns false on timeout.
    bool nextEvent(SwitchEvent &ev, uint32_t timeoutMs);
    bool isClosed(SwitchId id);
    // Waits, throwing away switch events (like a delay() in the old code).
    void sleep(uint32_t ms);
    // Like sleep(), but returns true early if the switch is pressed.
    bool sleepUntilPressed(uint32_t ms, SwitchId id);
    void clearEvents();

    // ---- Lamps
    void setLamp(LampId id, bool on); // also stops it blinking
    bool lamp(LampId id);
    // Toggles the lamp every intervalMs, `toggles` times, then leaves it alone.
    void blinkLamp(LampId id, uint16_t intervalMs, uint16_t toggles);

    // ---- Display (8 characters)
    void showText(const char *text); // left aligned
    void showNumber(long n);         // right aligned
    // Scrolls msg in the background until the next showText()/showNumber().
    // Calling it again with the same message keeps scrolling where it was.
    void scrollText(const String &msg, uint16_t stepMs = 220);

    // ---- Sound
    void playSound(const uint8_t *score, SoundPriority priority = SoundPriority::Normal);
    void stopSound();
    bool soundPlaying();
    void waitForSound(); // sleep() until the current sound ends

    // ---- Coils
    void pulseCoil(CoilId id, uint8_t ms);
    void setAutofire(uint8_t rules); // AutofireRule bits, 0 = all off

    // ---- Safety
    // Turns everything off and ignores game output from now on (used for OTA updates).
    void lockOut(const char *message);

    // MegaLink::Listener (called from the link task)
    void onHello(uint8_t version) override;
    void onSwitch(uint8_t id, bool closed) override;
    void onSwitchState(const uint8_t *bits) override;

private:
    static void serviceTask(void *self);
    void service();
    void sendDisplay();
    void sendLamps();
    void resync();
    void setText(const char *text);

    static const uint8_t DEBOUNCE_MS = 20;

    struct Blink
    {
        uint16_t interval;
        uint16_t remaining;
        uint32_t next;
    };

    MegaLink *link_ = nullptr;
    SoundPlayer *sound_ = nullptr;
    QueueHandle_t events_ = nullptr;
    std::mutex lock_;

    uint8_t switches_[SWITCH_BYTES] = {0};
    uint32_t lastClose_[SWITCH_COUNT] = {0};

    uint8_t lamps_[LAMP_BYTES] = {0};
    Blink blinks_[LAMP_COUNT] = {};
    bool lampsDirty_ = true;
    uint32_t lastLampSend_ = 0;

    char text_[DISPLAY_CHARS + 1] = "        ";
    bool textDirty_ = true;
    String scroll_;
    bool scrolling_ = false;
    uint16_t scrollStepMs_ = 220;
    uint32_t scrollNext_ = 0;
    size_t scrollPos_ = 0;

    uint8_t autofire_ = 0;
    volatile bool resyncNeeded_ = false;
    volatile bool lockedOut_ = false;
};
