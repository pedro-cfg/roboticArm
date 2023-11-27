#include <BluetoothSerial.h>
#include <string.h>


#define CAMERA_MODEL_AI_THINKER 
#include "camera_pins.h"
#include "miniMax.h"
#include "esp_camera.h"

#define USE_NAME // Comment this to use MAC address instead of a slaveName

#define TH 12
#define GVALUE 15
#define RVALUE 30

// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
  #error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

bool connected;
String slaveName = "ESP32-BT-Slave"; // Change this to reflect the real name of your slave BT device
String myName = "ESP32-BT-Master";

BluetoothSerial SerialBT;

miniMax mm = NULL;
Move bestMove;
bool player_eh_X = false;
bool myTurn = false;
int estado = 0;
int som = 0;
int luz = 0;
int resultado = 0;
bool jogada;
int vezes1 = 0;
int vezes2 = 0;
char parcial[3][3] = {{'_','_','_'},{'_','_','_'},{'_','_','_'}};
char vetor[3][3] = {{'_','_','_'},{'_','_','_'},{'_','_','_'}};
char verde[3][3] = {{'_','_','_'},{'_','_','_'},{'_','_','_'}};
char vermelho[3][3] = {{'_','_','_'},{'_','_','_'},{'_','_','_'}};
const int tamanho = 9216 * 3;
uint8_t imagem_original[tamanho];

void getImagem() {

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Não conseguiu iniciar camera");
    return;
  }

  int k = 0;
  for(int i = 0; i < fb->len; i+=2)
  {
    
    uint16_t imagem_565 = (fb->buf[i] << 8) | fb->buf[i+1];
    uint8_t r = ((imagem_565 >> 11) & 0x1F) * 8;
    uint8_t g = ((imagem_565 >> 5) & 0x3F) * 4;
    uint8_t b = (imagem_565 & 0x1F) * 8;

    imagem_original[k] = r;
    imagem_original[k+1] = g;
    imagem_original[k+2] = b;

    k += 3;
  }

  esp_camera_fb_return(fb);
}

void confirma_mudanca()
{
  jogada = false;
  int mudancas = 0;
  char m = '_';
  for(int j = 0; j < 3; j++)
  {
    for(int i = 0; i < 3; i++)
    {
      if(vetor[i][j] != parcial[i][j])
      {
        mudancas++;
        m = parcial[i][j];
        vetor[i][j] = m;
      }    
    }
  }
 
  if(m == 'o')
  {
    if(!player_eh_X)
    {
      myTurn = true;
    }
    luz = 3;
    envia_comando(3);
  } 
  else if(m == 'x')
  {
    if(player_eh_X)
    {
      myTurn = true;
    }
    luz = 2;
    envia_comando(3);
  } 
  
}

void calibracao()
{
  getImagem();
  delay(200);
  media_verde();
  verde_em_casas();
  media_vermelha();
  vermelho_em_casas();

  char b[3][3];

  for(int j = 0; j < 3; j++)
  {
    for(int i = 0; i < 3; i++)
    {
      if(vermelho[i][j] != '_')
        b[i][j] = 'x';
      else if (verde[i][j] != '_')
        b[i][j] = 'o';
      else
        b[i][j] = '_';
    }
  }

  Serial.print("|");
  Serial.print(b[0][0]);
  Serial.print("|");
  Serial.print(b[0][1]);
  Serial.print("|");
  Serial.print(b[0][2]);
  Serial.print("|");
  Serial.println();
  Serial.print("|");
  Serial.print(b[1][0]);
  Serial.print("|");
  Serial.print(b[1][1]);
  Serial.print("|");
  Serial.print(b[1][2]);
  Serial.print("|");
  Serial.println();
  Serial.print("|");
  Serial.print(b[2][0]);
  Serial.print("|");
  Serial.print(b[2][1]);
  Serial.print("|");
  Serial.print(b[2][2]);
  Serial.print("|");
  Serial.println();
  
}

void preenche_vetor()
{
  char b[3][3];
  bool mudou = false;

  for(int j = 0; j < 3; j++)
  {
    for(int i = 0; i < 3; i++)
    {
      if(vermelho[i][j] != '_')
        b[i][j] = 'x';
      else if (verde[i][j] != '_')
        b[i][j] = 'o';
      else
        b[i][j] = '_';
    }
  }

  for(int j = 0; j < 3; j++)
  {
    for(int i = 0; i < 3; i++)
    {
      if(b[i][j] != parcial[i][j])
        mudou = true;
    }
  }

  if(mudou)
  {
    if(jogada)
      jogada = false;
    vezes2 = 0;
    vezes1++;
    if(vezes1 > 4)
    {
      jogada = true;
      for(int j = 0; j < 3; j++)
      {
        for(int i = 0; i < 3; i++)
        {
          parcial[i][j] = b[i][j];
        }
      }
    }
  }
  else
  {
    vezes1 = 0;
    vezes2++;
    if(vezes2 > 5 && jogada)
    {
      confirma_mudanca();
    }
  }
}

void envia_comando(int op)
{
  char oper = op + '0';
  SerialBT.write(oper);
  if(op == 2)
  {
    char row = bestMove.row + '0';
    char col = bestMove.col + '0';
    SerialBT.write(row);
    SerialBT.write(col);
  }
  if(op == 3)
  {
    char c = luz + '0';
    SerialBT.write(c);
  }
  if(op == 4)
  {
    char s = som + '0';
    SerialBT.write(s);
  }
}

void printa_vetor()
{
  Serial.print("|");
  Serial.print(parcial[0][0]);
  Serial.print("|");
  Serial.print(parcial[0][1]);
  Serial.print("|");
  Serial.print(parcial[0][2]);
  Serial.print("|");
  Serial.println();
  Serial.print("|");
  Serial.print(parcial[1][0]);
  Serial.print("|");
  Serial.print(parcial[1][1]);
  Serial.print("|");
  Serial.print(parcial[1][2]);
  Serial.print("|");
  Serial.println();
  Serial.print("|");
  Serial.print(parcial[2][0]);
  Serial.print("|");
  Serial.print(parcial[2][1]);
  Serial.print("|");
  Serial.print(parcial[2][2]);
  Serial.print("|");
  Serial.println();

  Serial.print("|");
  Serial.print(vetor[0][0]);
  Serial.print("|");
  Serial.print(vetor[0][1]);
  Serial.print("|");
  Serial.print(vetor[0][2]);
  Serial.print("|");
  Serial.println();
  Serial.print("|");
  Serial.print(vetor[1][0]);
  Serial.print("|");
  Serial.print(vetor[1][1]);
  Serial.print("|");
  Serial.print(vetor[1][2]);
  Serial.print("|");
  Serial.println();
  Serial.print("|");
  Serial.print(vetor[2][0]);
  Serial.print("|");
  Serial.print(vetor[2][1]);
  Serial.print("|");
  Serial.print(vetor[2][2]);
  Serial.print("|");
  Serial.println();
}

void checa_inicio()
{
  int verde = 0;
  int vermelho = 0;
  for(int i = 0; i < tamanho; i+=3)
  {
    if(imagem_original[i+1] > imagem_original[i] + 15 && imagem_original[i+1] > imagem_original[i+2] + 15)
    {
          verde++;
    }
    if(imagem_original[i] > imagem_original[i+1] + 35 && imagem_original[i] > imagem_original[i+2] + 35)
    {
          vermelho++;
    }
  }
  if (verde > 500)
  {
    vezes1++;
    if(vezes1 > 10)
    {
      player_eh_X = false;
      mm = miniMax(true);
      estado = 1;
      luz = 3;
      envia_comando(3);
      envia_comando(1);
      vezes1 = 0;
      // Serial.print("O BRACO EH ");
      // Serial.println(mm.player);
      // Serial.print("O OUTRO EH ");
      // Serial.println(mm.opponent);
      // Serial.println();
      delay(10000);
    }
  }
  else if(vermelho > 30)
  {
    vezes1++;
    if(vezes1 > 20)
    {
      player_eh_X = true;
      mm = miniMax(false);
      estado = 1;
      vezes1 = 0;
      luz = 2;
      envia_comando(3);
    }
  }
  else
    vezes1 = 0;
  Serial.print("VERDE = ");
  Serial.print(verde);
  Serial.print(" VERMELHO = ");
  Serial.print(vermelho);
  Serial.println();
}

void verde_em_casas()
{
  int x_inicial, x_final, y_inicial, y_final = 0;
  int total = 0, pos = 0;
  int r = 0,g = 0;
  int pos0 = 0;
  int pos1 = 0;
  int pos2 = 0;
  int pos3 = 0;
  int pos4 = 0;
  int pos5 = 0;
  int pos6 = 0;
  int pos7 = 0;
  int pos8 = 0;

  //POS0
  x_inicial = 25;
  x_final = 35;
  y_inicial = 5;
  y_final = 15;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos+1] > imagem_original[pos] + GVALUE && imagem_original[pos+1] > imagem_original[pos+2] + GVALUE)
      {
        pos0++;
      }
    }
  }
  pos0 = ((float)pos0/(float)total) * 100;
  if(pos0 > TH)
    verde[0][0] = 'o';
  else
    verde[0][0] = '_';

  //POS1
  x_inicial = 35;
  x_final = 45;
  y_inicial = 25;
  y_final = 32;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos+1] > imagem_original[pos] + GVALUE && imagem_original[pos+1] > imagem_original[pos+2] + GVALUE)
      {
        pos1++;
      }
    }
  }
  pos1 = ((float)pos1/(float)total) * 100;
  if(pos1 > TH)
    verde[0][1] = 'o';
  else
    verde[0][1] = '_';

  //POS2
  x_inicial = 45;
  x_final = 55;
  y_inicial = 40;
  y_final = 58;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos+1] > imagem_original[pos] + GVALUE && imagem_original[pos+1] > imagem_original[pos+2] + GVALUE)
      {
        pos2++;
      }
    }
  }
  pos2 = ((float)pos2/(float)total) * 100;
  if(pos2 > TH)
    verde[0][2] = 'o';
  else
    verde[0][2] = '_';

  //POS3
  x_inicial = 7;
  x_final = 27;
  y_inicial = 10;
  y_final = 30;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos+1] > imagem_original[pos] + GVALUE && imagem_original[pos+1] > imagem_original[pos+2] + GVALUE)
      {
        pos3++;
      }
    }
  }
  pos3 = ((float)pos3/(float)total) * 100;
  if(pos3 > TH)
    verde[1][0] = 'o';
  else
    verde[1][0] = '_';

  //POS4
  x_inicial = 23;
  x_final = 43;
  y_inicial = 40;
  y_final = 52;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos+1] > imagem_original[pos] + GVALUE && imagem_original[pos+1] > imagem_original[pos+2] + GVALUE)
      {
        pos4++;
      }
    }
  }
  pos4 = ((float)pos4/(float)total) * 100;
  if(pos4 > TH)
    verde[1][1] = 'o';
  else
    verde[1][1] = '_';

  //POS5
  x_inicial = 40;
  x_final = 50;
  y_inicial = 60;
  y_final = 70;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos+1] > imagem_original[pos] + GVALUE && imagem_original[pos+1] > imagem_original[pos+2] + GVALUE)
      {
        pos5++;
      }
    }
  }
  pos5 = ((float)pos5/(float)total) * 100;
  if(pos5 > TH)
    verde[1][2] = 'o';
  else
    verde[1][2] = '_';

  //POS6
  x_inicial = 0;
  x_final = 15;
  y_inicial = 42;
  y_final = 55;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos+1] > imagem_original[pos] + GVALUE && imagem_original[pos+1] > imagem_original[pos+2] + GVALUE)
      {
        pos6++;
      }
    }
  }
  pos6 = ((float)pos6/(float)total) * 100;
  if(pos6 > TH)
    verde[2][0] = 'o';
  else
    verde[2][0] = '_';

  //POS7
  x_inicial = 14;
  x_final = 34;
  y_inicial = 66;
  y_final = 86;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos+1] > imagem_original[pos] + GVALUE && imagem_original[pos+1] > imagem_original[pos+2] + GVALUE)
      {
        pos7++;
      }
    }
  }
  pos7 = ((float)pos7/(float)total) * 100;
  if(pos7 > TH)
    verde[2][1] = 'o';
  else
    verde[2][1] = '_';

  //POS8
  x_inicial = 34;
  x_final = 44;
  y_inicial = 75;
  y_final = 90;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos+1] > imagem_original[pos] + GVALUE && imagem_original[pos+1] > imagem_original[pos+2] + GVALUE)
      {
        pos8++;
      }
    }
  }
  pos8 = ((float)pos8/(float)total) * 100;
  if(pos8 > TH)
    verde[2][2] = 'o';
  else
    verde[2][2] = '_';

  Serial.print("POS0 = ");
  Serial.print(pos0);
  Serial.print(" POS1 = ");
  Serial.print(pos1);
  Serial.print(" POS2 = ");
  Serial.print(pos2);
  Serial.print(" POS3 = ");
  Serial.print(pos3);
  Serial.print(" POS4 = ");
  Serial.print(pos4);
  Serial.print(" POS5 = ");
  Serial.print(pos5);
  Serial.print(" POS6 = ");
  Serial.print(pos6);
  Serial.print(" POS7 = ");
  Serial.print(pos7);
  Serial.print(" POS8 = ");
  Serial.print(pos8);
  
  Serial.println();
}

void media_verde()
{
  int x = 0, y = 0;
  int total = 0;
  int x_total = 0, y_total = 0;
  for(int i = 0; i < tamanho; i+=3)
  {
    if(imagem_original[i+1] > imagem_original[i] + 15 && imagem_original[i+1] > imagem_original[i+2] + 15)
    {
          x_total = x_total + (i % 288)/3;
          y_total = y_total + (i / 288);
          total++;
    }
  }
  if(total > 0)
  {
    x = x_total/total;
    y = y_total/total;
  }
  Serial.print("X = ");
  Serial.print(x);
  Serial.print(" Y = ");
  Serial.print(y);
  Serial.print(" TOTAL = ");
  Serial.print(total);
  Serial.println();
}

void vermelho_em_casas()
{
  int x_inicial, x_final, y_inicial, y_final = 0;
  int total = 0, pos = 0;
  int r = 0,g = 0;
  int pos0 = 0;
  int pos1 = 0;
  int pos2 = 0;
  int pos3 = 0;
  int pos4 = 0;
  int pos5 = 0;
  int pos6 = 0;
  int pos7 = 0;
  int pos8 = 0;

  //POS0
  x_inicial = 25;
  x_final = 35;
  y_inicial = 5;
  y_final = 15;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos] > imagem_original[pos+1] + RVALUE && imagem_original[pos] > imagem_original[pos+2] + RVALUE)
      {
        pos0++;
      }
    }
  }
  pos0 = ((float)pos0/(float)total) * 100;
  if(pos0 > TH)
    vermelho[0][0] = 'x';
  else
    vermelho[0][0] = '_';

  //POS1
  x_inicial = 35;
  x_final = 45;
  y_inicial = 25;
  y_final = 32;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos] > imagem_original[pos+1] + RVALUE && imagem_original[pos] > imagem_original[pos+2] + RVALUE)
      {
        pos1++;
      }
    }
  }
  pos1 = ((float)pos1/(float)total) * 100;
  if(pos1 > TH)
    vermelho[0][1] = 'x';
  else
    vermelho[0][1] = '_';


  //POS2
  x_inicial = 45;
  x_final = 55;
  y_inicial = 40;
  y_final = 58;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos] > imagem_original[pos+1] + RVALUE && imagem_original[pos] > imagem_original[pos+2] + RVALUE)
      {
        pos2++;
      }
    }
  }
  pos2 = ((float)pos2/(float)total) * 100;
  if(pos2 > TH)
    vermelho[0][2] = 'x';
  else
    vermelho[0][2] = '_';


  //POS3
  x_inicial = 7;
  x_final = 27;
  y_inicial = 10;
  y_final = 30;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos] > imagem_original[pos+1] + RVALUE && imagem_original[pos] > imagem_original[pos+2] + RVALUE)
      {
        pos3++;
      }
    }
  }
  pos3 = ((float)pos3/(float)total) * 100;
  if(pos3 > TH)
    vermelho[1][0] = 'x';
  else
    vermelho[1][0] = '_';


  //POS4
  x_inicial = 23;
  x_final = 43;
  y_inicial = 40;
  y_final = 52;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos] > imagem_original[pos+1] + RVALUE && imagem_original[pos] > imagem_original[pos+2] + RVALUE)
      {
        pos4++;
      }
    }
  }
  pos4 = ((float)pos4/(float)total) * 100;
  if(pos4 > TH)
    vermelho[1][1] = 'x';
  else
    vermelho[1][1] = '_';


  //POS5
  x_inicial = 40;
  x_final = 50;
  y_inicial = 60;
  y_final = 70;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos] > imagem_original[pos+1] + RVALUE && imagem_original[pos] > imagem_original[pos+2] + RVALUE)
      {
        pos5++;
      }
    }
  }
  pos5 = ((float)pos5/(float)total) * 100;
  if(pos5 > TH)
    vermelho[1][2] = 'x';
  else
    vermelho[1][2] = '_';


  //POS6
  x_inicial = 0;
  x_final = 15;
  y_inicial = 42;
  y_final = 55;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos] > imagem_original[pos+1] + RVALUE && imagem_original[pos] > imagem_original[pos+2] + RVALUE)
      {
        pos6++;
      }
    }
  }
  pos6 = ((float)pos6/(float)total) * 100;
  if(pos6 > TH)
    vermelho[2][0] = 'x';
  else
    vermelho[2][0] = '_';


  //POS7
  x_inicial = 14;
  x_final = 34;
  y_inicial = 66;
  y_final = 86;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos] > imagem_original[pos+1] + RVALUE && imagem_original[pos] > imagem_original[pos+2] + RVALUE)
      {
        pos7++;
      }
    }
  }
  pos7 = ((float)pos7/(float)total) * 100;
  if(pos7 > TH)
    vermelho[2][1] = 'x';
  else
    vermelho[2][1] = '_';


  //POS8
  x_inicial = 34;
  x_final = 44;
  y_inicial = 75;
  y_final = 90;
  pos = 0;
  total = 0;
  for(int j = y_inicial;j < y_final; j++)
  {
    for(int i = x_inicial; i < x_final; i++)
    {
      pos = j*288+i*3;
      total++;
      if(imagem_original[pos] > imagem_original[pos+1] + RVALUE && imagem_original[pos] > imagem_original[pos+2] + RVALUE)
      {
        pos8++;
      }
    }
  }
  pos8 = ((float)pos8/(float)total) * 100;
  if(pos8 > TH)
    vermelho[2][2] = 'x';
  else
    vermelho[2][2] = '_';


  Serial.print("POS0 = ");
  Serial.print(pos0);
  Serial.print(" POS1 = ");
  Serial.print(pos1);
  Serial.print(" POS2 = ");
  Serial.print(pos2);
  Serial.print(" POS3 = ");
  Serial.print(pos3);
  Serial.print(" POS4 = ");
  Serial.print(pos4);
  Serial.print(" POS5 = ");
  Serial.print(pos5);
  Serial.print(" POS6 = ");
  Serial.print(pos6);
  Serial.print(" POS7 = ");
  Serial.print(pos7);
  Serial.print(" POS8 = ");
  Serial.print(pos8);
  
  Serial.println();
}

void media_vermelha()
{
  int x = 0, y = 0;
  int total = 0;
  int x_total = 0, y_total = 0;
  for(int i = 0; i < tamanho; i+=3)
  {
    if(imagem_original[i] > imagem_original[i+1] + 40 && imagem_original[i] > imagem_original[i+2] + 40)
    {
          x_total = x_total + (i % 288)/3;
          y_total = y_total + (i / 288);
          total++;
    }
  }
  if(total > 0)
  {
    x = x_total/total;
    y = y_total/total;
  }
  Serial.print("X = ");
  Serial.print(x);
  Serial.print(" Y = ");
  Serial.print(y);
  Serial.print(" TOTAL = ");
  Serial.print(total);
  Serial.println();
}

void btConnect()
{
  SerialBT.connect();
  if(connected) {
    Serial.println("Reconnected Successfully!");
  } 
  else {
    Serial.println("Failed to reconnect. Make sure remote device is available and in range, then restart app.");
  }
}

void estado_final()
{

}

void estado_jogando()
{
  getImagem();
  delay(200);
  media_verde();
  verde_em_casas();
  media_vermelha();
  vermelho_em_casas();
  preenche_vetor();
  printa_vetor();

  resultado = mm.getResult(vetor);
  if(resultado != 0)
  {
    estado = 2;
    switch (resultado)
    {
    case 1:
      if(player_eh_X)
      {
        Serial.println("CASO1");
        luz = 2;
        envia_comando(3);
        som = 4;
        envia_comando(4);
      }
      else
      {
        Serial.println("CASO2");
        luz = 3;
        envia_comando(3);
        som = 3;
        envia_comando(4);
      }
      break;
    case 2:
      if(player_eh_X)
        {
          Serial.println("CASO3");
          luz = 3;
          envia_comando(3);
          som = 3;
          envia_comando(4);
        }
        else
        {
          Serial.println("CASO4");
          luz = 2;
          envia_comando(3);
          som = 4;
          envia_comando(4);
        }
        break;
      case 3:
        luz = 4;
        envia_comando(3);
        som = 5;
        envia_comando(4);
        break;
    }
  }

  if(myTurn && resultado == 0)
  {
    bestMove = mm.findBestMove(vetor);
    envia_comando(2);
    myTurn = false;
  }
}

void estado_inicial()
{
  getImagem();
  delay(200);
  checa_inicio();
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_96X96;
  config.pixel_format = PIXFORMAT_RGB565;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.fb_count = 1;

  //Bluetooth init
  SerialBT.begin(myName,true);

  connected = SerialBT.connect(slaveName);
  Serial.printf("Connecting to slave BT device named \"%s\"\n", slaveName.c_str());

  if(connected) {
    Serial.println("Connected Successfully!");
  } else {
    while(!SerialBT.connected(10000)) {
      Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app.");
    }
  }
  // Disconnect() may take up to 10 secs max
  if (SerialBT.disconnect()) {
    Serial.println("Disconnected!");
    connected = false;
  }
  // This would reconnect to the slaveName(will use address, if resolved) or address used with connect(slaveName/address).
  SerialBT.connect();
  if(connected) {
    Serial.println("Reconnected Successfully!");
  } else {
    while(!SerialBT.connected(10000)) {
      Serial.println("Failed to reconnect. Make sure remote device is available and in range, then restart app.");
    }
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera falhou com codigo 0x%x", err);
    som = 1;
    envia_comando(4);
    luz = 9;
    envia_comando(3);
    estado = 0;
    return;
  }
  else
    Serial.println("Camera funcionando ");
  
  Serial.println("Tudo pronto ");

  luz = 1;
  som = 2;
  envia_comando(3);
  envia_comando(4);
  delay(3000);
}

void loop() {
  if(!SerialBT.connected())
  {
    estado = 0;
    btConnect();
  }
  else
  {
    //estado = 3;
    switch(estado)
    {
      case 0:
        estado_inicial();
        break;
      case 1:
        estado_jogando();
        break;
      case 2:
        estado_final();
        break;
      case 3:
        calibracao();
        break;
    }
  }
}