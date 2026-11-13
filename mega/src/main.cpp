// Pinball machine I/O board.
//
// The Mega does no game logic. It scans the lamp/switch matrix, drives the
// coils (with safety limits and fast autofire for slingshots and bumpers),
// writes the displays and plays notes. All decisions come from the ESP over
// Serial2. The same protocol is also accepted on the USB port, so the machine
// can be tested from a laptop (see mega/tools/console.py).
//
// If the ESP goes quiet for LINK_TIMEOUT_MS the Mega goes safe: coils off,
// autofire off, lamps off, sound off, and "NO ESP" on the display.

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <Playtune.h>
#include <Protocol.h>

#include "Pins.h"
#include "Matrix.h"
#include "Coils.h"

// Playtune's note functions are global but not in its header.
void tune_playnote(byte chan, byte note);
void tune_stopnote(byte chan);

static Matrix matrix;
static Coils coils;
static Playtune playtune;
static Adafruit_AlphaNum4 displayLeft;
static Adafruit_AlphaNum4 displayRight;

static pb::FrameDecoder usbDecoder;
static pb::FrameDecoder espDecoder;

static bool linkUp = false;
static uint32_t lastRxTime = 0;
static uint32_t lastSwitchStateTime = 0;

static bool displayLeftDirty = false;
static bool displayRightDirty = false;

// ------------------------------------------------------------------ Sending

static void send(uint8_t type, const uint8_t *payload, uint8_t len)
{
    uint8_t frame[pb::MAX_FRAME];
    size_t n = pb::encodeFrame(type, payload, len, frame);
    ESP_SERIAL.write(frame, n);
    Serial.write(frame, n);
}

static void sendHello()
{
    uint8_t version = pb::PROTOCOL_VERSION;
    send(pb::MSG_HELLO, &version, 1);
}

static void sendSwitchState()
{
    send(pb::MSG_SWITCH_STATE, matrix.switchBits(), pb::SWITCH_BYTES);
    lastSwitchStateTime = millis();
}

// ------------------------------------------------------------------ Outputs

static void displayText(const char *text, uint8_t dots)
{
    for (uint8_t i = 0; i < pb::DISPLAY_CHARS; i++)
    {
        char c = isprint(text[i]) ? text[i] : ' ';
        bool dot = dots & (1 << i);
        if (i < 4)
            displayLeft.writeDigitAscii(i, c, dot);
        else
            displayRight.writeDigitAscii(i - 4, c, dot);
    }
    displayLeftDirty = displayRightDirty = true;
}

static void displaySegments(const uint8_t *payload)
{
    for (uint8_t i = 0; i < pb::DISPLAY_CHARS; i++)
    {
        uint16_t bits = payload[i * 2] | (payload[i * 2 + 1] << 8);
        if (i < 4)
            displayLeft.writeDigitRaw(i, bits);
        else
            displayRight.writeDigitRaw(i - 4, bits);
    }
    displayLeftDirty = displayRightDirty = true;
}

static void notesOff()
{
    for (uint8_t c = 0; c < pb::SOUND_CHANNELS; c++)
        tune_stopnote(c);
}

static void goSafe()
{
    coils.allOff();
    matrix.clearLamps();
    notesOff();
    displayText("NO ESP  ", 0);
}

// Called by the matrix once per frame while every lamp is off.
// Writes at most one display per frame to keep the gap short.
static void onFrameGap()
{
    if (displayLeftDirty)
    {
        displayLeft.writeDisplay();
        displayLeftDirty = false;
    }
    else if (displayRightDirty)
    {
        displayRight.writeDisplay();
        displayRightDirty = false;
    }
}

// ----------------------------------------------------------------- Switches

static void onSwitch(uint8_t id, bool closed)
{
    if (closed)
        coils.onSwitchClosed(id); // autofire first, it is time critical

    uint8_t payload[2] = {id, closed};
    send(pb::MSG_SWITCH, payload, 2);
}

// ---------------------------------------------------------------- Receiving

static void handleFrame(const pb::FrameDecoder &f)
{
    const uint8_t *p = f.payload();
    uint8_t len = f.length();

    lastRxTime = millis();
    if (!linkUp)
    {
        linkUp = true;
        sendHello();
        sendSwitchState();
    }

    switch (f.type())
    {
    case pb::MSG_HELLO:
        sendHello();
        sendSwitchState();
        break;
    case pb::MSG_PING:
        break;
    case pb::MSG_LAMPS:
        if (len == pb::LAMP_BYTES)
            matrix.setLamps(p);
        break;
    case pb::MSG_COIL_PULSE:
        if (len == 2)
            coils.pulse(p[0], p[1]);
        break;
    case pb::MSG_AUTOFIRE_RULE:
        if (len == 4)
            coils.setRule(p[0], p[1], p[2], p[3]);
        break;
    case pb::MSG_AUTOFIRE_ENABLE:
        if (len == 1)
            coils.setRulesEnabled(p[0]);
        break;
    case pb::MSG_DISPLAY_TEXT:
        if (len == pb::DISPLAY_CHARS + 1)
            displayText((const char *)p, p[pb::DISPLAY_CHARS]);
        break;
    case pb::MSG_DISPLAY_SEGMENTS:
        if (len == pb::DISPLAY_CHARS * 2)
            displaySegments(p);
        break;
    case pb::MSG_NOTE_ON:
        if (len == 2 && p[0] < pb::SOUND_CHANNELS && p[1] < 128)
            tune_playnote(p[0], p[1]);
        break;
    case pb::MSG_NOTE_OFF:
        if (len == 1 && p[0] == 0xFF)
            notesOff();
        else if (len == 1 && p[0] < pb::SOUND_CHANNELS)
            tune_stopnote(p[0]);
        break;
    }
}

static void readPort(Stream &port, pb::FrameDecoder &decoder)
{
    while (port.available())
    {
        if (decoder.feed(port.read()))
            handleFrame(decoder);
    }
}

// --------------------------------------------------------------------- Main

void setup()
{
    Serial.begin(pb::BAUD_RATE);
    ESP_SERIAL.begin(pb::BAUD_RATE);

    coils.begin();

    for (uint8_t c = 0; c < pb::SOUND_CHANNELS; c++)
        playtune.tune_initchan(SOUND_PINS[c]);

    displayLeft.begin(DISPLAY_LEFT_ADDR);
    displayRight.begin(DISPLAY_RIGHT_ADDR);
    Wire.setClock(400000); // faster writes, less lamp flicker (worked on this wiring in 2018)

    matrix.begin(onSwitch, onFrameGap);

    goSafe();
    sendHello();
}

void loop()
{
    readPort(ESP_SERIAL, espDecoder);
    readPort(Serial, usbDecoder);

    if (linkUp && millis() - lastRxTime > pb::LINK_TIMEOUT_MS)
    {
        linkUp = false;
        goSafe();
    }

    matrix.update();
    coils.update();

    if (linkUp && millis() - lastSwitchStateTime > 500)
        sendSwitchState();
}
