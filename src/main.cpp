/**
Programa de Pós Graduação em Engenharia Elétrica e Telecomunicações
LACOP - Laboratório de Comunicações Óticas - UFF
Autor : Márcio Alexandre Dias Garrido
**/
#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <logo.h>
#include <sensor.h>
#include <setupwifi.h>
#include "TimeLib.h"
#include "modem.h"

#define SDAPIN 21
#define SCLPIN 32
#define BAND  868E6  //you can set band here directly,e.g. 868E6,915E6

//Servidor 

//variaveis usadas para leitura do BMP180


//pinos e variaveis usdas na void EnviaDadosViaGSM
int t = 0;
int RST_PIN=23;    //RST_PIN=7 no Arduino e RST_PIN=23 no Esp32 LoRa



unsigned long EnviaInicio, EnviaFim;
int Inicio=1;   //Variavel para indicar que o programa esta inciando e a rotina de envio de dados envia dados no inico do programa. Apos a primeira 
 //temp - temperatura externa vem do DHT11
float NivelAnterior, NivelAtual=0.0; //para setar o FLag de envio FlagEnvio quando a ACA volta a ficar <1.0cm
int FlagEnvio=0;



void setup()
{
  Heltec.begin(true /*DisplayEnable Enable*/, true /*LoRa Enable*/, true /*Serial Enable*/, true /*LoRa use PABOOST*/, BAND /*LoRa RF working band*/);
  Serial.begin(9600);
  logo();
  delay(500);
  //Configuracao do MOdem GSM 
  pinMode(RST_PIN, OUTPUT);        // funcao para fazer o resset pelo pino RST_PIN=23 
  gprsSerial.begin(4800);          // the GPRS baud rate   
  delay(500);
  digitalWrite(RST_PIN, LOW);      // reset the SIM800  making the voltage LOW in reset pin
  delay(1000); 
  digitalWrite(RST_PIN, HIGH);     // set the SIM800  making the voltage HIGH in reset pin
  delay(15000);                    // Espera 15 segundos para a CLARO ser reconhecida no SIM800
  gprsSerial.println("AT+COPS?"); // Display the current network operator
  delay(1000);
  ShowSerialData();    
  gprsSerial.println("AT+CMEE=2"); // Habilita report de erros. Com o CMEE=2 é habilitado 
  delay(1000);                     // o codigo de erro e o uso de palavas que identificam o erro
  ShowSerialData();
  gprsSerial.println("AT+CSQ");  // Indica a qualidade do sinal. Entre 2 e 30 significa potencia entre -110 e -54dBm
                                 // potencia= (resposta-2)*2 - 110 dBm
  delay(1000);
  Resposta="";
  Espera=1;
  Pos1=15;  //posicao do caracter inicial da resposta
  Pos2=19;  //posicao do caracter final  da resposta
  ShowFullSerialData();
  Serial.println(Resposta.toFloat());
  RX_Pwr=(Resposta.toFloat()-2)*2 - 110;
  Serial.print(RX_Pwr);
  Serial.println(" dBm");
  GetTime(); // Define StartTime (em segundos) que é o horario que o programa entrou em funcionamento
}

void toSerial()
{
  while(gprsSerial.available()!=0)
  {
    Serial.write(gprsSerial.read());
  }
}


void SetaFlagEnvio(){
  Serial.println();
  Serial.println("--------------------------------------------------");
  Serial.println("SetaFlagEnvio - Seta o FlagEnvio quando aca volta para ZERO");
  Serial.println("--------------------------------------------------");
  
  NivelAnterior = NivelAtual;
  NivelAtual=aca;
 if(NivelAnterior>=1.0 && NivelAtual<1.0){
       FlagEnvio=1;
       Serial.print("FlagEnvio = ");
       Serial.println(FlagEnvio);
  }
  else{
       FlagEnvio=0;
       Serial.print("FlagEnvio = ");
       Serial.println(FlagEnvio);    
  }    
}

void loop()
{

  SetaFlagEnvio();     //== Seta FlagEnvia=1 quando o nivel de agua volta para menos que 1.0cm fr coluna de agua
  delay(5000);
  LastTime=now();
  int TempoEntreEnvios=300;
  if ((LastTime-StartTime>=TempoEntreEnvios)||(Inicio==1)||(aca_adj>=1.0)|| (FlagEnvio==1)){  //
      Inicio=0; 
      StartTime=LastTime; 
      EnviaInicio=now();  
      EnviaDadosViaGSM9(); // envia SEIS dados para o HostGator (como parametros) com a sequencia de comandos AT_HTTPxxxx com GET -OK funcionando
      EnviaFim=now();
      Serial.println();
      Serial.print("Tempo usado para EnvioDadosViaGSM =");
      Serial.print(EnviaFim-EnviaInicio);
      Serial.println(" seg");
  }
  else{
       Serial.println();
       Serial.print("Tempo para proximo envio sem ocorrencias =");
       Serial.print(TempoEntreEnvios-(LastTime-StartTime));
       Serial.println(" seg");
      }  
  
}
