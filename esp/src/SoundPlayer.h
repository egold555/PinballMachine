// Plays Playtune scores by streaming their notes to the Mega.
//
// All music lives on the ESP. A background task walks the score and sends
// "note on" / "note off" messages at the right times. The Mega just turns its
// three square-wave voices on and off. New sounds only need an ESP update.
//
// Score format (from Playtune / miditones):
//   0x9c nn   start note nn on channel c
//   0x8c      stop channel c
//   0xe0      restart from the beginning
//   0xf0      end
//   0x00-0x7f followed by one more byte: wait that many ms (15 bit, big endian)
#pragma once

#include <Arduino.h>
#include <mutex>
#include "MegaLink.h"

enum class SoundPriority : uint8_t
{
    Low,    // e.g. the scoring blip: interrupts other Low sounds only
    Normal, // never interrupted by anything except stop()
};

class SoundPlayer
{
public:
    void begin(MegaLink &link);

    // Starts a score unless a Normal sound is already playing.
    void play(const uint8_t *score, SoundPriority priority = SoundPriority::Normal);
    void stop();
    bool playing();

private:
    static void taskEntry(void *self);
    void run();
    void step();

    MegaLink *link_ = nullptr;
    TaskHandle_t task_ = nullptr;
    std::mutex lock_;

    const uint8_t *score_ = nullptr;
    const uint8_t *cursor_ = nullptr;
    SoundPriority priority_ = SoundPriority::Low;
    bool playing_ = false;
    bool restart_ = false;
    uint32_t nextStep_ = 0;
};
