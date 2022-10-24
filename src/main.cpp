#include <Arduino.h>
#include <SPI.h>
#include <logo.h>
#include <setupwifi.h>
#include <transmissao.h>
#include "TimeLib.h"


String rssi = "RSSI --";
String packSize = "--";
String packet;
int t = 0;
int RST_PIN=23; 
unsigned long StartTime, LastTime;
unsigned long EnviaInicio, EnviaFim;
int Inicio=1;

void setup()
{
  Serial.begin(115200);
  Heltec.begin(true /*DisplayEnable Enable*/, true /*LoRa Enable*/, true /*Serial Enable*/, true /*LoRa use PABOOST*/, BAND /*LoRa RF working band*/);
  logo();
  Heltec.display->clear();
  wellcome();
  delay(1000);
  WIFIScan(1);
  WIFISetUp();
  SWserial.begin(9600);
  delay(2000);

}

  
void loop() {
  delay(1000);
  LastTime=now();
  int TempoEntreEnvios=10;
  if ((LastTime-StartTime>=TempoEntreEnvios))
  {
  Inicio=0; 
  StartTime=LastTime; 
  EnviaInicio=now();  
  envio_dados();
  EnviaFim=now();
  Serial.println();
  Serial.print("Tempo usado para Envio de dados = ");
  Serial.print(EnviaFim-EnviaInicio);
  Serial.println(" seg");
  }
      else
      {
       Serial.println();
       Serial.print("Tempo para proximo envio sem ocorrencias = ");
       Serial.print(TempoEntreEnvios-(LastTime-StartTime));
       Serial.println(" seg");
      }  
}
