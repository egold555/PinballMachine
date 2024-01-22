#ifndef BUTTON__H_
#define BUTTON__H_

#include <Arduino.h>

// Configs
#define ANALOG_THRESHOLD 200
#define DEBOUNCE_DELAY 20

// defines a button on the pinball machine
typedef struct Button
{
    bool sw = false; // Switch
    bool pr = false; // Pressed
    long tm = 0;     // Time

    /**
     * magical debouncing code that works and should never be touched
     *
     * @param pin pin to debounce
     */
    void debounce(int pin)
    {
        bool oldSw = sw;
        bool newSw;
        newSw = analogRead(pin) > ANALOG_THRESHOLD;
        sw = newSw;

        pr = false;
        if (newSw && !oldSw)
        {
            unsigned long currtm = millis();
            if (currtm - tm > DEBOUNCE_DELAY)
            {
                pr = true;
            }
            tm = currtm;
        }
    }

    /**
     * magical debouncing code that works and should never be touched
     *
     * @param pin pin to debounce
     */
    void debounceDigital(int pin)
    {
        bool oldSw = sw;
        bool newSw;
        newSw = digitalRead(pin) == LOW;
        sw = newSw;

        pr = false;
        if (newSw && !oldSw)
        {
            unsigned long currtm = millis();
            if (currtm - tm > DEBOUNCE_DELAY)
            {
                pr = true;
            }
            tm = currtm;
        }
    }

} Button;

#endif // BUTTON__H_