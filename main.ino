#include "ServoManager.h"

#define MAGNET 17

ServoManager sm;

void setup() {
  sm.initialPosition();
  Serial.begin(9600);
  pinMode(MAGNET, OUTPUT);
  digitalWrite(MAGNET,HIGH);
}

void loop() {
  // sm.move_to_position(0,0);
  // delay(500);
 
  // sm.move_to_position(1,0);
  // delay(500);
 
  // sm.move_to_position(2,0);
  // delay(500);

  // sm.move_to_position(0,1);
  // delay(500);
 
  // sm.move_to_position(1,1);
  // delay(500);
 
  // sm.move_to_position(2,1);
  // delay(500);

  sm.initialPosition();
  Serial.println(sm.get_log());
  delay(200000);
}
