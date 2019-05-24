/*
 * midi.c
 *
 *  Created on: May 12, 2019
 *      Author: mats
 */
#include "midi.h"
#include "uart.h"
#include <math.h>

void noteOn(int note, int velocity){
    char command = 0x90;
    char parameterKey = (char)note;
    char parameterVelocity = (char)velocity;

    //ToDo: sending 6 bytes in a row and waiting for the transmission to end
    // might block everything. At a baud rate of 9600 sending 6 bytes will take
    // around 5ms
    UART4Tx(command);
    UART4Tx(parameterKey);
    UART4Tx(parameterVelocity);

    UART5Tx(command);
    UART5Tx(parameterKey);
    UART5Tx(parameterVelocity);
}

void noteOff(int note, int velocity){
    char command = 0x80;
    char parameterKey = (char)note;
    char parameterVelocity = (char)velocity;

    UART4Tx(command);
    UART4Tx(parameterKey);
    UART4Tx(parameterVelocity);

    UART5Tx(command);
    UART5Tx(parameterKey);
    UART5Tx(parameterVelocity);
}

void pitchbend(int value){
    value += 8192; // Center value (8192 = no bend)

    if (value > 16383)
        value = 16383;

    else if (value < 0)
        value = 0;

    char command = 0xE0;
    int lsbValue = value & 0x7F;
    int msbValue = value & 0x3F80;
    msbValue = msbValue >> 7;
    char parameterLsb = (char)lsbValue;
    char parameterMsb = (char)msbValue;

    UART4Tx(command);
    UART4Tx(parameterLsb);
    UART4Tx(parameterMsb);

    UART5Tx(command);
    UART5Tx(parameterLsb);
    UART5Tx(parameterMsb);
}

int freqToNote(float freq) {
    return log(freq/440.0)/log(2) * 12 + 69;
}

int noteToFreq(int note) {
  return powf(2, (note - 69)/12.0) * 440.0;
}
