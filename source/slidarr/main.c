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
    int string_base;
    int string_octave_span;
    volatile int string_current; // ADC result
    float current_freq;

    int string_history[STRING_HISTORY_SIZE];
    int string_history_index;
    float string_mean;
    float string_stddev;

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
    initHistory(string_history, STRING_HISTORY_SIZE);

    while(1){
        readADC(&string_current);

        logVal(string_history, STRING_HISTORY_SIZE, &string_history_index, string_current);
        string_mean = getMean(string_history, STRING_HISTORY_SIZE);
        string_stddev = getStdDev(string_history, STRING_HISTORY_SIZE);

        current_freq = calcFreq(base_freq, string_base, string_octave_span, string_current);

        // Store previous button states
        btn1_prev = btn1;
        btn2_prev = btn2;

        // Read button states
        btn1 = readButton(0); // TODO read and debounce buttons
        btn2 = readButton(1);

        switch (state) {
            case IDLE:
                // String is not touched: Nothing happens.

                if (string_mean > STRING_THRESHOLD && string_stddev < STRING_IDLE_STDDEV) { // TODO implement better detection of touch (filter string_current and measure over time)

                    // String has been touched: Turn the note on.
                    current_note = freqToNote(current_freq);
                    touchdown_val = string_current; // store current val (for pitchbending)
                    touchdown_freq = current_freq;

                    noteOn(current_note, 127);
                    state = SLIDE;
                }
                break;
            case SLIDE:
                // String is being touched: Bend the pitch.

                // Calculate amount of bending
                pitchbend_offset = (touchdown_val - string_mean/(float) string_octave_span * PITCHBEND_RESOLUTION;

                pitchbend(pitchbend_offset);

                if (string_mean < STRING_THRESHOLD && string_stddev < STRING_IDLE_STDDEV) {
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
                // Set string_current from current reading
                string_base = string_mean;

                if (!btn1_prev && btn1)
                    state = CALIBRATE_OCTAVE;

                break;

            case CALIBRATE_OCTAVE:
                // Set octave span from current reading

                string_octave_span = string_mean - string_base;

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

        delayMs(STRING_SAMPLING_DELAY);
    }
}


//Interrupt Status - Interrupt Service Routines (ISRs)
void GPIOPortF_Handler(void)
{
    if (GPIO_PORTF_RIS_R & 0x10) //button 2 F4
    {
        // GPIO_PORTF_MIS_R = 0x10;
        GPIO_PORTF_ICR_R = 0x10;    //edge-detect interrupt cleared by writing a '1'
        //Here! executed code
    }
        if (GPIO_PORTF_RIS_R & 0x01) //button 1 F0
    {
        GPIO_PORTF_ICR_R = 0x01;    //edge-detect interrupt cleared by writing a '1'
        //Here! executed code
    }
}

void GPIOPortD_Handler(void)
{
    if (GPIO_PORTD_RIS_R & 0x04) //button 3 D2
    {
        GPIO_PORTD_ICR_R = 0x04;    //edge-detect interrupt cleared by writing a '1'
        //Here! executed code
    }
    if (GPIO_PORTD_RIS_R & 0x08) //button 4 D3
    {
        GPIO_PORTD_ICR_R = 0x08;    //edge-detect interrupt cleared by writing a '1'
        //Here! executed code
    }
}






