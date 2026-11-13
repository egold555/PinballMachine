// WiFi and over-the-air updates.
//
// Put your WiFi details in src/secrets.h (copy secrets.example.h). Without
// it the machine still plays, just offline.
#pragma once

#include "Machine.h"

namespace net
{
    void begin(Machine &machine);
}
