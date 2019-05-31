#include "MIDIUSB.h"
#include <SoftwareSerial.h>
#include <stdint.h>

SoftwareSerial Bluetooth(10, 9); // TX, RX (pins in Bluetooth) like TX.Bluetooth -> pin 10 in Micro
uint8_t data;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Bluetooth.begin(115200);

  Serial.println("Serial start!");
  Bluetooth.println("Bluetooth start!");
}

void loop() {
  // put your main code here, to run repeatedlyv:
  while(Bluetooth.available()){
    delay(3);  //delay to allow buffer to fill 
    if (Bluetooth.available() > 0){
      data = Bluetooth.read();

      MidiUSB.write(data, 1);
      MidiUSB.flush(); // Not sure if needed
    }
  }
}

