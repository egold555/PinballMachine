// A way to play the machine. Add a new mode by subclassing this.
#pragma once

#include "../Machine.h"

class GameMode
{
public:
    virtual ~GameMode() {}
    virtual const char *name() const = 0;

    // Plays one full cycle: attract mode, wait for start, play the game,
    // game over. Returns when the machine is back to idle.
    virtual void run(Machine &m) = 0;
};
