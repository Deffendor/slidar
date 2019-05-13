#include <tm4c123gh6pm.h>
#include <stdint.h>

#include "defaults.h"
#include "midi.h"
#include "utils.h"
#include "adc.h"
#include "buttons.h"
#include "slidarr.h"
#include "uart.h"


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
    int touchdown_val;
    int pitchbend_offset;

    enum state_t state = IDLE;

    initButtons();
    initADC();
    initUART();

    while(1){
        readADC(&current_val);
        current_freq = calcFreq(base_freq, base_val, octave_span, current_val);

        switch (state) {
            case IDLE:
                // String is not touched: Nothing happens.

                if (current_val > TOUCH_THRESHOLD) { // TODO implement better detection of touch (filter current_val and measure over time)

                    // String has been touched: Turn the note on.
                    current_note = freqToNote(current_freq);
                    touchdown_val = current_val; // store current val (for pitchbending)

                    noteOn(current_note, 127);
                    state = SLIDE;
                }
                break;
            case SLIDE:
                // String is being touched: Bend the pitch.

                // Calculate amount of bending
                pitchbend_offset = (touchdown_val - current_val)/(float) octave_span * PITCHBEND_RESOLUTION;

                pitchbend(pitchbend_offset);

                if (current_val < TOUCH_THRESHOLD) {
                    // String has been released: Turn the note off.
                    noteOff(current_note, 127);
                    state = IDLE;
                }

                /* TODO implement calibration
                if (btn1) {
                    // SW1 pressed: Enter calibration mode
                    state = CALIBRATE_BASE;
                }
                */

                /* TODO implement scrolling
                if (btn2) {
                    // SW2 pressed: Scroll the frequency
                    state = SCOLL;
                }
                 */

                break;

            case CALIBRATE_BASE:
                // TODO implement calibration
                /*
                 if (btn1)
                    state = CALIBRATE_OCTAVE;
                */
                break;

            case CALIBRATE_OCTAVE:
                // TODO implement calibration
                break;

            case SCROLL:
                // TODO implement scrolling
                break;
        }

        delayMs(5);
    }
}







