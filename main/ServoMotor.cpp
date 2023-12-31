#include "ServoMotor.h"

ServoMotor::ServoMotor() 
{ 
}

ServoMotor::~ServoMotor() 
{ 
}

ServoMotor::ServoMotor(int pin, int bangle, bool inv, int s)
{
    base_angle = bangle;
    inverted = inv;
    servo.attach(pin);
    real_angle = -1;
    speed = s;
}

void ServoMotor::setPosition(int a)
{
    int ra = inverted?(180-base_angle-a):(base_angle+a);
    if(real_angle == -1)
        real_angle = ra;
    if(ra >= 0 && ra <= 180)
    {
        move(ra);
    }
    else if(ra < 0)
    {
        ra = 0;
        move(ra);
        a = 180 - base_angle;
    }
    else
    {
        ra = 180;
        move(ra);
        a = 180 - base_angle;
    }
    real_angle = ra;
    servo.write(real_angle);
    angle = a;
}

void ServoMotor::move(int a)
{
    if(speed == 1)
    {
        servo.write(a);
    }
    else
    {
        if(real_angle < a)
        {
            for(int i = real_angle + 1; i<=a; i++)
            {
                servo.write(i);
                delay(speed);
            }
        }
        else
        {
            for(int i = real_angle - 1; i>=a; i--)
            {
                servo.write(i);
                delay(speed);
            }
        }
    }
}