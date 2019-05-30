/*
 * slidarr.h
 *
 *  Created on: May 13, 2019
 *      Author: mats
 */

#ifndef SLIDARR_H_
#define SLIDARR_H_

float posToFreq(float base_freq, int base_val, int octave_span, int current_val);
int freqToPos(float base_freq, int base_val, int octave_span, float freq);

void initHistory(int *history, int max_items);
void logVal(int *history, int max_items, int *index, int newVal);
float getStdDev(int *arr, int max_items);
float getMean(int *arr, int max_items);

#endif /* SLIDARR_H_ */
