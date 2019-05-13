/*
 * slidarr.c
 *
 *  Created on: May 13, 2019
 *      Author: mats
 */

#include "slidarr.h"

#include <math.h>

float calcFreq(float base_freq, int base_val, int octave_span, int current_val) {
    float octave_frac, freq;

    octave_frac = (current_val - base_val) / (float) octave_span;
    freq = base_freq + powf(2, octave_frac) * octave_span;

    return freq;
}
