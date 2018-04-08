#include "run.h"

#define NUMRUNS 20
byte curtask;

void (*runfunction[NUMRUNS])(void);
unsigned long runinterval[NUMRUNS];
unsigned long waketime[NUMRUNS];
long count[NUMRUNS];
bool used[NUMRUNS];

void run(void (*userfunction)(void), unsigned long interval) {
	runDelay(userfunction, interval, 2000000000);
}

void runDelay(void (*userfunction)(void), unsigned long interval, long repeatCount) {
	int i;
	for (i = 0; i < NUMRUNS; ++i) {
		if (!used[i])
			break;
	}
	if (i >= NUMRUNS)
		return;
	
	runfunction[i] = userfunction;
	runinterval[i] = interval;
	waketime[i] = millis() + interval;
	count[i] = repeatCount;
	used[i] = true;
}

void snooze(unsigned long interval) {
	waketime[curtask] = millis() + interval;
}

void removeTask()
{
	used[curtask] = false;
}

void removeTask(void (*userfunction)(void)) {
	for (int i = 0 ; i < NUMRUNS; ++i) {
		if (runfunction[i] == userfunction) 
			used[i] = false;
	}
}

void runner(void) {
	unsigned long m = millis();
	for (curtask=0; curtask<NUMRUNS; curtask++) {
		if (used[curtask] && ((m >= waketime[curtask]))) {
			waketime[curtask] = m + runinterval[curtask];
			--count[curtask];
			(*runfunction[curtask])();
			
			if (count[curtask] <= 0) {
				used[curtask] = false;
			}
		}
	}
}