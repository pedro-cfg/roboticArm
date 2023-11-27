#ifndef SERVO_MOTOR
#define SERVO_MOTOR

#include <dummy.h>
#include <ESP32Servo.h>
#include <analogWrite.h>
#include <ESP32Tone.h>
#include <ESP32PWM.h>

class ServoMotor {
private:
    Servo servo;
    int base_angle;
    int angle;
    int real_angle;
    int speed;
    bool inverted;
public:
    ServoMotor();
    ServoMotor(int pin, int bangle, bool inv, int s);
    ~ServoMotor();
    void setPosition(int a);
    void move(int a);
};

#endif