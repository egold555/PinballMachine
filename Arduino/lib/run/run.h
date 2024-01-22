#include "Arduino.h"

void run(void (*)(void), unsigned long);

void runDelay(void (*)(void), unsigned long, long repeatCount);

void snooze(unsigned long);

void runner(void);

void removeTask(void);

void removeTask(void (*userfunction)(void));