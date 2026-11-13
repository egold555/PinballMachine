# ESP ↔ Mega protocol

Used by `mega/` and `esp/`. The definitions are in [Protocol.h](Protocol.h) (messages, framing) and [MachineMap.h](MachineMap.h) (switch, lamp and coil IDs). A copy for testing lives in [mega/tools/console.py](../mega/tools/console.py).

## Wire format

115200 baud, 8N1. Each message is one frame:

```
COBS( type, payload..., crc8 )  0x00
```

- [COBS](https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing) encoding means `0x00` only ever appears as the end of a frame, so a receiver that starts mid-stream or sees noise resyncs at the next `0x00`.
- The CRC-8 (polynomial 0x07) covers type and payload. Bad frames are dropped silently.
- Payloads are at most 24 bytes.

## Messages

| Type | Name | Direction | Payload |
|---|---|---|---|
| 0x01 | HELLO | both | protocol version. The Mega sends it on boot and whenever the link comes back; the ESP then resends all its settings. |
| 0x02 | SWITCH | Mega → ESP | switch id, 1 = closed / 0 = open |
| 0x03 | SWITCH_STATE | Mega → ESP | bitmap of all switches (4 bytes), after hello and every 500 ms |
| 0x04 | LOG | Mega → ESP | ASCII text |
| 0x10 | PING | ESP → Mega | none |
| 0x11 | LAMPS | ESP → Mega | bitmap of all 36 lamps (5 bytes) |
| 0x12 | COIL_PULSE | ESP → Mega | coil id, ms |
| 0x13 | AUTOFIRE_RULE | ESP → Mega | rule index (0-7), switch id, coil id, ms |
| 0x14 | AUTOFIRE_ENABLE | ESP → Mega | bitmask of enabled rules |
| 0x15 | DISPLAY_TEXT | ESP → Mega | 8 ASCII chars, decimal point bitmask |
| 0x16 | DISPLAY_SEGMENTS | ESP → Mega | 8 × uint16 little endian, raw segment bits |
| 0x17 | NOTE_ON | ESP → Mega | channel (0-2), MIDI note |
| 0x18 | NOTE_OFF | ESP → Mega | channel, or 0xFF for all |

Bitmaps are little endian by bit: ID `n` is bit `n % 8` of byte `n / 8`.

## Rules the Mega enforces on its own

- **Link watchdog:** if nothing valid arrives from the ESP for 1000 ms, the Mega turns off coils, autofire, lamps and sound and shows `NO ESP`. The ESP sends the lamp bitmap every 100 ms, which doubles as the heartbeat.
- **Coil limits:** a pulse is clamped to 150 ms, and a coil rests at least 25 ms between pulses. A pulse request for a coil that is on or resting is ignored.
- **Autofire:** fires on the switch *closing* only. A switch stuck closed does not keep the coil on.

## Changing the protocol

Bump `PROTOCOL_VERSION` and reflash both boards. The ESP prints a warning over USB if the versions don't match.
