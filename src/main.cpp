/**
Programa de Pós Graduação em Engenharia Elétrica e Telecomunicações
LACOP - Laboratório de Comunicações Óticas - UFF
Autor : Márcio Alexandre Dias Garrido
**/
#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <sensor.h>
#include <logo.h>
#include <setupwifi.h>

String rssi = "RSSI --";
String packSize = "--";
String packet;
unsigned int counter = 0;

void setup()
{
  pinMode(PIN_AP, INPUT);
  Serial.begin(9600);
  inicializa_sensor();
  Heltec.begin(true /*DisplayEnable Enable*/, true /*LoRa Enable*/, true /*Serial Enable*/, true /*LoRa use PABOOST*/, BAND /*LoRa RF working band*/);
  logo();
  delay(1000);
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
}

void loop()
{
  if (digitalRead(PIN_AP) == LOW)
  {
    configurar_WIFI();
  }
  coletadados();
  delay(1000);
}