/*
 * midi.c
 *
 *  Created on: May 12, 2019
 *      Author: mats
 */
#include "midi.h"
#include "uart.h"
#include <math.h>
#include <stdint.h>
#include "defaults.h"

void noteOn(int note, int velocity){
    char command = 0x90 +  + MIDI_CHANNEL - 1;
    char parameterKey = (char)note;
    char parameterVelocity = (char)velocity;

    // Sending 6 bytes in a row and waiting for the transmission to end
    // might block everything if no buffer is used. At a baud rate of 9600 sending 6 bytes will take
    // around 5ms if no buffer used.
    UART4Tx(command);
    UART4Tx(parameterKey);
    UART4Tx(parameterVelocity);

    uint8_t blu_command = 144 + MIDI_CHANNEL - 1;  //144 Decimal same as 0x90 HEX
    uint8_t blu_note = note;
    uint8_t blu_velocity = velocity;    //or 127

    UART5Tx(blu_command);
    UART5Tx(blu_note);
    UART5Tx(blu_velocity);
}

void noteOff(int note, int velocity){
    char command = 0x80 + MIDI_CHANNEL - 1;
    char parameterKey = (char)note;
    char parameterVelocity = (char)velocity;

    UART4Tx(command);
    UART4Tx(parameterKey);
    UART4Tx(parameterVelocity);

    uint8_t blu_command = 128 + MIDI_CHANNEL - 1;  //128 Decimal same as 0x80 HEX
    uint8_t blu_note = note;
    uint8_t blu_velocity = velocity;    //or 127

    UART5Tx(blu_command);
    UART5Tx(blu_note);
    UART5Tx(blu_velocity);
}

void pitchbend(int value){
    value += 8192; // Center value (8192 = no bend)

    if (value > 16383)
        value = 16383;

    else if (value < 0)
        value = 0;

    char command = 0xE0 + MIDI_CHANNEL - 1;
    int lsbValue = value & 0x7F;
    int msbValue = value & 0x3F80;
    msbValue = msbValue >> 7;
    char parameterLsb = (char)lsbValue;
    char parameterMsb = (char)msbValue;

    UART4Tx(command);
    UART4Tx(parameterLsb);
    UART4Tx(parameterMsb);

    uint8_t blu_command = 224 + MIDI_CHANNEL - 1;  //224 Decimal same as 0xE0 HEX
    uint8_t blu_parameterLsb = lsbValue;
    uint8_t blu_parameterMsb = msbValue;

    UART5Tx(blu_command);
    UART5Tx(blu_parameterLsb);
    UART5Tx(blu_parameterMsb);
}

int freqToNote(float freq) {
    return log(freq/440.0)/log(2) * 12 + 69;
}

float noteToFreq(int note) {
  return powf(2, (note - 69)/12.0) * 440.0;
}
