/*
 * midi.h
 *
 *  Created on: May 12, 2019
 *      Author: mats
 */

#ifndef MIDI_H_
#define MIDI_H_

void noteOn(int note, int velocity);
void noteOff(int note, int velocity);
void pitchbend(int value);

#endif /* MIDI_H_ */
