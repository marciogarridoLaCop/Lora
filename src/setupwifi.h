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
#include <heltec.h>
//const int PIN_AP = 13;


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

void WIFISetUp(void)
{
  const char *ssid = "Quarto";
  const char *password = "master10";
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);
  WiFi.begin(ssid, password);
  delay(100);

  byte count = 0;
  while (WiFi.status() != WL_CONNECTED && count < 10)
  {
    count++;
    delay(500);
    Heltec.display->drawString(0, 0, "Conectando...");
    Heltec.display->display();
  }

  Heltec.display->clear();
  if (WiFi.status() == WL_CONNECTED)
  {
    Heltec.display->drawString(0, 0, "Conexão...OK.");
    Heltec.display->display();
    //		delay(500);
  }
  else
  {
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Falha na conexão");
    Heltec.display->display();
    // while(1);
  }
  Heltec.display->drawString(0, 10, "WIFI Setup OK");
  Heltec.display->display();
  delay(500);
}
void WIFIScan(unsigned int value)
{
  unsigned int i;
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.mode(WIFI_MODE_NULL);
  }
  for (i = 0; i < value; i++)
  {
    Heltec.display->drawString(0, 20, "Procurando redes...");
    Heltec.display->display();

    int n = WiFi.scanNetworks();
    Heltec.display->drawString(0, 30, "Procura finalizada");
    Heltec.display->display();
    delay(500);
    Heltec.display->clear();

    if (n == 0)
    {
      Heltec.display->clear();
      Heltec.display->drawString(0, 0, "Sem redes ativas");
      Heltec.display->display();
      // while(1);
    }
    else
    {
      Heltec.display->drawString(0, 0, (String)n);
      Heltec.display->drawString(14, 0, "Redes encontradas:");
      Heltec.display->display();
      delay(500);

      for (int i = 0; i < n; ++i)
      {
        // Print SSID and RSSI for each network found
        Heltec.display->drawString(0, (i + 1) * 9, (String)(i + 1));
        Heltec.display->drawString(6, (i + 1) * 9, ":");
        Heltec.display->drawString(12, (i + 1) * 9, (String)(WiFi.SSID(i)));
        Heltec.display->drawString(90, (i + 1) * 9, " (");
        Heltec.display->drawString(98, (i + 1) * 9, (String)(WiFi.RSSI(i)));
        Heltec.display->drawString(114, (i + 1) * 9, ")");
        delay(10);
      }
    }

    Heltec.display->display();
    delay(800);
    Heltec.display->clear();
  }
}