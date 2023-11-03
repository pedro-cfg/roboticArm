#include "ServoManager.h"

#define MAGNET 17

ServoManager sm; /*Objeto de Gerenciador dos ServoMotores*/

void setup() {
  sm.initialPosition(); /*Define a posição inicial do braço*/
  Serial.begin(9600);
  pinMode(MAGNET, OUTPUT);   /*Código do imã (será modificado)*/
  digitalWrite(MAGNET,LOW);
}

void loop() {

  /*Comandos de movimentação do braço*/
 
  sm.initialPosition();
  digitalWrite(MAGNET,HIGH);
  sm.move_to_pieces();
  delay(1000);

  sm.intermediaryPosition();
  sm.move_to_position(1,2);
  digitalWrite(MAGNET,LOW);
  delay(1000);

  sm.initialPosition();
  digitalWrite(MAGNET,HIGH);
  sm.move_to_pieces();
  delay(1000);

  sm.intermediaryPosition();
  sm.move_to_position(1,1);
  digitalWrite(MAGNET,LOW);
  delay(1000);

  sm.initialPosition();
  digitalWrite(MAGNET,HIGH);
  sm.move_to_pieces();
  delay(1000);

  sm.intermediaryPosition();
  sm.move_to_position(1,0);
  digitalWrite(MAGNET,LOW);
  delay(1000);


  sm.initialPosition(); /*Retorna a posição inicial e espera*/
  Serial.println(sm.get_log()); /*Método para debug - o gerenciador retorna a variável a ser printada no Serial*/
  delay(200000);
}
