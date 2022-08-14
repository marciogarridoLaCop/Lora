#include <WiFi.h>
#include <api_wifi.h>
#include <api_gsm.h>
#include <sensor.h>

void envio_dados()
{
    inicializa_sensor();
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("Envio por Wifi");
        wifi_http_post(coleta_dados("1"/*Id do sensor*/));
       
    }
    else
    {
       Serial.println("Envio por GSM");
       gsm_config_gprs();
       gsm_http_post(coleta_dados("1"/*Id do sensor*/));
      
    }
}