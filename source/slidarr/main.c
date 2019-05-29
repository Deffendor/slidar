//#define RED_LED    (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 1*4)))   //GPIO_PIN_1
//#define GREEN_LED  (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 3*4)))   //GPIO_PIN_2
//#define BLUE_LED (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 2*4))) //GPIO_PIN_3

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

volatile int btn1, btn2;

/**
 * main.c
 */
int main(void)
{
    int string_base = DEFAULT_STRING_BASE;
    int string_octave_span = DEFAULT_STRING_OCTAVE_SPAN;
    volatile int string_current; // ADC result
    float current_freq;

    int string_history[STRING_HISTORY_SIZE];
    int string_history_index = 0;
    float string_mean;
    float string_stddev;

    float base_freq = DEFAULT_BASE_FREQUENCY;
    float prev_base_freq;
    float touchdown_freq;

    int current_note;
    int note_on = 0;
    float current_note_freq;
    int touchdown_val;
    int note_val;
    int pitchbend_offset;
    int semitone_span = DEFAULT_STRING_OCTAVE_SPAN/12;

    int string_touched, string_untouched = 0, string_moving = 0;

    int time = 0;
    int time_last_pitchbend = 0;

    enum state_t state = IDLE;

    int btnCalibrate = 0, btnScroll = 0;
    btn1 = 0;
    btn2 = 0;
    int new_octave_span;

    initButtons();
    initLEDs();
    initADC();
    initUART();
    initHistory(string_history, STRING_HISTORY_SIZE);
    initSysTickTimer(STRING_SAMPLING_DELAY * 1000); // in microseconds
    initTiTimer();

    while(1){
        readADC(&string_current);

        logVal(string_history, STRING_HISTORY_SIZE, &string_history_index, string_current);
        string_mean = getMean(string_history, STRING_HISTORY_SIZE);
        string_stddev = getStdDev(string_history, STRING_HISTORY_SIZE);

        current_freq = posToFreq(base_freq, string_base, string_octave_span, string_mean);

        //  read to not get stuck
        if(btn1){
            btn1 = readButton(0);
        }

        if(btn2){
            btn2 = readButton(1);
        }

        // Store button states
        btnCalibrate = btn1;
        btnScroll = btn2;

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
                base_freq = prev_base_freq + current_freq - touchdown_freq;

                if (!btnScroll){
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


void GPIO_PORTF_InterruptHandler(void){
    GPIO_PORTF_ICR_R |= 0x11; // clear interrupt
    NVIC_DIS0_R |= 0x40000000; // disable while waiting for debounce
    NVIC_EN0_R |= 0x00080000; // enable timer, somehow disable always disables all
    TIMER0_CTL_R |= 0x01; // start timer
}


void Timer0A_InterruptHandler(void){
    volatile int readback; //dummy variable to write to

    // read buttons here
    btn1 = readButton(0);
    btn2 = readButton(1);

    // reset button interrupts
    NVIC_EN0_R |= 0x40000000; // enable switch interrupts again
    GPIO_PORTF_ICR_R |= 0x11; // clear interrupt
    readback = GPIO_PORTF_ICR_R; // force read to clear

    // reset timer
    TIMER0_ICR_R = 0x1;
    readback = TIMER0_ICR_R;
}






