#include <Arduino.h>
#include <SPI.h>
#include <logo.h>
#include <setupwifi.h>
#include <transmissao.h>


String rssi = "RSSI --";
String packSize = "--";
String packet;
int t = 0;
int RST_PIN=23; 


void setup()
{
  Serial.begin(115200);
  Heltec.begin(true /*DisplayEnable Enable*/, true /*LoRa Enable*/, true /*Serial Enable*/, true /*LoRa use PABOOST*/, BAND /*LoRa RF working band*/);
  logo();
  Heltec.display->clear();
  wellcome();
  delay(1000);
  WIFIScan(1);
  WIFISetUp();
  SWserial.begin(9600);
  delay(2000);
}

void loop() {

  delay(5000);
  envio_dados();

}
