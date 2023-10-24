#include "Headers/ServoManager.h"

#define MAGNET 17

ServoManager sm; /*Objeto de Gerenciador dos ServoMotores*/

void setup() {
  sm.initialPosition(); /*Define a posição inicial do braço*/
  Serial.begin(9600);
  pinMode(MAGNET, OUTPUT);   /*Código do imã (será modificado)*/
  digitalWrite(MAGNET,HIGH);
}

void loop() {

  /*Comandos de movimentação do braço*/

  //sm.move_to_position(0,0);
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

  sm.initialPosition(); /*Retorna a posição inicial e espera*/
  Serial.println(sm.get_log()); /*Método para debug - o gerenciador retorna a variável a ser printada no Serial*/
  delay(200000);
}
