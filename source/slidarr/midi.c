/*
 * midi.c
 *
 *  Created on: May 12, 2019
 *      Author: mats
 */
#include "midi.h"

#include <math.h>

void noteOn(int note, int velocity){
    char command = 0x90;
    char parameterKey = (char)note;
    char parameterVelocity = (char)velocity;
    UART4Tx(command);
    UART4Tx(parameterKey);
    UART4Tx(parameterVelocity);
}

void noteOff(int note, int velocity){
    char command = 0x80;
    char parameterKey = (char)note;
    char parameterVelocity = (char)velocity;
    UART4Tx(command);
    UART4Tx(parameterKey);
    UART4Tx(parameterVelocity);
}

void pitchbend(int value){
    value += 8192; // Center value (8192 = no bend)

    char command = 0xE0;
    int lsbValue = value & 0x7F;
    int msbValue = value & 0x3F80;
    msbValue = msbValue >> 7;
    char parameterLsb = (char)lsbValue;
    char parameterMsb = (char)msbValue;
    UART4Tx(command);
    UART4Tx(parameterLsb);
    UART4Tx(parameterMsb);
}

int freqToNote(float freq) {
    return log(freq/440.0)/log(2) * 12 + 69;
}
