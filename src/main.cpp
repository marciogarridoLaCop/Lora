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
#include <ArduinoJson.h>
#include <HTTPClient.h>


String rssi = "RSSI --";
String packSize = "--";
String packet;
HTTPClient http;
String sensorReadings;
unsigned int counter = 0;
const char* serverName = "https://api-esp.herokuapp.com/DataLog/";

void setup()
{
  //pinMode(PIN_AP, INPUT);
  Serial.begin(115200);
  inicializa_sensor();
  Heltec.begin(true /*DisplayEnable Enable*/, true /*LoRa Enable*/, true /*Serial Enable*/, true /*LoRa use PABOOST*/, BAND /*LoRa RF working band*/);
  logo();
  Heltec.display->clear();
  WIFIScan(1);
  WIFISetUp();
 
 // WiFi.disconnect(true); 
  delay(1000);
  //WiFi.mode(WIFI_STA);
  //WiFi.setAutoConnect(true);
  Heltec.display->clear();
  wellcome();
  delay(1000);

  //wifiManager.setAPCallback(configModeCallback);
  //wifiManager.setSaveConfigCallback(saveConfigCallback);
}
void loop()
{
  /**
  if (digitalRead(PIN_AP) == LOW)
  {
    configurar_WIFI();
  }
  **/
  
 
  http.begin(serverName);     //Specify request destination
  int httpCode = http.GET();            //Send the request
  String payload = http.getString();    //Get the response payload
  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  http.end();  //Close connection
 
  //coletadados();
  Serial.print("IP : ");
  Serial.println(WiFi.localIP());
  Serial.println("");
 
  delay(1000);
}

