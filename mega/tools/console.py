#!/usr/bin/env python3
"""Test the Mega I/O board from a laptop over USB, no ESP needed.

Pretends to be the ESP: keeps the link alive, prints every switch event,
and lets you drive lamps, coils, the display and sound by hand.

    pip install pyserial        (or use PlatformIO's: ~/.platformio/penv/bin/python)
    python3 mega/tools/console.py /dev/ttyACM0

Opening the port resets the Mega, so give it two seconds. Type "help".
Keep the ESP unplugged while using this, or both will be talking at once.
"""

import sys
import threading
import time

import serial

# Must match protocol/Protocol.h and protocol/MachineMap.h
HELLO, SWITCH, SWITCH_STATE, LOG = 0x01, 0x02, 0x03, 0x04
PING, LAMPS, COIL_PULSE, AUTOFIRE_RULE, AUTOFIRE_ENABLE = 0x10, 0x11, 0x12, 0x13, 0x14
DISPLAY_TEXT, NOTE_ON, NOTE_OFF = 0x15, 0x17, 0x18
LAMP_COUNT = 36

SWITCH_NAMES = {
    0: "BALL_RETURN", 1: "TILT", 2: "RIGHT_SPINNER", 3: "RIGHT_EXTRA_BALL_LANE",
    4: "A", 5: "START", 6: "LEFT_TARGET", 7: "LEFT_SLINGSHOT",
    8: "B", 9: "LEFT_THUMPER_BUMPER", 10: "LEFT_BUMPER", 11: "LEFT_EXTRA_BALL_LANE",
    12: "C", 13: "RIGHT_THUMPER_BUMPER", 14: "LEFT_SPINNER", 15: "LEFT_ADVANCE_LANE",
    16: "D", 18: "RIGHT_TARGET", 19: "RIGHT_SLINGSHOT",
    20: "CENTER_TARGET", 22: "RIGHT_BUMPER", 23: "RIGHT_ADVANCE_LANE",
    24: "FLIPPER_LEFT", 25: "FLIPPER_RIGHT",
}
COILS = ["SLINGSHOT_LEFT", "BALL_RETURN", "SLINGSHOT_RIGHT", "THUMPER_LEFT", "THUMPER_RIGHT"]


def crc8(data):
    crc = 0
    for b in data:
        crc ^= b
        for _ in range(8):
            crc = ((crc << 1) ^ 0x07) & 0xFF if crc & 0x80 else (crc << 1) & 0xFF
    return crc


def encode(msg_type, payload=b""):
    raw = bytes([msg_type]) + bytes(payload)
    raw += bytes([crc8(raw)])
    out, block = bytearray(), bytearray()
    for b in raw:
        if b == 0:
            out += bytes([len(block) + 1]) + block
            block = bytearray()
        else:
            block.append(b)
    out += bytes([len(block) + 1]) + block
    return bytes(out) + b"\x00"


def decode(frame):
    raw, i = bytearray(), 0
    while i < len(frame):
        code = frame[i]
        if code == 0 or i + code > len(frame):
            return None
        raw += frame[i + 1:i + code]
        i += code
        if i < len(frame):
            raw.append(0)
    if len(raw) < 2 or crc8(raw[:-1]) != raw[-1]:
        return None
    return raw[0], bytes(raw[1:-1])


class Mega:
    def __init__(self, port):
        self.port = serial.Serial(port, 115200, timeout=0.05)
        self.lock = threading.Lock()
        self.lamps = bytearray(5)
        threading.Thread(target=self.reader, daemon=True).start()
        threading.Thread(target=self.heartbeat, daemon=True).start()

    def send(self, msg_type, payload=b""):
        with self.lock:
            self.port.write(encode(msg_type, payload))

    def heartbeat(self):
        while True:
            self.send(LAMPS, self.lamps)
            time.sleep(0.1)

    def reader(self):
        buf = bytearray()
        while True:
            for b in self.port.read(64):
                if b != 0:
                    buf.append(b)
                    continue
                msg = decode(bytes(buf))
                buf.clear()
                if msg:
                    self.show(*msg)

    def show(self, msg_type, p):
        if msg_type == SWITCH and len(p) == 2:
            name = SWITCH_NAMES.get(p[0], "?")
            print(f"\n  switch {p[0]:2} {name:22} {'CLOSED' if p[1] else 'open'}")
        elif msg_type == HELLO:
            print(f"\n  Mega says hello (protocol v{p[0] if p else '?'})")
        elif msg_type == LOG:
            print(f"\n  mega: {p.decode(errors='replace')}")

    def set_lamp(self, i, on):
        if on:
            self.lamps[i // 8] |= 1 << (i % 8)
        else:
            self.lamps[i // 8] &= ~(1 << (i % 8))


HELP = """
  lamp <0-35|all> on|off     set a lamp (IDs in protocol/MachineMap.h)
  chase                      light each lamp in turn, to map them out
  pulse <coil> [ms]          fire a coil, default 30 ms (coils: 0-4, or a name)
  autofire on|off            slingshots and thumpers fire on their own switches
  text <message>             show up to 8 characters on the display
  note <channel 0-2> <midi>  play a note (60 = middle C);  "note off" stops all
  quit
"""


def main():
    if len(sys.argv) != 2:
        sys.exit(f"usage: {sys.argv[0]} /dev/ttyACM0")

    mega = Mega(sys.argv[1])
    print("Waiting for the Mega to reset...")
    time.sleep(2)
    mega.send(HELLO, b"\x01")
    print('Ready. Type "help".')

    while True:
        try:
            parts = input("> ").split()
        except (EOFError, KeyboardInterrupt):
            break
        if not parts:
            continue
        cmd, args = parts[0].lower(), parts[1:]
        try:
            if cmd == "help":
                print(HELP)
            elif cmd == "quit":
                break
            elif cmd == "lamp":
                ids = range(LAMP_COUNT) if args[0] == "all" else [int(args[0])]
                for i in ids:
                    mega.set_lamp(i, args[1] == "on")
            elif cmd == "chase":
                for i in range(LAMP_COUNT):
                    mega.lamps[:] = bytes(5)
                    mega.set_lamp(i, True)
                    print(f"  lamp {i}")
                    time.sleep(0.6)
                mega.lamps[:] = bytes(5)
            elif cmd == "pulse":
                coil = COILS.index(args[0].upper()) if not args[0].isdigit() else int(args[0])
                ms = int(args[1]) if len(args) > 1 else 30
                mega.send(COIL_PULSE, bytes([coil, ms]))
            elif cmd == "autofire":
                rules = [(7, 0), (19, 2), (9, 3), (13, 4)]  # (switch, coil), same as the ESP
                for i, (sw, coil) in enumerate(rules):
                    mega.send(AUTOFIRE_RULE, bytes([i, sw, coil, 50]))
                mega.send(AUTOFIRE_ENABLE, bytes([0x0F if args[0] == "on" else 0]))
            elif cmd == "text":
                text = " ".join(args)[:8].ljust(8)
                mega.send(DISPLAY_TEXT, text.encode("ascii", "replace") + b"\x00")
            elif cmd == "note":
                if args[0] == "off":
                    mega.send(NOTE_OFF, b"\xff")
                else:
                    mega.send(NOTE_ON, bytes([int(args[0]), int(args[1])]))
            else:
                print('  unknown command, type "help"')
        except (IndexError, ValueError):
            print('  bad arguments, type "help"')


if __name__ == "__main__":
    main()
