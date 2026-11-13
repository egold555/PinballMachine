// Hardware map of the pinball machine: every switch, lamp and coil, by ID.
//
// Shared by the Mega (I/O board) and the ESP (game logic). IDs are what goes
// over the serial link. They come straight from how the machine is wired, as
// recovered from the original Arduino code (Arduino/lib/PinballMachine).
//
// Matrix IDs are  column * 4 + row,  where column is the MX0..MX8 output and
// row is the SW0..SW3 input (switches) or LT0..LT3 output (lamps).
#pragma once

#include <stdint.h>

namespace pb
{

    // ---------------------------------------------------------------- Switches
    enum SwitchId : uint8_t
    {
        // column 0
        SW_BALL_RETURN = 0,
        SW_TILT = 1,
        SW_RIGHT_SPINNER = 2,
        SW_RIGHT_EXTRA_BALL_LANE = 3,
        // column 1
        SW_A = 4,
        SW_START = 5,
        SW_LEFT_TARGET = 6,
        SW_LEFT_SLINGSHOT = 7,
        // column 2
        SW_B = 8,
        SW_LEFT_THUMPER_BUMPER = 9,
        SW_LEFT_BUMPER = 10,
        SW_LEFT_EXTRA_BALL_LANE = 11,
        // column 3
        SW_C = 12,
        SW_RIGHT_THUMPER_BUMPER = 13,
        SW_LEFT_SPINNER = 14,
        SW_LEFT_ADVANCE_LANE = 15,
        // column 4 (row 1 is not connected)
        SW_D = 16,
        SW_RIGHT_TARGET = 18,
        SW_RIGHT_SLINGSHOT = 19,
        // column 5 (row 1 is not connected)
        SW_CENTER_TARGET = 20,
        SW_RIGHT_BUMPER = 22,
        SW_RIGHT_ADVANCE_LANE = 23,
        // not in the matrix: read directly from digital pins
        SW_FLIPPER_LEFT = 24,
        SW_FLIPPER_RIGHT = 25,
    };

    const uint8_t SWITCH_COUNT = 26;
    const uint8_t SWITCH_MATRIX_COLUMNS = 6; // columns 0-5 have switches
    const uint8_t SWITCH_BYTES = (SWITCH_COUNT + 7) / 8;

    // Matrix positions with nothing wired to them. Never read, never reported.
    inline bool switchIsUnused(uint8_t id) { return id == 17 || id == 21; }

    // ------------------------------------------------------------------- Lamps
    enum LampId : uint8_t
    {
        // column 0 (row 0 not connected)
        LAMP_BONUS_8000 = 1,
        LAMP_BONUS_9000 = 2,
        LAMP_EXTRA_BALL_RIGHT = 3,
        // column 1
        LAMP_A = 4,
        LAMP_BONUS_6000 = 5,
        LAMP_TARGET_LEFT = 6, // the "1" light
        LAMP_BONUS_1000 = 7,
        // column 2
        LAMP_B = 8,
        LAMP_BONUS_5000 = 9,
        LAMP_DOUBLE_BONUS = 10,
        LAMP_EXTRA_BALL_LEFT = 11,
        // column 3
        LAMP_C = 12,
        LAMP_BONUS_7000 = 13,
        LAMP_BONUS_10000 = 14,
        LAMP_BONUS_2000 = 15,
        // column 4
        LAMP_D = 16,
        LAMP_BONUS_3000 = 17,
        LAMP_TARGET_RIGHT = 18, // the "2" light
        LAMP_SAME_PLAYER_SHOOTS = 19,
        // column 5 (row 3 not connected)
        LAMP_TARGET_CENTER = 20, // the "3" light
        LAMP_BONUS_4000 = 21,
        LAMP_TRIPLE_BONUS = 22,
        // column 6
        LAMP_BALL_2 = 24,
        LAMP_BALL_3 = 25,
        LAMP_BALL_4 = 26,
        LAMP_BALL_5 = 27,
        // column 7
        LAMP_PLAYER_1 = 28,
        LAMP_PLAYER_2 = 29,
        LAMP_PLAYER_3 = 30,
        LAMP_PLAYER_4 = 31,
        // column 8 (rows 0-2 not connected)
        LAMP_BALL_1 = 35,
    };

    const uint8_t LAMP_COUNT = 36;
    const uint8_t LAMP_COLUMNS = 9;
    const uint8_t LAMP_BYTES = (LAMP_COUNT + 7) / 8;

    // ------------------------------------------------------------------- Coils
    enum CoilId : uint8_t
    {
        COIL_SLINGSHOT_LEFT = 0,
        COIL_BALL_RETURN = 1,
        COIL_SLINGSHOT_RIGHT = 2,
        COIL_THUMPER_LEFT = 3,
        COIL_THUMPER_RIGHT = 4,
    };

    const uint8_t COIL_COUNT = 5;

    // ------------------------------------------------------------------- Sound
    const uint8_t SOUND_CHANNELS = 3; // square-wave voices on the Mega

    // ----------------------------------------------------------------- Display
    const uint8_t DISPLAY_CHARS = 8; // two 4-character alphanumeric displays

} // namespace pb
