#include "SPIFFS.h"
#include <BluetoothSerial.h>
#include "AudioFileSourceSPIFFS.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"

#include "ServoManager.h"

#define MAGNET 17
#define GREEN 27
#define RED 14
#define BLUE 12

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
  #error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

AudioGeneratorMP3 *mp3;
AudioFileSourceSPIFFS *file1;
AudioFileSourceSPIFFS *file2;
AudioFileSourceSPIFFS *file3;
AudioFileSourceSPIFFS *file4;
AudioFileSourceSPIFFS *file5;
AudioOutputI2SNoDAC *out;
AudioFileSourceID3 *id1;
AudioFileSourceID3 *id2;
AudioFileSourceID3 *id3;
AudioFileSourceID3 *id4;
AudioFileSourceID3 *id5;

BluetoothSerial SerialBT;
String device_name = "ESP32-BT-Slave";

ServoManager sm; /*Objeto de Gerenciador dos ServoMotores*/

//Não sei
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  (void)cbData;
  Serial.printf("ID3 callback for: %s = '", type);

  if (isUnicode) {
    string += 2;
  }
  
  while (*string) {
    char a = *(string++);
    if (isUnicode) {
      string++;
    }
    Serial.printf("%c", a);
  }
  Serial.printf("'\n");
  Serial.flush();
}

void play_sound(int s)
{
  if (mp3->isRunning()) {
    mp3->stop();
  }

  switch(s)
  {
    case 1:
      mp3->begin(id1, out);
      break;
    case 2:
      mp3->begin(id2, out);
      break;
    case 3:
      mp3->begin(id3, out);
      break;
    case 4:
      mp3->begin(id4, out);
      break;
    case 5:
      mp3->begin(id5, out);
      break;
  }
}

void choose_color(int c)
{
  auto r = LOW, g = LOW, b = LOW;
  switch(c)
  {
    case 1:
      b = HIGH;
      break;
    case 2:
      g = HIGH;
      break;
    case 3:
      r = HIGH;
      break;
    case 4:
      g = HIGH;
      r = HIGH;
      break;
  }
  digitalWrite(RED, r);
  digitalWrite(GREEN, g);
  digitalWrite(BLUE, b);
}

void move_arm(int lin, int col)
{
  //sm.initialPosition();
  digitalWrite(MAGNET,HIGH);
  sm.move_to_pieces();
  delay(1000);

  sm.intermediaryPosition();
  sm.move_to_position(lin,col);
  digitalWrite(MAGNET,LOW);
  delay(1000);

  sm.initialPosition();
}

void first_move()
{
  int lin = random(3);
  int col = random(3);
  Serial.println(lin);
  Serial.println(col);
  move_arm(lin,col);
}

void listen()
{
  char op = '0';
  if(SerialBT.available()) 
  {
    op = SerialBT.read();
  }
  if(op == '1')
  {
    first_move();
  }
  else if(op == '2')
  {
    while(!SerialBT.available()){}
    char lin = SerialBT.read();
    while(!SerialBT.available()){}
    char col = SerialBT.read();
    int l = lin - '0';
    int c = col - '0';
    c = (c-2) * (-1);
    Serial.println(l);
    Serial.println(c);
    move_arm(l,c);
  }
  else if(op == '3')
  {
    while(!SerialBT.available()){}
    char color = SerialBT.read(); 
    int c = color - '0';
    choose_color(c);
  }
  else if(op == '4')
  {
    while(!SerialBT.available()){}
    char sound = SerialBT.read(); 
    int s = sound - '0';
    play_sound(s);
  }
}

void setup() {
  Serial.begin(115200);
  
  SPIFFS.begin();
  audioLogger = &Serial;

  file1 = new AudioFileSourceSPIFFS("/problema.mp3");
  id1 = new AudioFileSourceID3(file1);
  id1->RegisterMetadataCB(MDCallback, (void*)"ID3TAG");

  file2 = new AudioFileSourceSPIFFS("/camera_inicializada.mp3");
  id2 = new AudioFileSourceID3(file2);
  id2->RegisterMetadataCB(MDCallback, (void*)"ID3TAG");

  file3 = new AudioFileSourceSPIFFS("/vermelho_venceu.mp3");
  id3 = new AudioFileSourceID3(file3);
  id3->RegisterMetadataCB(MDCallback, (void*)"ID3TAG");

  file4 = new AudioFileSourceSPIFFS("/verde_venceu.mp3");
  id4 = new AudioFileSourceID3(file4);
  id4->RegisterMetadataCB(MDCallback, (void*)"ID3TAG");

  file5 = new AudioFileSourceSPIFFS("/empate.mp3");
  id5 = new AudioFileSourceID3(file5);
  id5->RegisterMetadataCB(MDCallback, (void*)"ID3TAG");

  out = new AudioOutputI2SNoDAC();
  mp3 = new AudioGeneratorMP3();
  //mp3->begin(id1, out);

  SerialBT.begin(device_name); //Bluetooth device name

  pinMode(MAGNET, OUTPUT);   /*Código do imã (será modificado)*/
  digitalWrite(MAGNET,LOW);

  pinMode(GREEN, OUTPUT);
  digitalWrite(GREEN, LOW);
  pinMode(RED, OUTPUT);
  digitalWrite(RED, LOW);
  pinMode(BLUE, OUTPUT);
  digitalWrite(BLUE, LOW);

  //mp3->begin(id1, out);

  sm.initialPosition(); /*Define a posição inicial do braço*/

  delay(1000);
}

void loop() {
  if (mp3->isRunning()) 
    if (!mp3->loop()) mp3->stop();
  listen();
  //Serial.println("TESTE"); /*Método para debug - o gerenciador retorna a variável a ser printada no Serial*/
}
