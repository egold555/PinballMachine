#include "MegaLink.h"

void MegaLink::begin(HardwareSerial &port, int rxPin, int txPin, Listener *listener)
{
    port_ = &port;
    listener_ = listener;
    lastRx_ = millis() - pb::LINK_TIMEOUT_MS;
    port_->begin(pb::BAUD_RATE, SERIAL_8N1, rxPin, txPin);
    xTaskCreate(rxTask, "megaRx", 4096, this, 5, nullptr);

    uint8_t version = pb::PROTOCOL_VERSION;
    send(pb::MSG_HELLO, &version, 1);
}

void MegaLink::send(uint8_t type, const uint8_t *payload, uint8_t len)
{
    if (!port_)
        return; // not started yet; the Mega gets everything again on hello
    uint8_t frame[pb::MAX_FRAME];
    size_t n = pb::encodeFrame(type, payload, len, frame);
    std::lock_guard<std::mutex> lock(txLock_);
    port_->write(frame, n);
}

void MegaLink::rxTask(void *self)
{
    MegaLink *link = static_cast<MegaLink *>(self);
    for (;;)
    {
        while (link->port_->available())
        {
            if (link->decoder_.feed(link->port_->read()))
                link->handleFrame();
        }
        vTaskDelay(1);
    }
}

void MegaLink::handleFrame()
{
    lastRx_ = millis();
    const uint8_t *p = decoder_.payload();
    uint8_t len = decoder_.length();

    switch (decoder_.type())
    {
    case pb::MSG_HELLO:
        if (len == 1)
        {
            if (p[0] != pb::PROTOCOL_VERSION)
                Serial.printf("[link] WARNING: Mega speaks protocol v%u, ESP speaks v%u. Reflash both.\n", p[0], pb::PROTOCOL_VERSION);
            listener_->onHello(p[0]);
        }
        break;
    case pb::MSG_SWITCH:
        if (len == 2 && p[0] < pb::SWITCH_COUNT)
            listener_->onSwitch(p[0], p[1]);
        break;
    case pb::MSG_SWITCH_STATE:
        if (len == pb::SWITCH_BYTES)
            listener_->onSwitchState(p);
        break;
    case pb::MSG_LOG:
        Serial.printf("[mega] %.*s\n", len, (const char *)p);
        break;
    }
}
