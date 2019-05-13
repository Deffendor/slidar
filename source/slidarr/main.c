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
    float prev_base_freq;
    float touchdown_freq;

    int current_note;
    int touchdown_val;
    int pitchbend_offset;

    enum state_t state = IDLE;

    int btn1, btn2, btn1_prev, btn2_prev;

    initButtons();
    initADC();
    initUART();

    while(1){
        readADC(&current_val);
        current_freq = calcFreq(base_freq, base_val, octave_span, current_val);

        // Store previous button states
        btn1_prev = btn1;
        btn2_prev = btn2;

        // Read button states
        btn1 = readButton(0); // TODO read and debounce buttons
        btn2 = readButton(1);

        switch (state) {
            case IDLE:
                // String is not touched: Nothing happens.

                if (current_val > TOUCH_THRESHOLD) { // TODO implement better detection of touch (filter current_val and measure over time)

                    // String has been touched: Turn the note on.
                    current_note = freqToNote(current_freq);
                    touchdown_val = current_val; // store current val (for pitchbending)
                    touchdown_freq = current_freq;

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

                if (btn1) {
                    // SW1 pressed: Enter calibration mode
                    noteOff(current_note, 127);

                    state = CALIBRATE_BASE;
                }

                if (btn2) {
                    // SW2 pressed: Scroll the frequency
                    noteOff(current_note, 127);

                    prev_base_freq = base_freq;
                    state = SCROLL;
                }

                break;

            case CALIBRATE_BASE:
                // Set current_val from current reading
                base_val = current_val;

                if (!btn1_prev && btn1)
                    state = CALIBRATE_OCTAVE;

                break;

            case CALIBRATE_OCTAVE:
                // Set octave span from current reading

                octave_span = current_val - base_val;

                if (!btn1_prev && btn1)
                    state = IDLE;

                break;

            case SCROLL:
                // Move the base frequency by sliding and holding button 2
                base_freq = prev_base_freq + current_freq - touchdown_freq;

                if (!btn2)
                    state = IDLE;

                break;
        }

        delayMs(5);
    }
}







