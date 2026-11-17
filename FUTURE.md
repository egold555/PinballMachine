# Future ideas

Things that are possible but not built yet.

## Flash the Mega over WiFi through the ESP

Only needed with the ESP brain. With the Pi brain ([`pi/`](pi/)) the Pi is already on the Mega's USB port and can flash it directly.

Right now the Mega can only be flashed over its own USB port. It can also be done over the network, with the ESP passing the upload through.

### Why the current wiring isn't enough

The Mega's bootloader only listens on **Serial0** (pins 0/1, the same lines as USB), not Serial2. It also only runs for about a second after a reset, so the ESP needs a way to reset the Mega.

### Extra wiring (new jumpers only, nothing existing changes)

| XIAO ESP32-C6 | | Mega |
|---|---|---|
| free pin, e.g. D0 (TX) | → | pin 0 (RX0) |
| free pin, e.g. D1 (RX) | ← voltage divider ← | pin 1 (TX0) |
| free pin, e.g. D2 | ← 1N4148 diode ← | RESET (power header) |

- Use the same 1 kΩ / 2 kΩ divider as on the Serial2 link, because Mega TX is 5 V.
- The diode's stripe (cathode) goes toward the ESP. When the ESP pulls D2 low, the Mega resets. When D2 is high, the diode blocks the Mega's 5 V from reaching the ESP.
- The Mega already has 1 kΩ resistors between its USB chip and pins 0/1, so the ESP can drive pin 0 without a conflict.

### Software

- **ESP:** open a TCP port (e.g. `pinball.local:2323`). When a client connects:
  1. pause the game and show `UPDATING`
  2. pulse RESET
  3. pass bytes both ways between TCP and Serial0 at the bootloader's baud rate (115200)
  4. go back to normal when the client disconnects
- **Mega:** add an `ota` env to `mega/platformio.ini` that runs avrdude with `-c wiring -P net:pinball.local:2323`. Then `pio run -e ota -t upload` works for both boards.

### Risk

Low. A WiFi upload never touches the bootloader, so a failed upload is fixed by plugging in USB and flashing normally. The fiddly part is reset timing, which has to land inside the bootloader's short window. If it misses, the upload fails and can be retried.
