///////////////////////////////// [ MUSIC / SFX ] /////////////////////////////////
const byte PROGMEM SOUND_STARTUP[] = {
    0, 1, 0x90, 67, 0, 59, 0x80, 0, 3, 0x90, 71, 0, 55, 0x80, 0, 3, 0x90, 74, 0, 55, 0x80,
    0, 3, 0x90, 79, 0, 55, 0x80, 0, 3, 0x90, 83, 0, 56, 0x80, 0, 3, 0x90, 68, 0, 56, 0x80,
    0, 3, 0x90, 72, 0, 54, 0x80, 0, 3, 0x90, 75, 0, 54, 0x80, 0, 3, 0x90, 80, 0, 55, 0x80,
    0, 3, 0x90, 84, 0, 56, 0x80, 0, 3, 0x90, 70, 0, 61, 0x80, 0, 3, 0x90, 74, 0, 55, 0x80,
    0, 3, 0x90, 77, 0, 55, 0x80, 0, 3, 0x90, 82, 0, 55, 0x80, 0, 3, 0x90, 86, 0, 56, 0x80,
    0xf0};

const byte PROGMEM SOUND_POINT[] = {
    0x91, 50, 0, 200, 0x81, 0xf0
    //Start1, NoteNumMidi, Start delay, Delay, Stop1, End
};

const byte PROGMEM SOUND_NEWBALL[] = {
    0, 1, 0x90, 72, 0, 38, 0x80, 0, 2, 0x90, 73, 0, 34, 0x80, 0, 2, 0x90, 74, 0, 29, 0x80,
    1, 235, 0x90, 55, 0x91, 67, 0x92, 71, 0, 153, 0x80, 0x81, 0x82, 0, 0, 0x90, 74, 0x91, 77, 0, 146,
    0, 3, 0x80, 0x81, 0, 34, 0, 37, 0, 37, 0, 38, 0, 1, 0x90, 74, 0x91, 55, 0x92, 77, 0, 36,
    0, 38, 0, 38, 0, 38, 0x80, 0x82, 0x81, 0, 0, 0x90, 74, 0x91, 55, 0x92, 77, 0, 38, 0, 65,
    0x80, 0x82, 0x81, 0, 100, 0x90, 72, 0x91, 57, 0x92, 76, 0, 99, 0x81, 0x80, 0x82, 0, 100, 0x90, 59, 0x91, 71,
    0x92, 74, 0, 96, 0x80, 0x81, 0x82, 0, 100, 0x90, 60, 0x91, 67, 0x92, 72, 0, 153, 0x80, 0x81, 0x82, 0, 0,
    0x90, 64, 0, 149, 0x80, 0, 0, 0x90, 55, 0, 148, 0x80, 0, 0, 0x90, 64, 0, 148, 0x80, 0, 0,
    0x90, 48, 0x91, 60, 0, 153, 0x80, 0x81, 0xf0};

const byte PROGMEM SOUND_EXTRABALL[] = {
    0, 1, 0x90, 76, 0, 152, 0x91, 79, 0, 5, 0x80, 0, 143, 0x90, 88, 0, 5, 0x81, 0, 142, 0x91, 84,
    0, 5, 0x80, 0, 142, 0x90, 86, 0, 5, 0x81, 0, 148, 0x91, 91, 0, 5, 0x80, 0, 149, 0x81, 0xf0};

// const byte PROGMEM SOUND_ENDING_SONG[] = {
//     0, 2, 0x90, 62, 0x91, 62, 1, 26, 0x92, 64, 0, 10, 0x80, 0x81, 1, 9, 0x90, 66, 0x91, 60, 0, 10,
//     0x82, 1, 7, 0x92, 67, 0, 9, 0x80, 0x81, 1, 18, 0x90, 69, 0x91, 60, 0, 10, 0x82, 1, 9, 0x92, 70,
//     0, 10, 0x80, 0x81, 1, 7, 0x90, 71, 0x91, 62, 0, 9, 0x82, 0, 136, 0x80, 0x81, 0, 0, 0x90, 71,
//     0x91, 62, 0, 137, 0x80, 0x81, 0, 0, 0x90, 71, 0x91, 62, 0, 108, 0x80, 0, 23, 0, 24, 0x81, 0, 25,
//     0, 49, 0, 45, 0x90, 64, 0x91, 55, 0x92, 71, 0, 4, 0, 49, 0, 49, 0, 3, 0x82, 0x81, 0, 47,
//     0, 0, 0x80, 0, 49, 0, 72, 0, 1, 0x90, 65, 0x91, 55, 0x92, 71, 1, 159, 0, 49, 0, 49,
//     0, 45, 0x80, 0x82, 0, 0, 0x90, 67, 0, 4, 0, 50, 0, 50, 0, 50, 0, 50, 0, 73, 0x80,
//     0x81, 0, 1, 0x90, 67, 0x91, 48, 0x92, 76, 0, 70, 0, 95, 0x81, 0, 119, 0x91, 55, 0, 155, 0x81,
//     0, 119, 0x91, 60, 0, 106, 0x81, 0, 166, 0x82, 0, 1, 0x91, 47, 0x92, 66, 0, 31, 0x80, 0, 132,
//     0x81, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 59, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 48,
//     0x91, 67, 0, 31, 0x82, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106,
//     0x80, 0, 144, 0x81, 0, 21, 0, 1, 0x90, 52, 0, 146, 0x91, 67, 0, 17, 0x80, 0, 119, 0x90, 69,
//     0x92, 55, 0, 16, 0x81, 0, 121, 0x91, 71, 0, 16, 0x80, 0, 1, 0x82, 0, 119, 0x90, 72, 0x92, 60,
//     0, 16, 0x81, 0, 89, 0x82, 0, 29, 0x91, 74, 0, 16, 0x80, 0, 121, 0x90, 48, 0x92, 67, 0, 15,
//     0x81, 0, 148, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 166, 0, 1,
//     0x90, 47, 0x91, 66, 0, 31, 0x82, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0, 0,
//     0x90, 59, 0x92, 69, 0, 21, 0x81, 0, 84, 0x80, 0, 166, 0, 1, 0x90, 48, 0x91, 67, 0, 9, 0x82,
//     0, 154, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 144, 0x81, 0, 23,
//     0x90, 52, 0, 164, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 29,
//     0x90, 67, 0, 138, 0x91, 50, 0x92, 65, 0, 15, 0x80, 0, 148, 0x81, 0, 119, 0x90, 55, 0, 155, 0x80,
//     0, 119, 0x90, 59, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 49, 0x91, 64, 0, 31, 0x82, 0, 132, 0x80,
//     0, 119, 0x90, 54, 0, 155, 0x80, 0, 119, 0x90, 58, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 50, 0x92, 65,
//     0, 31, 0x81, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 59, 0, 106, 0x80, 0, 144,
//     0x82, 0, 21, 0, 1, 0x90, 47, 0, 146, 0x91, 67, 0, 17, 0x80, 0, 119, 0x90, 69, 0x92, 55, 0, 16,
//     0x81, 0, 121, 0x91, 71, 0, 16, 0x80, 0, 1, 0x82, 0, 119, 0x90, 72, 0x92, 59, 0, 16, 0x81, 0, 89,
//     0x82, 0, 29, 0x91, 73, 0, 16, 0x80, 0, 121, 0x90, 50, 0x92, 65, 0, 15, 0x81, 0, 148, 0x80, 0, 119,
//     0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 59, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 47, 0x91, 59, 0, 31,
//     0x82, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0, 0, 0x90, 59, 0x92, 69, 0, 21,
//     0x81, 0, 84, 0x80, 0, 166, 0, 1, 0x90, 48, 0x91, 67, 0, 9, 0x82, 0, 154, 0x80, 0, 119, 0x90, 55,
//     0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 144, 0x81, 0, 23, 0x90, 43, 0, 164, 0x80, 0, 119,
//     0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 29, 0x90, 67, 0, 138, 0x91, 48, 0x92, 76,
//     0, 15, 0x80, 0, 148, 0x81, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 64, 0, 106, 0x80, 0, 166,
//     0, 1, 0x90, 47, 0x91, 74, 0, 31, 0x82, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119,
//     0x90, 62, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 46, 0x92, 73, 0, 31, 0x81, 0, 132, 0x80, 0, 119,
//     0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 61, 0, 106, 0x80, 0, 144, 0x82, 0, 21, 0, 1, 0x90, 49,
//     0x91, 79, 0, 164, 0x80, 0, 119, 0x90, 81, 0x92, 55, 0, 10, 0x81, 0, 145, 0x82, 0x80, 0, 119, 0x90, 64,
//     0, 106, 0x80, 0, 29, 0x90, 79, 0, 138, 0x91, 50, 0x92, 74, 0, 15, 0x80, 0, 148, 0x81, 0, 119,
//     0x90, 57, 0, 155, 0x80, 0, 119, 0x90, 65, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 49, 0x91, 73, 0, 31,
//     0x82, 0, 132, 0x80, 0, 119, 0x90, 57, 0, 155, 0x80, 0, 119, 0x90, 65, 0, 106, 0x80, 0, 166, 0, 1,
//     0x90, 48, 0x92, 72, 0, 31, 0x81, 0, 132, 0x80, 0, 119, 0x90, 57, 0, 155, 0x80, 0, 119, 0x90, 65,
//     0, 106, 0x80, 0, 166, 0x82, 0, 1, 0x90, 47, 0x91, 77, 0, 164, 0x80, 0, 119, 0x90, 79, 0x92, 55,
//     0, 10, 0x81, 0, 145, 0x82, 0x80, 0, 119, 0x90, 65, 0, 106, 0x80, 0, 29, 0x90, 77, 0, 138, 0x91, 36,
//     0x92, 60, 0, 15, 0x80, 0, 148, 0x81, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 64, 0, 106,
//     0x80, 0, 166, 0, 1, 0x90, 43, 0x91, 65, 0, 31, 0x82, 0, 132, 0x80, 0, 119, 0, 0, 0x90, 55,
//     0x92, 67, 0, 10, 0x81, 0, 98, 0x80, 0, 23, 0, 49, 0, 49, 0, 45, 0, 0, 0x90, 55, 0x91, 71,
//     0, 4, 0, 6, 0x82, 0, 43, 0, 50, 0, 50, 0, 0, 0x80, 0, 50, 0, 73, 0x81, 0, 1,
//     0x90, 53, 0x91, 71, 0x92, 76, 0, 70, 0, 77, 0x81, 0x82, 0x80, 0, 0, 0x90, 71, 0x91, 53, 0x92, 76,
//     0, 137, 0x81, 0x80, 0x82, 0, 0, 0x90, 53, 0x91, 71, 0x92, 76, 1, 155, 0x81, 0x82, 0, 0, 0x91, 47,
//     0x92, 65, 0, 16, 0x80, 0, 120, 0, 1, 0x90, 48, 0, 15, 0x81, 0x82, 3, 48, 0x80, 0, 0, 0, 2,
//     0x90, 62, 0x91, 62, 1, 26, 0x92, 64, 0, 10, 0x80, 0x81, 1, 9, 0x90, 66, 0x91, 60, 0, 10, 0x82,
//     1, 7, 0x92, 67, 0, 9, 0x80, 0x81, 1, 18, 0x90, 69, 0x91, 60, 0, 10, 0x82, 1, 9, 0x92, 70,
//     0, 10, 0x80, 0x81, 1, 7, 0x90, 71, 0x91, 62, 0, 9, 0x82, 0, 136, 0x80, 0x81, 0, 0, 0x90, 71,
//     0x91, 62, 0, 137, 0x80, 0x81, 0, 0, 0x90, 71, 0x91, 62, 0, 108, 0x80, 0, 23, 0, 24, 0x81, 0, 25,
//     0, 49, 0, 45, 0x90, 64, 0x91, 55, 0x92, 71, 0, 4, 0, 49, 0, 49, 0, 3, 0x82, 0x81, 0, 47,
//     0, 0, 0x80, 0, 49, 0, 72, 0, 1, 0x90, 65, 0x91, 55, 0x92, 71, 1, 159, 0, 49, 0, 49,
//     0, 45, 0x80, 0x82, 0, 0, 0x90, 67, 0, 4, 0, 50, 0, 50, 0, 50, 0, 50, 0, 73, 0x80,
//     0x81, 0, 1, 0x90, 67, 0x91, 48, 0x92, 76, 0, 70, 0, 95, 0x81, 0, 119, 0x91, 55, 0, 155, 0x81,
//     0, 119, 0x91, 60, 0, 106, 0x81, 0, 166, 0x82, 0, 1, 0x91, 47, 0x92, 66, 0, 31, 0x80, 0, 132,
//     0x81, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 59, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 48,
//     0x91, 67, 0, 31, 0x82, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106,
//     0x80, 0, 144, 0x81, 0, 21, 0, 1, 0x90, 52, 0, 146, 0x91, 67, 0, 17, 0x80, 0, 119, 0x90, 69,
//     0x92, 55, 0, 16, 0x81, 0, 121, 0x91, 71, 0, 16, 0x80, 0, 1, 0x82, 0, 119, 0x90, 72, 0x92, 60,
//     0, 16, 0x81, 0, 89, 0x82, 0, 29, 0x91, 74, 0, 16, 0x80, 0, 121, 0x90, 48, 0x92, 67, 0, 15,
//     0x81, 0, 148, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 166, 0, 1,
//     0x90, 47, 0x91, 66, 0, 31, 0x82, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0, 0,
//     0x90, 59, 0x92, 69, 0, 21, 0x81, 0, 84, 0x80, 0, 166, 0, 1, 0x90, 48, 0x91, 67, 0, 9, 0x82,
//     0, 154, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 144, 0x81, 0, 23,
//     0x90, 52, 0, 164, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 29,
//     0x90, 67, 0, 138, 0x91, 50, 0x92, 65, 0, 15, 0x80, 0, 148, 0x81, 0, 119, 0x90, 55, 0, 155, 0x80,
//     0, 119, 0x90, 59, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 49, 0x91, 64, 0, 31, 0x82, 0, 132, 0x80,
//     0, 119, 0x90, 54, 0, 155, 0x80, 0, 119, 0x90, 58, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 50, 0x92, 65,
//     0, 31, 0x81, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 59, 0, 106, 0x80, 0, 144,
//     0x82, 0, 21, 0, 1, 0x90, 47, 0, 146, 0x91, 67, 0, 17, 0x80, 0, 119, 0x90, 69, 0x92, 55, 0, 16,
//     0x81, 0, 121, 0x91, 71, 0, 16, 0x80, 0, 1, 0x82, 0, 119, 0x90, 72, 0x92, 59, 0, 16, 0x81, 0, 89,
//     0x82, 0, 29, 0x91, 73, 0, 16, 0x80, 0, 121, 0x90, 50, 0x92, 65, 0, 15, 0x81, 0, 148, 0x80, 0, 119,
//     0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 59, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 47, 0x91, 59, 0, 31,
//     0x82, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0, 0, 0x90, 59, 0x92, 69, 0, 21,
//     0x81, 0, 84, 0x80, 0, 166, 0, 1, 0x90, 48, 0x91, 67, 0, 9, 0x82, 0, 154, 0x80, 0, 119, 0x90, 55,
//     0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 144, 0x81, 0, 23, 0x90, 43, 0, 164, 0x80, 0, 119,
//     0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 60, 0, 106, 0x80, 0, 29, 0x90, 67, 0, 138, 0x91, 48, 0x92, 76,
//     0, 15, 0x80, 0, 148, 0x81, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 64, 0, 106, 0x80, 0, 166,
//     0, 1, 0x90, 47, 0x91, 74, 0, 31, 0x82, 0, 132, 0x80, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119,
//     0x90, 62, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 46, 0x92, 73, 0, 31, 0x81, 0, 132, 0x80, 0, 119,
//     0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 61, 0, 106, 0x80, 0, 144, 0x82, 0, 21, 0, 1, 0x90, 49,
//     0x91, 79, 0, 164, 0x80, 0, 119, 0x90, 81, 0x92, 55, 0, 10, 0x81, 0, 145, 0x82, 0x80, 0, 119, 0x90, 64,
//     0, 106, 0x80, 0, 29, 0x90, 79, 0, 138, 0x91, 50, 0x92, 74, 0, 15, 0x80, 0, 148, 0x81, 0, 119,
//     0x90, 57, 0, 155, 0x80, 0, 119, 0x90, 65, 0, 106, 0x80, 0, 166, 0, 1, 0x90, 49, 0x91, 73, 0, 31,
//     0x82, 0, 132, 0x80, 0, 119, 0x90, 57, 0, 155, 0x80, 0, 119, 0x90, 65, 0, 106, 0x80, 0, 166, 0, 1,
//     0x90, 48, 0x92, 72, 0, 31, 0x81, 0, 132, 0x80, 0, 119, 0x90, 57, 0, 155, 0x80, 0, 119, 0x90, 65,
//     0, 106, 0x80, 0, 166, 0x82, 0, 1, 0x90, 47, 0x91, 77, 0, 164, 0x80, 0, 119, 0x90, 79, 0x92, 55,
//     0, 10, 0x81, 0, 145, 0x82, 0x80, 0, 119, 0x90, 65, 0, 106, 0x80, 0, 29, 0x90, 77, 0, 138, 0x91, 36,
//     0x92, 60, 0, 15, 0x80, 0, 148, 0x81, 0, 119, 0x90, 55, 0, 155, 0x80, 0, 119, 0x90, 64, 0, 106,
//     0x80, 0, 166, 0, 1, 0x90, 43, 0x91, 65, 0, 31, 0x82, 0, 132, 0x80, 0, 119, 0, 0, 0x90, 55,
//     0x92, 67, 0, 10, 0x81, 0, 98, 0x80, 0, 23, 0, 49, 0, 49, 0, 45, 0, 0, 0x90, 55, 0x91, 71,
//     0, 4, 0, 6, 0x82, 0, 43, 0, 50, 0, 50, 0, 0, 0x80, 0, 50, 0, 73, 0x81, 0, 1,
//     0x90, 53, 0x91, 71, 0x92, 76, 0, 70, 0, 77, 0x81, 0x82, 0x80, 0, 0, 0x90, 71, 0x91, 53, 0x92, 76,
//     0, 137, 0x81, 0x80, 0x82, 0, 0, 0x90, 53, 0x91, 71, 0x92, 76, 1, 155, 0x81, 0x82, 0, 0, 0x91, 47,
//     0x92, 65, 0, 16, 0x80, 0, 120, 0, 1, 0x90, 48, 0, 15, 0x81, 0x82, 3, 48, 0x80, 0xf0};

const byte PROGMEM SOUND_TILT[] = {
    0x90, 63, 0, 225, 0x80, 0, 112, 0x90, 69, 0x91, 66, 0, 225, 0x80, 0x81, 0, 112, 0x90, 63, 0, 225,
    0x80, 0, 112, 0x90, 72, 0x91, 66, 0, 225, 0x80, 0x81, 0xf0};

const byte PROGMEM SOUND_NAME_ENTER[] = {
    0, 1, 0x90, 83, 0, 73, 0x80, 0, 4, 0x90, 88, 2, 8, 0x80, 0xf0};

const byte PROGMEM SOUND_ENDING_SONG[] = {
    0, 0, 0x90, 59, 0, 105, 0x80, 0, 21, 0x90, 47, 0x91, 59, 0, 105, 0x80, 0x81, 0, 21, 0x90, 35,
    0x91, 66, 0, 211, 0x80, 0x81, 0, 42, 0x90, 42, 0x91, 66, 0, 148, 0x80, 0, 63, 0x81, 0, 42, 0x90, 47,
    0x91, 66, 0, 211, 0x81, 0, 42, 0x91, 66, 0, 63, 0x80, 0, 63, 0x90, 46, 0, 84, 0x81, 0, 21,
    0x80, 0, 21, 0x90, 66, 0x91, 42, 0, 211, 0x80, 0x81, 0, 42, 0x90, 66, 0x91, 47, 0, 148, 0x81, 0, 105,
    0x91, 47, 0, 190, 0x80, 0, 63, 0x90, 66, 0, 190, 0x81, 0, 21, 0x80, 0, 42, 0x90, 35, 0x91, 66,
    0, 211, 0x80, 0x81, 0, 42, 0x90, 35, 0x91, 64, 0, 148, 0x80, 0, 63, 0x81, 0, 42, 0x90, 47, 0x91, 63,
    0, 211, 0x81, 0, 42, 0x91, 61, 0, 63, 0x80, 0, 42, 0x81, 0, 21, 0x90, 47, 0x91, 59, 0, 105,
    0x80, 0, 21, 0x90, 42, 0, 211, 0x80, 0, 42, 0x90, 47, 0, 190, 0x81, 0, 21, 0x80, 0, 42, 0x90, 47,
    0, 211, 0x80, 0, 42, 0x90, 64, 0x91, 42, 0, 148, 0x81, 0, 63, 0x80, 0, 42, 0x90, 33, 0x91, 64,
    0, 148, 0x80, 0, 63, 0x81, 0, 42, 0x90, 33, 0x91, 64, 0, 148, 0x80, 0, 63, 0x81, 0, 42, 0x90, 45,
    0x91, 64, 0, 211, 0x81, 0, 42, 0x91, 64, 0, 63, 0x80, 0, 42, 0x81, 0, 21, 0x90, 45, 0x91, 64,
    0, 105, 0x80, 0, 21, 0x90, 40, 0, 211, 0x81, 0x80, 0, 42, 0x90, 64, 0x91, 45, 0, 148, 0x81, 0, 63,
    0x80, 0, 42, 0x90, 45, 0, 211, 0x80, 0, 42, 0x90, 64, 0x91, 33, 0, 211, 0x80, 0x81, 0, 42, 0x90, 64,
    0x91, 33, 0, 211, 0x80, 0x81, 0, 42, 0x90, 62, 0x91, 33, 0, 148, 0x81, 0, 63, 0x80, 0, 42, 0x90, 45,
    0x91, 61, 0, 211, 0x81, 0, 42, 0x91, 62, 0, 63, 0x80, 0, 63, 0x90, 45, 0, 84, 0x81, 0, 21,
    0x80, 0, 21, 0x90, 64, 0x91, 40, 0, 127, 0x80, 0x90, 63, 0, 84, 0x81, 0, 42, 0x80, 0x90, 45, 0x91, 64,
    0, 148, 0x80, 0, 63, 0x81, 0, 42, 0x90, 45, 0, 211, 0x80, 0, 42, 0x90, 62, 0x91, 33, 0, 105,
    0x80, 0, 21, 0x90, 62, 0, 84, 0x81, 0, 21, 0x80, 0, 21, 0x90, 31, 0x91, 62, 0, 211, 0x80, 0x81,
    0, 42, 0x90, 31, 0x91, 62, 0, 148, 0x80, 0, 63, 0x81, 0, 42, 0x90, 43, 0x91, 62, 0, 105, 0x81,
    0, 21, 0x91, 62, 0, 190, 0x80, 0, 63, 0x90, 43, 0, 63, 0x81, 0, 42, 0x80, 0, 21, 0x90, 62,
    0x91, 38, 0, 211, 0x80, 0x81, 0, 42, 0x90, 62, 0x91, 43, 0, 148, 0x81, 0, 63, 0x80, 0, 42, 0x90, 43,
    0, 211, 0x80, 0, 42, 0x90, 62, 0x91, 31, 0, 211, 0x80, 0x81, 0, 42, 0x90, 62, 0x91, 31, 0, 105,
    0x80, 0, 21, 0x90, 62, 0, 84, 0x81, 0, 21, 0x80, 0, 21, 0x90, 31, 0x91, 60, 0, 148, 0x80, 0, 63,
    0x81, 0, 42, 0x90, 43, 0x91, 59, 0, 211, 0x81, 0, 42, 0x91, 60, 0, 63, 0x80, 0, 63, 0x90, 43,
    0, 84, 0x81, 0, 21, 0x80, 0, 21, 0x90, 62, 0x91, 38, 0, 127, 0x80, 0x90, 61, 0, 84, 0x81, 0, 42,
    0x80, 0x90, 43, 0x91, 62, 0, 148, 0x80, 0, 63, 0x81, 0, 42, 0x90, 43, 0, 211, 0x80, 0, 42, 0x90, 62,
    0x91, 31, 0, 211, 0x80, 0x81, 0, 42, 0x90, 61, 0x91, 30, 0, 211, 0x81, 0, 42, 0x91, 30, 0, 63,
    0x80, 0, 63, 0x90, 61, 0, 21, 0x81, 0, 105, 0x91, 42, 0, 105, 0x80, 0, 21, 0x90, 59, 0, 105,
    0x80, 0, 21, 0x90, 61, 0, 63, 0x81, 0, 63, 0x80, 0x90, 42, 0x91, 57, 0, 105, 0x80, 0, 21, 0x81,
    0x90, 37, 0x91, 58, 1, 61, 0x80, 0, 63, 0x90, 42, 0, 63, 0x81, 0, 42, 0x80, 0, 21, 0x90, 37,
    1, 61, 0x80, 0, 63, 0x90, 30, 0, 84, 0x80, 0, 42, 0x90, 71, 0x91, 58, 0x92, 30, 0x93, 54, 0x94, 49,
    0x95, 42, 0, 148, 0x82, 0x81, 0x83, 0x84, 0x85, 1, 40, 0x80, 0, 63, 0x90, 69, 1, 61, 0x80, 0, 63,
    0x90, 69, 0, 105, 0x80, 0, 21, 0x90, 66, 1, 188, 0x80, 0, 63, 0x90, 64, 1, 188, 0x80, 0, 63,
    0x90, 63, 0x91, 35, 0x92, 66, 0x93, 59, 0x94, 54, 0x95, 47, 0, 211, 0x81, 0x80, 0x83, 0x84, 0x85, 0, 42,
    0x90, 35, 0x91, 63, 0x93, 59, 0x94, 54, 0x95, 47, 0, 211, 0x80, 0x81, 0x83, 0x84, 0x85, 1, 40, 0x90, 33,
    0x91, 64, 0x93, 61, 0x94, 57, 0x95, 52, 0, 127, 0x82, 1, 82, 0x81, 0x83, 0x84, 0x85, 0x80, 0, 42, 0x90, 66,
    0x91, 38, 0x92, 62, 0x93, 57, 0x94, 50, 1, 19, 0x81, 0, 169, 0x80, 0x82, 0x83, 0x84, 0, 63, 0x90, 40,
    0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 211, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 42, 0x90, 40,
    0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 1, 188, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 63, 0x91, 64,
    0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 211, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 42, 0x91, 64, 0x92, 59,
    0x93, 56, 0x94, 52, 0x95, 47, 0, 105, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 21, 0x91, 64, 0x92, 59, 0x93, 56,
    0x94, 52, 0x95, 47, 0, 105, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 21, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52,
    0x95, 47, 0, 211, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 42, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47,
    0, 211, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 42, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 211,
    0x81, 0x82, 0x83, 0x84, 0x85, 0, 42, 0x80, 0x90, 64, 0x91, 59, 0x92, 56, 0x93, 52, 0x94, 47, 0x95, 40, 0, 105,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 21, 0x90, 64, 0x91, 59, 0x92, 56, 0x93, 52, 0x94, 47, 0x95, 40, 0, 105,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 21, 0x90, 35, 0x91, 63, 0x92, 59, 0x93, 54, 0x94, 47, 0, 211, 0x80,
    0x81, 0x82, 0x83, 0x84, 0, 42, 0x90, 35, 0x91, 63, 0x92, 59, 0x93, 54, 0x94, 47, 0, 211, 0x80, 0x81, 0x82,
    0x83, 0x84, 1, 40, 0x90, 33, 0x91, 64, 0x92, 61, 0x93, 57, 0x94, 52, 0x95, 45, 1, 210, 0x80, 0x81, 0x82,
    0x83, 0x84, 0x85, 0, 42, 0x90, 38, 0x91, 66, 0x92, 62, 0x93, 57, 0x94, 50, 1, 19, 0x80, 0, 169, 0x81,
    0x82, 0x83, 0x84, 0, 63, 0x90, 40, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 211, 0x80, 0x81,
    0x82, 0x83, 0x84, 0x85, 0, 42, 0x90, 40, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 211, 0x81,
    0x82, 0x83, 0x84, 0x85, 0, 42, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 105, 0x81, 0x82, 0x83,
    0x84, 0x85, 0, 21, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 105, 0x81, 0x82, 0x83, 0x84, 0x85,
    0, 21, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 211, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 42,
    0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 105, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 21, 0x91, 64,
    0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 105, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 21, 0x91, 64, 0x92, 59,
    0x93, 56, 0x94, 52, 0x95, 47, 0, 211, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 42, 0x91, 64, 0x92, 59, 0x93, 56,
    0x94, 52, 0x95, 47, 0, 63, 0x80, 0, 148, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 42, 0x90, 64, 0x91, 59, 0x92, 56,
    0x93, 52, 0x94, 47, 0x95, 40, 0, 211, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 42, 0x90, 64, 0x91, 59, 0x92, 56,
    0x93, 52, 0x94, 47, 0x95, 40, 0, 211, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 0, 0x90, 59, 0, 105, 0x80,
    0, 21, 0x90, 47, 0x91, 59, 0, 105, 0x80, 0x81, 0, 21, 0x90, 35, 0x91, 66, 0, 211, 0x80, 0x81, 0, 42,
    0x90, 42, 0x91, 66, 0, 148, 0x80, 0, 63, 0x81, 0, 42, 0x90, 47, 0x91, 66, 0, 211, 0x81, 0, 42,
    0x91, 66, 0, 63, 0x80, 0, 63, 0x90, 46, 0, 84, 0x81, 0, 21, 0x80, 0, 21, 0x90, 66, 0x91, 42,
    0, 211, 0x80, 0x81, 0, 42, 0x90, 66, 0x91, 47, 0, 148, 0x81, 0, 105, 0x91, 47, 0, 190, 0x80, 0, 63,
    0x90, 66, 0, 190, 0x81, 0, 21, 0x80, 0, 42, 0x90, 35, 0x91, 66, 0, 211, 0x80, 0x81, 0, 42, 0x90, 35,
    0x91, 64, 0, 148, 0x80, 0, 63, 0x81, 0, 42, 0x90, 47, 0x91, 63, 0, 211, 0x81, 0, 42, 0x91, 61,
    0, 63, 0x80, 0, 42, 0x81, 0, 21, 0x90, 47, 0x91, 59, 0, 105, 0x80, 0, 21, 0x90, 42, 0, 211,
    0x80, 0, 42, 0x90, 47, 0, 190, 0x81, 0, 21, 0x80, 0, 42, 0x90, 47, 0, 211, 0x80, 0, 42, 0x90, 64,
    0x91, 42, 0, 148, 0x81, 0, 63, 0x80, 0, 42, 0x90, 33, 0x91, 64, 0, 148, 0x80, 0, 63, 0x81, 0, 42,
    0x90, 33, 0x91, 64, 0, 148, 0x80, 0, 63, 0x81, 0, 42, 0x90, 45, 0x91, 64, 0, 211, 0x81, 0, 42,
    0x91, 64, 0, 63, 0x80, 0, 42, 0x81, 0, 21, 0x90, 45, 0x91, 64, 0, 105, 0x80, 0, 21, 0x90, 40,
    0, 211, 0x81, 0x80, 0, 42, 0x90, 64, 0x91, 45, 0, 148, 0x81, 0, 63, 0x80, 0, 42, 0x90, 45, 0, 211,
    0x80, 0, 42, 0x90, 64, 0x91, 33, 0, 211, 0x80, 0x81, 0, 42, 0x90, 64, 0x91, 33, 0, 211, 0x80, 0x81,
    0, 42, 0x90, 62, 0x91, 33, 0, 148, 0x81, 0, 63, 0x80, 0, 42, 0x90, 45, 0x91, 61, 0, 211, 0x81,
    0, 42, 0x91, 62, 0, 63, 0x80, 0, 63, 0x90, 45, 0, 84, 0x81, 0, 21, 0x80, 0, 21, 0x90, 64,
    0x91, 40, 0, 127, 0x80, 0x90, 63, 0, 84, 0x81, 0, 42, 0x80, 0x90, 45, 0x91, 64, 0, 148, 0x80, 0, 63,
    0x81, 0, 42, 0x90, 45, 0, 211, 0x80, 0, 42, 0x90, 62, 0x91, 33, 0, 105, 0x80, 0, 21, 0x90, 62,
    0, 84, 0x81, 0, 21, 0x80, 0, 21, 0x90, 31, 0x91, 62, 0, 211, 0x80, 0x81, 0, 42, 0x90, 31, 0x91, 62,
    0, 148, 0x80, 0, 63, 0x81, 0, 42, 0x90, 43, 0x91, 62, 0, 105, 0x81, 0, 21, 0x91, 62, 0, 190,
    0x80, 0, 63, 0x90, 43, 0, 63, 0x81, 0, 42, 0x80, 0, 21, 0x90, 62, 0x91, 38, 0, 211, 0x80, 0x81,
    0, 42, 0x90, 62, 0x91, 43, 0, 148, 0x81, 0, 63, 0x80, 0, 42, 0x90, 43, 0, 211, 0x80, 0, 42,
    0x90, 62, 0x91, 31, 0, 211, 0x80, 0x81, 0, 42, 0x90, 62, 0x91, 31, 0, 105, 0x80, 0, 21, 0x90, 62,
    0, 84, 0x81, 0, 21, 0x80, 0, 21, 0x90, 31, 0x91, 60, 0, 148, 0x80, 0, 63, 0x81, 0, 42, 0x90, 43,
    0x91, 59, 0, 211, 0x81, 0, 42, 0x91, 60, 0, 63, 0x80, 0, 63, 0x90, 43, 0, 84, 0x81, 0, 21,
    0x80, 0, 21, 0x90, 62, 0x91, 38, 0, 127, 0x80, 0x90, 61, 0, 84, 0x81, 0, 42, 0x80, 0x90, 43, 0x91, 62,
    0, 148, 0x80, 0, 63, 0x81, 0, 42, 0x90, 43, 0, 211, 0x80, 0, 42, 0x90, 62, 0x91, 31, 0, 211,
    0x80, 0x81, 0, 42, 0x90, 61, 0x91, 30, 0, 211, 0x81, 0, 42, 0x91, 30, 0, 63, 0x80, 0, 63, 0x90, 61,
    0, 21, 0x81, 0, 105, 0x91, 42, 0, 105, 0x80, 0, 21, 0x90, 59, 0, 105, 0x80, 0, 21, 0x90, 61,
    0, 63, 0x81, 0, 63, 0x80, 0x90, 42, 0x91, 57, 0, 105, 0x80, 0, 21, 0x81, 0x90, 37, 0x91, 58, 1, 61,
    0x80, 0, 63, 0x90, 42, 0, 63, 0x81, 0, 42, 0x80, 0, 21, 0x90, 37, 1, 61, 0x80, 0, 63, 0x90, 30,
    0, 84, 0x80, 0, 42, 0x90, 71, 0x91, 58, 0x92, 30, 0x93, 54, 0x94, 49, 0x95, 42, 0, 148, 0x82, 0x81,
    0x83, 0x84, 0x85, 1, 40, 0x80, 0, 63, 0x90, 69, 1, 61, 0x80, 0, 63, 0x90, 69, 0, 105, 0x80, 0, 21,
    0x90, 66, 1, 188, 0x80, 0, 63, 0x90, 64, 1, 188, 0x80, 0, 63, 0x90, 63, 0x91, 35, 0x92, 66, 0x93, 59,
    0x94, 54, 0x95, 47, 0, 211, 0x81, 0x80, 0x83, 0x84, 0x85, 0, 42, 0x90, 35, 0x91, 63, 0x93, 59, 0x94, 54,
    0x95, 47, 0, 211, 0x80, 0x81, 0x83, 0x84, 0x85, 1, 40, 0x90, 33, 0x91, 64, 0x93, 61, 0x94, 57, 0x95, 52,
    0, 127, 0x82, 1, 82, 0x81, 0x83, 0x84, 0x85, 0x80, 0, 42, 0x90, 66, 0x91, 38, 0x92, 62, 0x93, 57, 0x94, 50,
    1, 19, 0x81, 0, 169, 0x80, 0x82, 0x83, 0x84, 0, 63, 0x90, 40, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52,
    0x95, 47, 0, 211, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 42, 0x90, 40, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52,
    0x95, 47, 1, 188, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 63, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47,
    0, 211, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 42, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 105,
    0x81, 0x82, 0x83, 0x84, 0x85, 0, 21, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 105, 0x81, 0x82,
    0x83, 0x84, 0x85, 0, 21, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 211, 0x81, 0x82, 0x83, 0x84,
    0x85, 0, 42, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 211, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 42,
    0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 211, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 42, 0x80, 0x90, 64,
    0x91, 59, 0x92, 56, 0x93, 52, 0x94, 47, 0x95, 40, 0, 105, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 21, 0x90, 64,
    0x91, 59, 0x92, 56, 0x93, 52, 0x94, 47, 0x95, 40, 0, 105, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 21, 0x90, 35,
    0x91, 63, 0x92, 59, 0x93, 54, 0x94, 47, 0, 211, 0x80, 0x81, 0x82, 0x83, 0x84, 0, 42, 0x90, 35, 0x91, 63,
    0x92, 59, 0x93, 54, 0x94, 47, 0, 211, 0x80, 0x81, 0x82, 0x83, 0x84, 1, 40, 0x90, 33, 0x91, 64, 0x92, 61,
    0x93, 57, 0x94, 52, 0x95, 45, 1, 210, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 42, 0x90, 38, 0x91, 66, 0x92, 62,
    0x93, 57, 0x94, 50, 1, 19, 0x80, 0, 169, 0x81, 0x82, 0x83, 0x84, 0, 63, 0x90, 40, 0x91, 64, 0x92, 59,
    0x93, 56, 0x94, 52, 0x95, 47, 0, 211, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 42, 0x90, 40, 0x91, 64, 0x92, 59,
    0x93, 56, 0x94, 52, 0x95, 47, 0, 211, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 42, 0x91, 64, 0x92, 59, 0x93, 56,
    0x94, 52, 0x95, 47, 0, 105, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 21, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52,
    0x95, 47, 0, 105, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 21, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47,
    0, 211, 0x81, 0x82, 0x83, 0x84, 0x85, 0, 42, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 105,
    0x81, 0x82, 0x83, 0x84, 0x85, 0, 21, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 105, 0x81, 0x82,
    0x83, 0x84, 0x85, 0, 21, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 211, 0x81, 0x82, 0x83, 0x84,
    0x85, 0, 42, 0x91, 64, 0x92, 59, 0x93, 56, 0x94, 52, 0x95, 47, 0, 63, 0x80, 0, 148, 0x81, 0x82, 0x83,
    0x84, 0x85, 0, 42, 0x90, 64, 0x91, 59, 0x92, 56, 0x93, 52, 0x94, 47, 0x95, 40, 0, 211, 0x80, 0x81, 0x82,
    0x83, 0x84, 0x85, 0, 42, 0x90, 64, 0x91, 59, 0x92, 56, 0x93, 52, 0x94, 47, 0x95, 40, 0, 211, 0x80, 0x81,
    0x82, 0x83, 0x84, 0x85, 0xf0};

///////////////////////////////// [ END MUSIC / SFX ] /////////////////////////////////