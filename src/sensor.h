/**
Programa de Pós Graduação em Engenharia Elétrica e Telecomunicações
LACOP - Laboratório de Comunicações Óticas - UFF
Autor : Márcio Alexandre Dias Garrido
Biblioteca de leitura do Adafruit_BMP085 e print de dados no display do LORA
**/
#include <heltec.h>
#include <Adafruit_BMP085.h>
String value;

// porta física do sensor bmp
#define SDA 21
#define SCL 13


Adafruit_BMP085 bme;
TwoWire I2Cone = TwoWire(1);

#define BAND 868E6 // you can set band here directly,e.g. 868E6,915E6,433E6

// BME280 definição da porta SDA e SLC
void inicializa_sensor()
{
  I2Cone.begin(SDA, SCL, 100000);
  bool status1 = bme.begin(0x76, &I2Cone);
  if (!status1)
  {
    Serial.println("Não foi possível localizar o sensor !");
    while (1)
      ;
  }
}

void coletadados(){
  value ="Temperatura = ";  //make sure the string is empty if its not
  value += bme.readTemperature();
  value +=" *C"; 
  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, value);
  
  value ="Pressão = ";  //make sure the string is empty if its not
  value += bme.readPressure();
  value +=" *Pa"; 
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 10, value);

  value ="Altitude = ";  //make sure the string is empty if its not
  value += bme.readAltitude();
  value +=" metros"; 
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 20, value);


  value ="P. mar = ";  //make sure the string is empty if its not
  value += bme.readSealevelPressure();
  value +=" Pa"; 
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 30, value);
  

  value ="Altitude real = ";  //make sure the string is empty if its not
  value += bme.readAltitude(101500);
  value +=" Pa"; 
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 40, value);
  Heltec.display->display();

}