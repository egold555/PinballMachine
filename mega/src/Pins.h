// Mega pin assignments. Copied from the original Arduino/lib/PinballMachine/Pinouts.h,
// which matches the wiring. Do not change these without changing the wiring.
#pragma once

#include <Arduino.h>
#include <MachineMap.h>

// Coils, indexed by pb::CoilId
const uint8_t COIL_PINS[pb::COIL_COUNT] = {
    22, // COIL_SLINGSHOT_LEFT
    23, // COIL_BALL_RETURN
    24, // COIL_SLINGSHOT_RIGHT
    25, // COIL_THUMPER_LEFT
    26, // COIL_THUMPER_RIGHT
};

// Matrix column drivers MX0..MX8
const uint8_t COLUMN_PINS[pb::LAMP_COLUMNS] = {28, 29, 30, 31, 32, 33, 48, 50, 52};

// Lamp row outputs LT0..LT3
const uint8_t LAMP_ROW_PINS[4] = {36, 37, 38, 39};

// Switch row inputs SW0..SW3. Read with analogRead, closed above SWITCH_THRESHOLD.
const uint8_t SWITCH_ROW_PINS[4] = {A12, A13, A14, A15};
const int SWITCH_THRESHOLD = 200;

// Flipper buttons, active LOW with the internal pull-up
const uint8_t FLIPPER_LEFT_PIN = 2;
const uint8_t FLIPPER_RIGHT_PIN = 3;

// Sound: one square-wave voice per pin, driven by Playtune
const uint8_t SOUND_PINS[pb::SOUND_CHANNELS] = {44, 45, 46};

// Alphanumeric displays on I2C (pins 20/21)
const uint8_t DISPLAY_LEFT_ADDR = 0x70;
const uint8_t DISPLAY_RIGHT_ADDR = 0x71;

// ESP link: Serial2 = TX2 pin 16, RX2 pin 17
#define ESP_SERIAL Serial2
