#ifndef SERVO_MANAGER
#define SERVO_MANAGER

#include <math.h>
#include "ServoMotor.h"
#include "Position.h"

#define PI 3.14156

class ServoManager {
private:
    ServoMotor servo1;
    ServoMotor servo2;
    ServoMotor servo3;
    ServoMotor servo4;
    Position pieces;
    Position positions[3][3]; /*Matriz de posições do tabuleiro e suas coodenadas*/
    /*Declaração das variáveis usadas nas contas*/
    double xret,yret,dpol,angle_pol,h_triangle,d,d_root,angle0,angle1,angle12,angle01,angle23;
    double l0,l1,l2,l3,h;  /*Parâmetros de entrada*/
    int angle_motor1,angle_motor2,angle_motor3,angle_motor4; /*Saídas*/
    double log;

public:
    ServoManager();
    ~ServoManager();
    void move_to_position(int line, int col);
    void move_to_pieces();
    void initialPosition();
    void intermediaryPosition();
    void movement(int x, int y);
    void calculate(int x, int y);
    double get_log();
};

#endif