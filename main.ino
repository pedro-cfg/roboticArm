#include "ServoManager.h"

#define MAGNET 17

ServoManager sm;

void setup() {
  sm.initialPosition();
  Serial.begin(9600);
  pinMode(MAGNET, OUTPUT);
}

void loop() {
  sm.initialPosition();
  sm.calculate(0,100);
  digitalWrite(MAGNET,HIGH);
  sm.movement();
  sm.initialPosition();
  sm.calculate(0,250);
  sm.movement();
  digitalWrite(MAGNET,LOW);
  delay(200);
  sm.initialPosition();
  digitalWrite(MAGNET,HIGH);
  sm.movement();
  sm.initialPosition();
  sm.calculate(0,100);
  sm.movement();
  digitalWrite(MAGNET,LOW);
  delay(200);
  Serial.println(sm.get_log());
}
