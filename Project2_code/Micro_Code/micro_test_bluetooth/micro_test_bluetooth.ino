#include <SoftwareSerial.h>
SoftwareSerial Bluetooth(10, 9); // TX, RX (pins in Bluetooth) like TX.Bluetooth -> pin 10 in Micro

int ledpin = 13;
int s_data;

void setup(){
  Bluetooth.begin(9600);
  Serial.begin(9600);
  pinMode(ledpin, OUTPUT);
  
  Serial.println("Waiting for command...");
  //Bluetooth.println("Send 1 to turn on the LED. Send 0 to turn Off");
}

void loop(){ 
  if(Bluetooth.available()){
    s_data = Bluetooth.read(); // reading on tempPin   
    //Bluetooth.println(s_data); Bluetooth.println("\n");
    Serial.print(s_data); 
    //Serial.println("\n");
  }
  else
    Serial.println("No Connection!!"); 
  delay(1000);
}
