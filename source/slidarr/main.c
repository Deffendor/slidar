#include <tm4c123gh6pm.h>
#include <stdint.h>

#include "defaults.h"
#include "midi.h"
#include "utils.h"
#include "adc.h"
#include "buttons.h"


enum state_t {
    IDLE,
    SLIDE,
    CALIBRATE_BASE,
    CALIBRATE_OCTAVE,
    SCROLL
};

/**
 * main.c
 */
int main(void)
{
    int base_val;
    int octave_span;
    volatile int current_val; // ADC result
    float current_freq;

    float base_freq = DEFAULT_BASE_FREQUENCY;
    int current_note;

    enum state_t state = IDLE;

    init_buttons();
    init_ADC();
    init_UART();

    while(1){
        read_ADC(&current_val);
        current_freq = calcFreq(base_freq, base_val, octave_span, current_val);

        switch (state) {
            case IDLE:
                // String is not touched: Nothing happens.

                if (current_val > TOUCH_THRESHOLD) {
                    // String has been touched: Turn the note on.
                    noteOn(0x40, 0x40);
                    state = SLIDE;
                }
                break;
            case SLIDE:
                // String is being touched: Bend the pitch.
                if (current_val < TOUCH_THRESHOLD) {
                    // String has been released: Turn the note off.
                    noteOff(0x40, 0x40);
                    state = IDLE;
                }

                /*
                if (btn1) {
                    // SW1 pressed: Enter calibration mode
                    state = CALIBRATE_BASE;
                }
                */

                /*
                if (btn2) {
                    // SW2 pressed: Scroll the frequency
                    state = SCOLL;
                }
                 */

                break;

            case CALIBRATE_BASE:
                /*
                 if (btn1)
                    state = CALIBRATE_OCTAVE;
                */
                break;

            case CALIBRATE_OCTAVE:
                break;

            case SCROLL:
                break;
        }

        /*noteOn(0x40, 0x40);
        delayMs(1000);

        noteOff(0x40, 0x40);
        delayMs(1000);*/
    }
}







