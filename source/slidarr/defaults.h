/*
 * defs.h
 *
 *  Created on: May 12, 2019
 *      Author: mats
 */

#ifndef DEFAULTS_H_
#define DEFAULTS_H_

#define STRING_THRESHOLD 1000 // TODO find a good value
#define STRING_IDLE_STDDEV 50 // TODO find a good value
#define DEFAULT_STRING_OCTAVE_SPAN 500
#define DEFAULT_STRING_BASE 3000
#define DEFAULT_BASE_FREQUENCY 261.63
#define PITCHBEND_MAX 8192*2
#define PITCHBEND_MAX_SEMITONES 2
#define PITCHBEND_INTERVAL 2 // Interval [ms] between pitchbend messages

#define STRING_HISTORY_SIZE 40
#define STRING_SAMPLING_DELAY 5

#define MINIMUM_BASE_FREQUENCY 27.5
#define MAXIMUM_BASE_FREQUENCY 2093.0

#define BAUD_RATE 9600 // options are: 9600, 38400, 115200

#endif /* DEFAULTS_H_ */
