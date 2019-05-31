#include "MIDIUSB.h"
#include <SoftwareSerial.h>

SoftwareSerial Bluetooth(10, 9); // TX, RX (pins in Bluetooth) like TX.Bluetooth -> pin 10 in Micro
char data;
String readString, value1, value2, value3, value4;
//uint8_t byte1;
byte n1, n2, n3 , n4; //int! HEX!

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Bluetooth.begin(115200);

  Serial.println("Serial start!");
  Bluetooth.println("Bluetooth start!");
//  n1 = 9;       //noteON
  n2 = 144;     //channel
  n3 = 127;     //pitch
  n4 = 48;      //velocity
}

void loop() {
  // put your main code here, to run repeatedlyv:
  while(Bluetooth.available()){
    delay(3);  //delay to allow buffer to fill 
    if (Bluetooth.available() > 0){
      data = Bluetooth.read();
      readString += data;
    }
  }
  if (readString.length() >0){
      //Serial.println(readString);
      value1 = readString.substring(0, 3);
//      value2 = readString.substring(3, 6);
//      value3 = readString.substring(6, 9);
//      value4 = readString.substring(9, 12);
      n1 = value1.toInt();
//      n2 = value2.toInt();
//      n3 = value3.toInt();
//      n4 = value4.toInt();
      
      Serial.println("Sending note");
      mynote(n1); 
      MidiUSB.flush();
      
      mynote(n2); 
      MidiUSB.flush();
      mynote(n3); 
      MidiUSB.flush();
      mynote(n4); 
      MidiUSB.flush();
      
      /*
      noteOn(n1,n2,n3);
      MidiUSB.flush();
      */
        Serial.print("note1: ");
        Serial.print(n1);
        Serial.print("\t note2: ");
        Serial.print(n2);
        Serial.print("\t note3: ");
        Serial.print(n3);
        Serial.print("\t note4: ");
        Serial.print(n4);
      
      readString="";
      Serial.println("");
      delay(1);
  }
}

//---------------------------------------------------------------------------
void mynote(byte pitch) {
  midiEventPacket_t event = {pitch};
  MidiUSB.sendMIDI(event);
}

/*
 * After change MIDIUSB files to make MidiUSB.sendMIDI read only one byte1! 
void mynote(byte pitch) {
  midiEventPacket_musleh event = {pitch};
  MidiUSB.sendMIDI(event);
}
*/

/*
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
*/
