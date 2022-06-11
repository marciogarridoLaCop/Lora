/**
Programa de Pós Graduação em Engenharia Elétrica e Telecomunicações
LACOP - Laboratório de Comunicações Óticas - UFF
Autor : Márcio Alexandre Dias Garrido
Biblioteca de configuração do AP no ESP32
**/
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <DNSServer.h>
#include <WebServer.h>
const int PIN_AP = 13;


WiFiManager wifiManager;

void configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println("Entrou no modo de configuração");
  Serial.println(WiFi.softAPIP());                      // imprime o IP do AP
  Serial.println(myWiFiManager->getConfigPortalSSID()); // imprime o SSID criado da rede
}
void saveConfigCallback()
{
  Serial.println("Configuração salva");
}


void conectado(){
  if(WiFi.status()== WL_CONNECTED){ //Se conectado na rede
      digitalWrite(32,HIGH); //Acende LED AZUL
      
   }
   else{ //se não conectado na rede
      digitalWrite(32,LOW); //Apaga LED AZUL
      //Pisca LED Vermelho
      digitalWrite(26,HIGH);
      delay(500);
      digitalWrite(26,LOW);
      delay(500);
      wifiManager.autoConnect();//Função para se autoconectar na rede
   }  
}

void configurar_WIFI()
{



    Serial.println("Abertura Portal"); // Abre o portal
    digitalWrite(26, HIGH);            // Acende LED Vermelho
    digitalWrite(32, LOW);
    wifiManager.resetSettings(); // Apaga rede salva anteriormente
    if (!wifiManager.startConfigPortal("ESP32-ADMIN", "12456"))
    {                                      // Nome da Rede e Senha gerada pela ESP
      Serial.println("Falha ao conectar"); // Se caso não conectar na rede mostra mensagem de falha
      delay(2000);
      ESP.restart(); // Reinicia ESP após não conseguir conexão na rede
    }
    else
    { // Se caso conectar
      Serial.println("Conectado na Rede!!!");
      ESP.restart(); // Reinicia ESP após conseguir conexão na rede
    }
  


}
