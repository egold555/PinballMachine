// Serial protocol between the ESP (game logic) and the Mega (I/O board).
//
// Every message is one frame:
//
//     COBS( [type] [payload ...] [crc8] )  0x00
//
// COBS encoding guarantees the only 0x00 byte on the wire is the frame end,
// so a receiver that joins mid-stream or sees noise resyncs at the next 0x00.
// The CRC drops corrupted frames. See protocol/README.md for the full list.
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "MachineMap.h"

namespace pb
{

    const uint8_t PROTOCOL_VERSION = 1;
    const uint32_t BAUD_RATE = 115200;

    // The Mega goes safe (coils off, autofire off) if it hears nothing from
    // the ESP for this long. The ESP sends something at least every
    // HEARTBEAT_MS, so a few lost frames are harmless.
    const uint16_t LINK_TIMEOUT_MS = 1000;
    const uint16_t HEARTBEAT_MS = 100;

    const uint8_t MAX_PAYLOAD = 24;
    const uint8_t MAX_AUTOFIRE_RULES = 8;

    enum MsgType : uint8_t
    {
        // ---- Both directions
        MSG_HELLO = 0x01, // [protocol version]  Sent on boot. The Mega answers an ESP hello with its own.

        // ---- Mega -> ESP
        MSG_SWITCH = 0x02,       // [switch id] [1 = closed, 0 = open]
        MSG_SWITCH_STATE = 0x03, // [SWITCH_BYTES bitmap]  Every switch, sent after hello and twice a second.
        MSG_LOG = 0x04,          // [ascii text]  Debug text from the Mega.

        // ---- ESP -> Mega
        MSG_PING = 0x10,              // []  Keeps the link alive when there is nothing else to say.
        MSG_LAMPS = 0x11,             // [LAMP_BYTES bitmap]  The state of every lamp.
        MSG_COIL_PULSE = 0x12,        // [coil id] [ms]  Fire a coil once. The Mega clamps ms to a safe maximum.
        MSG_AUTOFIRE_RULE = 0x13,     // [rule index] [switch id] [coil id] [ms]  When the switch closes, pulse the coil.
        MSG_AUTOFIRE_ENABLE = 0x14,   // [bitmask of rule indexes]  0 disables all (tilt, game over).
        MSG_DISPLAY_TEXT = 0x15,      // [8 ascii chars] [decimal point bitmask]
        MSG_DISPLAY_SEGMENTS = 0x16,  // [8 x uint16 little endian]  Raw segment bits, for animations.
        MSG_NOTE_ON = 0x17,           // [channel] [midi note]
        MSG_NOTE_OFF = 0x18,          // [channel]  0xFF = all channels
    };

    // ------------------------------------------------------------- Bitmaps

    inline bool bitGet(const uint8_t *bits, uint8_t i) { return bits[i >> 3] & (1 << (i & 7)); }

    inline void bitPut(uint8_t *bits, uint8_t i, bool on)
    {
        if (on)
            bits[i >> 3] |= (1 << (i & 7));
        else
            bits[i >> 3] &= ~(1 << (i & 7));
    }

    // -------------------------------------------------------------- Framing

    inline uint8_t crc8(const uint8_t *data, size_t len)
    {
        uint8_t crc = 0;
        while (len--)
        {
            crc ^= *data++;
            for (uint8_t i = 0; i < 8; i++)
                crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x07) : (uint8_t)(crc << 1);
        }
        return crc;
    }

    const size_t MAX_RAW = MAX_PAYLOAD + 2;   // type + payload + crc
    const size_t MAX_FRAME = MAX_RAW + 2;     // + COBS overhead + 0x00 terminator

    // Builds a complete frame into out (at least MAX_FRAME bytes).
    // Returns the number of bytes to send, or 0 if the payload is too big.
    inline size_t encodeFrame(uint8_t type, const uint8_t *payload, uint8_t len, uint8_t *out)
    {
        if (len > MAX_PAYLOAD)
            return 0;

        uint8_t raw[MAX_RAW];
        raw[0] = type;
        if (len)
            memcpy(raw + 1, payload, len);
        raw[len + 1] = crc8(raw, len + 1);
        size_t rawLen = len + 2;

        // COBS: each 0x00 is replaced by the distance to the next one.
        size_t codeAt = 0, o = 1;
        uint8_t code = 1;
        for (size_t i = 0; i < rawLen; i++)
        {
            if (raw[i] == 0)
            {
                out[codeAt] = code;
                codeAt = o++;
                code = 1;
            }
            else
            {
                out[o++] = raw[i];
                code++;
            }
        }
        out[codeAt] = code;
        out[o++] = 0;
        return o;
    }

    // Feed it received bytes one at a time. When feed() returns true, a valid
    // frame is ready in type() / payload() / length() until the next feed().
    class FrameDecoder
    {
    public:
        bool feed(uint8_t b)
        {
            if (b == 0)
            {
                bool ok = !overflow_ && decode();
                n_ = 0;
                overflow_ = false;
                return ok;
            }
            if (n_ >= sizeof(buf_))
                overflow_ = true;
            else
                buf_[n_++] = b;
            return false;
        }

        uint8_t type() const { return raw_[0]; }
        const uint8_t *payload() const { return raw_ + 1; }
        uint8_t length() const { return len_; }

    private:
        bool decode()
        {
            size_t i = 0, o = 0;
            while (i < n_)
            {
                uint8_t code = buf_[i++];
                if (code == 0)
                    return false;
                for (uint8_t k = 1; k < code; k++)
                {
                    if (i >= n_)
                        return false;
                    raw_[o++] = buf_[i++];
                }
                if (i < n_)
                    raw_[o++] = 0;
            }
            if (o < 2 || crc8(raw_, o - 1) != raw_[o - 1])
                return false;
            len_ = o - 2;
            return true;
        }

        uint8_t buf_[MAX_FRAME];
        uint8_t raw_[MAX_FRAME];
        size_t n_ = 0;
        uint8_t len_ = 0;
        bool overflow_ = false;
    };

} // namespace pb
