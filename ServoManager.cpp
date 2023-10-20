#include "ServoManager.h"

ServoManager::ServoManager():
servo1(5, -7, true), 
servo2(18, 18, false), 
servo3(19, 0, true), 
servo4(23, 5, false) 
{ 
    l0 = 110;
    l1 = 139;
    l2 = 162;
    l3 = 71;
    h = 10;
    log = 0.1;
}

ServoManager::~ServoManager() 
{ 
}

void ServoManager::initialPosition()
{
    servo2.setPosition(0);
    servo3.setPosition(90);
    servo4.setPosition(170);
    servo3.setPosition(150);
    servo1.setPosition(90);
}

void ServoManager::movement()
{
    // servo3.setPosition(50);
    // servo4.setPosition(30);
    // servo2.setPosition(50);
    servo1.setPosition(angle_motor1);
    delay(500);
    servo3.setPosition(angle_motor3);
    servo4.setPosition(angle_motor4);
    servo2.setPosition(angle_motor2);
    delay(1000);
    //initialPosition();
}

void ServoManager::calculate(int x, int y)
{
    xret = x;
    yret = y;
    dpol = sqrt(xret*xret+yret*yret);
    angle_pol = acos(xret/dpol)*180.f/PI;
    h_triangle = h + l3 - l0;
    d = dpol*dpol+h_triangle*h_triangle;
    d_root = sqrt(d);
    angle0 = (h_triangle>0)?(acos(dpol/d_root)*180.f/PI):((-1.f)*acos(dpol/d_root)*180.f/PI);
    angle1 = acos((l1*l1+d-l2*l2)/(2*l1*d_root))*180.f/PI;
    angle01 = angle0+angle1+90.f;
    angle12 = acos((l1*l1+l2*l2-d)/(2*l1*l2))*180.f/PI;
    angle23 = 360.f-angle01-angle12;
    angle_motor1 = (int)angle_pol;
    angle_motor2 = 180 - (int)angle01;
    angle_motor3 = 180 - (int)angle12;
    angle_motor4 = 180 - (int)angle23;
    log = angle_motor1;
}

double ServoManager::get_log()
{
    return log;
}