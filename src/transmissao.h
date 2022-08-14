#include <WiFi.h>
#include <api.h>
#include <sensor.h>
#include <gsm.h>


void envio_dados()
{
    inicializa_sensor();
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("Envio por Wifi");
        wifi_http_post(coletadados("1"));
       
    }
    else
    {
       Serial.println("Envio por GSM");
       gsm_config_gprs();
       gsm_http_post(coletadados("1"));
      
    }
}