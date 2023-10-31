#include "ServoManager.h"

ServoManager::ServoManager():
/*Construtor de Servos
Pino de entrada, 
Offset (Compensa os angulos que sobram na montagem do braço),
Vai de 0 a 180 (false) ou de 180 a 0 (true),
Velocidade - Menor é mais rapido (Espera x nanosegundos para incrementar o angulo)
*/
servo1(5, -17, false,15),   
servo2(18, 18, false,10), 
servo3(19, 7, true,10), 
servo4(23, -12, false,10) 
{ 
    l0 = 110; /*Comprimento dos braços em mm*/
    l1 = 139;
    l2 = 162;
    l3 = 71;
    h = -15; /*Altura desejada até o chão*/
    log = 0.1;
    positions[0][0] = Position(-55,85);  /*Inicia as posições do tabuleiro*/
    positions[1][0] = Position(-62,175);
    positions[2][0] = Position(-70,265);
    positions[0][1] = Position(27,60);
    positions[1][1] = Position(25,155);
    positions[2][1] = Position(20,260);
    positions[0][2] = Position(85,38);
    positions[1][2] = Position(108,140);
    positions[2][2] = Position(115,242);
}

ServoManager::~ServoManager() 
{ 
}

/*Define a posição de inicio ou descanso do braço*/
void ServoManager::initialPosition()
{
    servo2.setPosition(0); 
    servo3.setPosition(90);
    servo4.setPosition(30);
    servo1.setPosition(90);
    servo3.setPosition(160);
}

/*Realiza um movimento para as coodenadas retângulares x e y,
sendo o ponto (0,0) o centro da base do braço*/
void ServoManager::movement(int x, int y)
{
    calculate(x,y); /*Chama o método para realizar as contas dos ângulos dos motores*/
    initialPosition(); /*Chama a posição inicial para evitar que o braço realize 
    um movimento prejudicial (Bata em alguma superfície durante o movimento)*/
    delay(2000);
    servo1.setPosition(angle_motor1); /*Movimenta a base para o ângulo polar desejado*/
    servo2.setPosition(-10);
    delay(3000); /*Espera que o movimento tenha sido realizado por completo*/
    servo3.setPosition(angle_motor3); /*Realiza os demais movimentos, na ordem que evita batidas em superfícies*/
    servo4.setPosition(angle_motor4);
    servo2.setPosition(angle_motor2);
    delay(100); /*Espera um tempo para pegar a peça*/
}

/*Função responsável por calcular os ângulos dos motores*/
void ServoManager::calculate(int x, int y)
{
    xret = x; /*Coordenadas retangulares novas são armazenadas*/
    yret = y;
    dpol = sqrt(xret*xret+yret*yret); /*Calcula a distância em coordenadas polares*/
    angle_pol = acos(xret/dpol)*180.f/PI; /*Calcula o ângulo em coordenadas polares*/
    h_triangle = h + l3 - l0; /*Diferença entre a base e a ponta do braço*/
    d = dpol*dpol+h_triangle*h_triangle; /*Distancia entre o final da base e o inicio da ponta do braço ao quadrado*/
    d_root = sqrt(d); /*Distancia entre o final da base e o inicio da ponta do braço*/
    /*Angulos dos triangulos internos formados, calculados a partir da lei dos cossenos*/
    angle0 = (h_triangle>0)?(acos(dpol/d_root)*180.f/PI):((-1.f)*acos(dpol/d_root)*180.f/PI);
    angle1 = acos((l1*l1+d-l2*l2)/(2*l1*d_root))*180.f/PI; 
    angle01 = angle0+angle1+90.f;  /*Ângulo interno do segundo motor (primeiro fora da base)*/
    angle12 = acos((l1*l1+l2*l2-d)/(2*l1*l2))*180.f/PI; /*Ângulo do terceiro motor (cotovelo)*/
    angle23 = 360.f-angle01-angle12; /*Ângulo interno do último motor*/
    /*Salva os ângulos nas variáveis que serão usadas nas movimentações (o complemento em alguns casos)*/
    angle_motor1 = (int)angle_pol;
    angle_motor2 = 180 - (int)angle01;
    angle_motor3 = 180 - (int)angle12;
    angle_motor4 = 180 - (int)angle23;
    log = y; /*Variável que se deseja verificar o valor na saida Serial*/
}

/*Recebe a posição a partir do vetor de posições,
descobre suas coordenadas e chama o metodo para movimentar o braço*/
void ServoManager::move_to_position(int line, int col)
{
    Position p = positions[line][col];
    movement(p.getX(),p.getY());
}

/*Método que retorna o log para ser mostrado na saída Serial*/
double ServoManager::get_log()
{
    return log;
}