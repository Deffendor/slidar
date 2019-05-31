/*
 * slidarr.c
 *
 *  Created on: May 13, 2019
 *      Author: mats
 */

#include "slidarr.h"

#include <math.h>

float posToFreq(float base_freq, int base_val, int octave_span, int current_val) {
    float octave_frac, freq;

    octave_frac = (current_val - base_val) / (float) octave_span;
    freq = powf(2, octave_frac) * base_freq;

    return freq;
}

int freqToPos(float base_freq, int base_val, int octave_span, float freq) {
    return log(freq/base_freq)/log(2)*octave_span + base_val;
}

void initHistory(int *history, int max_items) {
    int i;
    for (i = 0; i < max_items; i++)
        history[i] = 0;
}

void logVal(int *history, int max_items, int *index, int newVal) {
    // Store value
    history[*index] = newVal;

    // Increment index in the ring buffer
    *index = (*index == max_items-1) ? 0 : *index + 1;
}

float getStdDev(int *arr, int max_items) {
    int i;
    float mean, sum;

    mean = getMean(arr, max_items);
    sum = 0;

    for (i = 0; i < max_items; i++) {
        sum += pow(arr[i] - mean, 2);
    }

    return sqrt(sum/max_items);
}

float getMean(int *arr, int max_items) {
    int i;
    float sum = 0;

    for (i = 0; i < max_items; i++)
        sum += arr[i];

    return sum / max_items;
}
