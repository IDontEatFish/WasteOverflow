// Include NewPing Library
#include "NewPing.h"
#include <Wire.h>

// Hook up HC-SR04 with Trig to Arduino Pin 9, Echo to Arduino pin 10
#define TRIGGER_PIN 9
#define ECHO_PIN 10

// Maximum distance we want to ping for (in centimeters).
#define MAX_DISTANCE 400  

// NewPing setup of pins and maximum distance.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);


unsigned int distance = 0;

void setup() {
  Serial.begin(115200);

  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); /* register receive event */
}

void receiveEvent(int howMany) {
 while (0 <Wire.available()) {
    char c = Wire.read();      /* receive byte as a character */
    Serial.print(c);           /* print the character */
  }
 Serial.println();             /* to newline */
}


void requestEvent() {
  Serial.print("got request");
  //Wire.write("123456789012");
  char message[13];
  sprintf(message,"%013d",distance);
  //Serial.write(message);
  //Serial.println();
  Wire.write(message); // respond with message 
  // as expected by master
}

void loop() {
  distance = sonar.ping_cm();
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(500);
}
