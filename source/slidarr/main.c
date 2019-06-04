/*
 * main.c
 *
 * Slidarr
 * 
 * Programming Embedded Systems, Project
 * Uppsala University
 * Spring semester 2019
 * 
 * by Mohammand E. M., Mats J., Sören M.
 */


#include <tm4c123gh6pm.h>
#include <stdint.h>

#include "defaults.h"
#include "midi.h"
#include "utils.h"
#include "adc.h"
#include "buttons.h"
#include "leds.h"
#include "slidarr.h"
#include "uart.h"
#include "sttimer.h"
#include "titimer.h"


enum state_t {
    IDLE,
    SLIDE,
    CALIBRATE,
    SCROLL
};

volatile int btn1;// btn1 is SW2 which is connected to PF0
volatile int btn2;// btn2 is SW1 which is connected to PF4

/**
 * main.c
 */
int main(void)
{
    int string_base = DEFAULT_STRING_BASE;                  // raw adc start value
    int string_octave_span = DEFAULT_STRING_OCTAVE_SPAN;    // adc range to play on, from stirng_base to (string_base + string_octave_span)
    volatile int string_current;                            // ADC result
    float current_freq;                                     // corresponding keyboard frequency to string_current

    int string_history[STRING_HISTORY_SIZE];                // buffer array to smoothen value
    int string_history_index = 0;                           // buffer index (ring buffer)
    float string_mean;                                      // average is used as filtered value to create notes from
    float string_stddev;                                    // used to detect if currently sliding

    float base_freq = DEFAULT_BASE_FREQUENCY;               // reference frequency setting the location on the "keyboard"
    float prev_base_freq;                                   // used for scrolling, changing the "keyboard" range
    float touchdown_freq;                                   // location where the initial touch was made
    float scroll_base_freq;                                 // used for scrolling

    int current_note;                                       // midi note converted from the frequency
    int note_on = 0;                                        // flag that tracks the state of the note
    float current_note_freq;                                // calculated frequency of the current_note
    int touchdown_val;                                      // current value for pitchbending
    int note_val;                                           //
    int pitchbend_offset;                                   // pitchbend value to send via midi
    int semitone_span = DEFAULT_STRING_OCTAVE_SPAN/12;      // 

    int string_touched, string_moving = 0; //string_untouched = 0,  // flags for the current situation on the string

    int time = 0;                                           // time generated from systick
    int time_last_pitchbend = 0;                            // keep track of the pitchbending

    enum state_t state = IDLE;                              // state

    int btnCalibrate = 0, btnScroll = 0;                    // button states
    int btnCalibrateDebounceTime = 0, btnScrollDebounceTime = 0;
    btn1 = 0;
    btn2 = 0;
    int new_octave_span;                                    // helper variable for calibration

    initUART(BAUD_RATE);
    initButtons();
    initLEDs();
    initADC();
    initHistory(string_history, STRING_HISTORY_SIZE);
    initSysTickTimer(STRING_SAMPLING_DELAY * 1000); // in microseconds

    while(1){
        readADC(&string_current);

        logVal(string_history, STRING_HISTORY_SIZE, &string_history_index, string_current);
        string_mean = getMean(string_history, STRING_HISTORY_SIZE);
        string_stddev = getStdDev(string_history, STRING_HISTORY_SIZE);

        current_freq = posToFreq(base_freq, string_base, string_octave_span, string_mean);

        btn1 = readButton(0);
        btn2 = readButton(1);

        // Debounce without interrupt
        if(btnCalibrateDebounceTime > 0){
            btnCalibrateDebounceTime -= STRING_SAMPLING_DELAY;
            if(btnCalibrateDebounceTime <= 0){
                btnCalibrate = btn1;
            }
        } else {
            if(btn1 && !btnCalibrate){
                btnCalibrateDebounceTime = 50;
            } else {
                btnCalibrate = btn1;
            }
        }

        // Debounce without interrupt
        if(btnScrollDebounceTime > 0){
            btnScrollDebounceTime -= STRING_SAMPLING_DELAY;
            if(btnScrollDebounceTime <= 0){
                btnScroll = btn2;
            }
        } else {
            if(btn2 && !btnScroll){
                btnScrollDebounceTime = 50;
            } else {
                btnScroll = btn2;
            }
        }

        // Boolean to check if string is being touched
        string_touched = string_mean > STRING_THRESHOLD;
        string_moving = string_stddev > STRING_IDLE_STDDEV;

        //UART4Tx((char) string_current >> 4);

        switch (state) {
            case IDLE:
                // String is not touched: Nothing happens.

                if (string_touched && !string_moving) { // TODO implement better detection of touch (filter string_current and measure over time)

                    // Calculate active note
                    current_note = freqToNote(current_freq);
                    current_note_freq = noteToFreq(current_note);
                    note_val = freqToPos(base_freq, string_base, string_octave_span, current_note_freq);

                    touchdown_val = note_val; // store current val (for pitchbending)
                    touchdown_freq = current_freq;

                    state = SLIDE;
                    setLED(1, 1);
                }

                break;
            case SLIDE:
                // String is being touched: Bend the pitch.

                // Calculate amount of bending
                pitchbend_offset = (string_mean - touchdown_val)
                  /(float) (semitone_span*PITCHBEND_MAX_SEMITONES)
                  *PITCHBEND_MAX/2;

                if (!string_touched
                  || pitchbend_offset > PITCHBEND_MAX/2
                  ||  pitchbend_offset < -PITCHBEND_MAX/2) {

                    // String has been released, or pitchbend reached maximum: 
                    // Turn the note off.
                    noteOff(current_note, 127);
                    note_on = 0;
                    
                    // Reset pitchbend
                    pitchbend(0);

                    setLED(1, 0);
                    state = IDLE;
                } else if (time > time_last_pitchbend + PITCHBEND_INTERVAL && !string_moving) {
                    // Send pitchbend message with specified interval

                    pitchbend(pitchbend_offset);
                    time_last_pitchbend = time;

                    // Turn on note _after_ pitchbend has been set
                    if (!note_on) {
                      noteOn(current_note, 127);
                      note_on = 1;
                    }
                }
                

                if (btnScroll) {
                    // SW2 pressed: Scroll the frequency
                    
                    prev_base_freq = base_freq;
                    scroll_base_freq = base_freq;
                    setLED(2,1);
                    state = SCROLL;
                }

                break;

            case CALIBRATE:
                // Set string_current from current reading
                if (string_mean < string_base)
                    string_base = string_mean;

                // Set octave span from current reading
                new_octave_span = string_mean - string_base;

                if (new_octave_span > string_octave_span)
                    string_octave_span = new_octave_span;

                if (!string_touched || string_moving) {
                    semitone_span = string_octave_span/12;
                    setLED(0, 0);
                    state = IDLE;
                }

                break;

            case SCROLL:
                // Move the base frequency by sliding and holding button 2
                scroll_base_freq = prev_base_freq + current_freq - touchdown_freq;

                // Check if minimum frequency is breached
                if(scroll_base_freq < MINIMUM_BASE_FREQUENCY){
                    scroll_base_freq = MINIMUM_BASE_FREQUENCY;
                }

                // Check if maximum frequency is breached
                if(scroll_base_freq > MAXIMUM_BASE_FREQUENCY){
                    scroll_base_freq = MAXIMUM_BASE_FREQUENCY;
                }

                if (!btnScroll){
                    base_freq = scroll_base_freq;
                    setLED(2,0);
                    state = SLIDE;
                }

                break;
        }


        if (btnCalibrate && state != CALIBRATE) {
            // SW1 pressed: Enter calibration mode
            if (note_on) {
              noteOff(current_note, 127);
              note_on = 0;
              //btn1 = 0;
            }

            // Reset values
            string_base = 10000;
            string_octave_span = 0;

            setLED(0, 1);

            state = CALIBRATE;
        }

        waitForSystick();
        time += STRING_SAMPLING_DELAY;
    }
}

