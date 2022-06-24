#include <heltec.h>
#include <lacop.h>
#include <WiFi.h>
void logo()
{
  Heltec.display->clear();
  Heltec.display->drawXbm(0, 0, uff_lacop_width, uff_lacop_height, (const unsigned char *)uff_lacop_bits);
  Heltec.display->display();
}

void wellcome()
{
  Heltec.display->clear();
  Heltec.begin(true, false, true);

  Heltec.display->setContrast(255);
  Heltec.display->clear();

  Heltec.display->setFont(ArialMT_Plain_24);
  Heltec.display->drawString(0, 0, "LoRa WiFi");

  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(0, 25, "WeatherCloud");

  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 45, "WiFi:" + WiFi.localIP().toString() );

  Heltec.display->display();
  Heltec.display->clear();
}