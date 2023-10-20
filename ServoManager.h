#ifndef SERVO_MANAGER
#define SERVO_MANAGER

#include <math.h>
#include "ServoMotor.h"

#define PI 3.14156

class ServoManager {
private:
    ServoMotor servo1;
    ServoMotor servo2;
    ServoMotor servo3;
    ServoMotor servo4;
    double l0,l1,l2,l3;
    double xret,yret,dpol,angle_pol,h;
    double h_triangle,d,d_root,angle0,angle1,angle12,angle01,angle23;
    int angle_motor1,angle_motor2,angle_motor3,angle_motor4;
    double log;

public:
    ServoManager();
    ~ServoManager();
    void initialPosition();
    void movement();
    void calculate(int x, int y);
    double get_log();
};

#endif