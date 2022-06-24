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
#include <Arduino_JSON.h>
#include <HTTPClient.h>

String rssi = "RSSI --";
String packSize = "--";
String packet;
HTTPClient http;
String sensorReadings;

unsigned int counter = 0;
const char *serverName = "https://api-esp.herokuapp.com/DataLog/";
unsigned long timerDelay = 5000;
unsigned long lastTime = 0;
void setup()
{
  // pinMode(PIN_AP, INPUT);
  Serial.begin(115200);
  inicializa_sensor();
  Heltec.begin(true /*DisplayEnable Enable*/, true /*LoRa Enable*/, true /*Serial Enable*/, true /*LoRa use PABOOST*/, BAND /*LoRa RF working band*/);
  logo();
  Heltec.display->clear();
  WIFIScan(1);
  WIFISetUp();

  // WiFi.disconnect(true);
  delay(1000);
  // WiFi.mode(WIFI_STA);
  // WiFi.setAutoConnect(true);
  Heltec.display->clear();
  wellcome();
  delay(1000);

  // wifiManager.setAPCallback(configModeCallback);
  // wifiManager.setSaveConfigCallback(saveConfigCallback);
}



String httpPostdata(const char *serverName,int sensor,float temperatura,float umidade,float pressao,float altitude)
{
  
  HTTPClient http;

  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");
  http.setAuthorization("hardware-api", "S@les0288");
  String httpRequestData = "{\"sensor\":\"" + String(1) + "\",\"temperatura\":\"" + String(24) +"\",\"umidade\":\"" + String(2) + "\",\"pressao\":\"" + String(10) + "\"}";

  // Send HTTP POST request
  int httpResponseCode = http.POST(httpRequestData);

  String payload = "{}";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
String httpGETRequest(const char *serverName)
{
   HTTPClient http;

  
  http.begin(serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.setAuthorization("hardware-api", "S@les0288");
  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
void loop()
{
  //sensorReadings = httpPostdata(serverName);
  //Serial.println(sensorReadings);
  delay(1000);
  coletadados();
  
}
