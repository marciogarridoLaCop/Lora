/**
Programa de Pós Graduação em Engenharia Elétrica e Telecomunicações
LACOP - Laboratório de Comunicações Óticas - UFF
Autor : Márcio Alexandre Dias Garrido
**/
#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <logo.h>
#include <setupwifi.h>
#include <api.h>


String rssi = "RSSI --";
String packSize = "--";
String packet;
String sensorReadings;


void setup()
{
  Serial.begin(115200);
  inicializa_sensor();
  Heltec.begin(true /*DisplayEnable Enable*/, true /*LoRa Enable*/, true /*Serial Enable*/, true /*LoRa use PABOOST*/, BAND /*LoRa RF working band*/);
  logo();
  Heltec.display->clear();
  WIFIScan(1);
  WIFISetUp();
  delay(1000);
  Heltec.display->clear();
  wellcome();
  delay(1000);
}


void loop()
{
  delay(1000);
  sensorReadings = httpPostdata(1);
  Serial.println(sensorReadings);
}
