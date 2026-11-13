// Pinball machine brain (Seeed XIAO ESP32-C6).
//
// All game logic lives here. The Mega is only an I/O board: it reports
// switches and does what this side tells it. See ../README.md.

#include <Arduino.h>
#include "MegaLink.h"
#include "SoundPlayer.h"
#include "Machine.h"
#include "Net.h"
#include "games/OriginalGame.h"

// UART to the Mega: XIAO pin D7 (RX) <- Mega TX2, D6 (TX) -> Mega RX2
static const int MEGA_RX_PIN = D7;
static const int MEGA_TX_PIN = D6;

static MegaLink megaLink;
static SoundPlayer sound;
static Machine machine;
static OriginalGame originalGame;

void setup()
{
    Serial.begin(115200);
    Serial.println("\n[boot] pinball brain starting");

    machine.begin(megaLink, sound);
    sound.begin(megaLink);
    megaLink.begin(Serial1, MEGA_RX_PIN, MEGA_TX_PIN, &machine);
    net::begin(machine);
}

// Game code runs here and may block. Everything time critical is in
// background tasks (ESP) or on the Mega.
void loop()
{
    GameMode &mode = originalGame;
    Serial.printf("[game] %s\n", mode.name());
    mode.run(machine);
}
