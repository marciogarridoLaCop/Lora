#include <HTTPClient.h>
#include <sensor.h>


const char *serverName = "https://api-esp.herokuapp.com/DataLogSensor/";
const char *username = "hardware-api";
const char *password = "S@les0288";

String httpPostdata(int sensorID )// O parametro id sensor é o identificador da estação hardware cadastrada na API- REST
{
  coletadados();
  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");
  http.setAuthorization(username,password);
  String dados;
  dados =  "{\"sensor\":\"" + String(sensorID);
  dados += "\",\"temperatura\":\"" + String(sensorReadingsArr[0]);
  dados += "\",\"pressao\":\"" + String(sensorReadingsArr[1]);
  dados += "\",\"altitude\":\"" + String(sensorReadingsArr[2]);
  dados += "\",\"pressa_nivel_mar\":\"" + String(sensorReadingsArr[3]);
  dados += "\",\"altitude_real\":\"" + String(sensorReadingsArr[4]) + "\"}";
  String httpRequestData = dados;
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
  
  http.end();

  return payload;
}
String httpGETRequest()
{
  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.setAuthorization(username,password);
  
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
  
  http.end();

  return payload;
}