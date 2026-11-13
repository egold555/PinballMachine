// Serial link to the Mega I/O board.
//
// A background task reads frames from the Mega and hands them to a Listener.
// send() can be called from any task.
#pragma once

#include <Arduino.h>
#include <Protocol.h>
#include <mutex>

class MegaLink
{
public:
    class Listener
    {
    public:
        virtual void onHello(uint8_t version) = 0;
        virtual void onSwitch(uint8_t id, bool closed) = 0;
        virtual void onSwitchState(const uint8_t *bits) = 0;
    };

    void begin(HardwareSerial &port, int rxPin, int txPin, Listener *listener);

    void send(uint8_t type, const uint8_t *payload = nullptr, uint8_t len = 0);

    // True if the Mega has said anything recently.
    bool connected() const { return millis() - lastRx_ < pb::LINK_TIMEOUT_MS; }

private:
    static void rxTask(void *self);
    void handleFrame();

    HardwareSerial *port_ = nullptr;
    Listener *listener_ = nullptr;
    pb::FrameDecoder decoder_;
    std::mutex txLock_;
    volatile uint32_t lastRx_ = 0;
};
