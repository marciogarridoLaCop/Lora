#include <HTTPClient.h>

const char *serverName = "https://api-esp.herokuapp.com/datalogsensor/";
const char *username = "hardware-api";
const char *password = "S@les0288";


String wifi_http_post(String Dados_Sensor)// O parametro id sensor é o identificador da estação hardware cadastrada na API- REST
{
  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");
  http.setAuthorization(username,password);
  String httpRequestData =  Dados_Sensor;
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