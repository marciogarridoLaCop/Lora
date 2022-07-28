/**
Programa de Pós Graduação em Engenharia Elétrica e Telecomunicações
LACOP - Laboratório de Comunicações Óticas - UFF
Autor : Márcio Alexandre Dias Garrido
**/
#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <logo.h>
#include <SoftwareSerial.h>
#include <sensor.h>
#include <setupwifi.h>
#include "TimeLib.h"

String rssi = "RSSI --";
String packSize = "--";
String packet;
String sensorReadings;

#define SDAPIN 21
#define SCLPIN 32


#define BAND  868E6  //you can set band here directly,e.g. 868E6,915E6


SoftwareSerial gprsSerial(33, 12); //(RX, TX da Esp32 para se comunicar com o MODEM GSM SIM800)

//variaveis usadas para leitura do BMP180
float TempInt, PressaoAtm;

//pinos e variaveis usdas na void EnviaDadosViaGSM
int t = 0;
int retorno=0;
int Pos1, Pos2, ContPos, Espera;
int RST_PIN=23;    //RST_PIN=7 no Arduino e RST_PIN=23 no Esp32 LoRa

float RX_Pwr;
String str, Resposta;
unsigned long StartTime, LastTime;
unsigned long EnviaInicio, EnviaFim;
int Inicio=1;   //Variavel para indicar que o programa esta inciando e a rotina de envio de dados envia dados no inico do programa. Apos a primeira 
                //tranmissao, inicio é feito inicio=0 .

float aca, aca_adj;

float h;      //umidade vem do DHT11
float temp;  //temp - temperatura externa vem do DHT11


float NivelAnterior, NivelAtual=0.0; //para setar o FLag de envio FlagEnvio quando a ACA volta a ficar <1.0cm
int FlagEnvio=0;


// void que manda para o monitor seria o retorno dos comandos AT
void ShowSerialData(){
  while(gprsSerial.available()!=0){ 
    Serial.write(gprsSerial.read());    
    }
  delay(500);
  Serial.println();
}
void ShowFullSerialData(){
  str="";           //Armazena todo o comando  e a resposta
  Resposta="";       //Armazena somente a resosta que esta entre os caracteres Pos1 e Pos2
  ContPos=0;         //conta a posicao dos caracteres que estao chegando pela porta serial
  while(gprsSerial.available()!=0){ 
    retorno= gprsSerial.read();
    ContPos=ContPos+1;
    str=str+(char)retorno;
    if(Pos1<=ContPos && ContPos<=Pos2){
      Resposta=Resposta+(char)retorno;
    }
    Serial.write(retorno); 
    delay(Espera);   
    //Serial.print(" Eu recebi:");
    //Serial.println(retorno);
    //Serial.println((char)retorno);
    Serial.print(retorno, DEC);  
    }
  delay(Espera*2);
  Serial.println();
  Serial.print(str);
  Serial.println();
  Serial.print(Resposta);
  Serial.println();
}

void GetTime(){
  
  //float ano, mes, dia, hora, minuto, segundo;
  int ano, mes, dia, hora, minuto, segundo;
  Serial.println();
  Serial.println("--------------------------------------------------");
  Serial.println("GetTime - Recebendo horario local");
  Serial.println("--------------------------------------------------");
  
  gprsSerial.println("AT+CLTS=1"); // Recebe o horario local da operadora
  delay(1000);                     
  ShowSerialData();

  gprsSerial.println("AT&W"); // Recebe o horario local da operadora
  delay(1000);                     
  ShowSerialData();

  gprsSerial.println("AT+CCLK?"); // Recebe o horario local da operadora
  delay(1000);                     
  Resposta="";
  Espera=1;
  Pos1=20;  //posicao do caracter inicial da resposta
  Pos2=36;  //posicao do caracter final  da resposta
  ShowFullSerialData();
  delay(1000);
  ano=(Resposta.substring(0, 2)).toInt();
  mes=(Resposta.substring(3, 5)).toInt();
  dia=(Resposta.substring(6, 8)).toInt();
  hora=(Resposta.substring(9, 11)).toInt();
  minuto=(Resposta.substring(12, 14)).toInt();
  segundo=(Resposta.substring(15, 17)).toInt();
 
  setTime(hora,minuto,segundo,dia,mes,ano);
  StartTime=now();
  Serial.print("Tempo inicial = ");
  Serial.println(StartTime);
  
}

void setup()
{
  Serial.begin(9600);
  Heltec.begin(true /*DisplayEnable Enable*/, true /*LoRa Enable*/, true /*Serial Enable*/, true /*LoRa use PABOOST*/, BAND /*LoRa RF working band*/);
  
Serial.begin(9600);
  
  delay(500);

  //Configuracao do MOdem GSM 
  pinMode(RST_PIN, OUTPUT);        // funcao para fazer o resset pelo pino RST_PIN=23 
  gprsSerial.begin(4800);          // the GPRS baud rate   
  delay(500);
  digitalWrite(RST_PIN, LOW);      // reset the SIM800  making the voltage LOW in reset pin
  delay(1000); 
  digitalWrite(RST_PIN, HIGH);     // set the SIM800  making the voltage HIGH in reset pin
  delay(15000);                    // Espera 15 segundos para a CLARO ser reconhecida no SIM800
                                   // Deve ser alterado por algum teste que indique que o SIM800 se registrou na rede

  // Comandos para o Modem - reconhecer operadora, potencia, etc..                             
  gprsSerial.println("AT+COPS?"); // Display the current network operator
  delay(1000);
  ShowSerialData();    
  
  gprsSerial.println("AT+CMEE=2"); // Habilita report de erros. Com o CMEE=2 é habilitado 
  delay(1000);                     // o codigo de erro e o uso de palavas que identificam o erro
  ShowSerialData();
  
  gprsSerial.println("AT+CSQ");  // Indica a qualidade do sinal. Entre 2 e 30 significa potencia entre -110 e -54dBm
                                 // potencia= (resposta-2)*2 - 110 dBm
  delay(1000);
   //ShowSerialData();
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



void EnviaDadosViaGSM2(){
//Envio dos dados via GSM
  Serial.println();
  Serial.println("----------------------------------------------------------");
  Serial.println("EnviaDadosViaGSM - Envio dos dados via GSM - HostGator");
  Serial.println("----------------------------------------------------------");
 gprsSerial.println("AT");
  delay(1000);
  //ShowSerialData();
          //------Inicio do ShowSerialData
          while(gprsSerial.available()!=0){ 
          Serial.write(gprsSerial.read());    
          }
          delay(3000);
          Serial.println();
          //------FIm do ShowSerialData
  delay(1000);

  gprsSerial.println("AT+CPIN?");  //Indica se uma senha sera requerida. READY significa que nao requer senha
  delay(1000);
  ShowSerialData();
  delay(1000);

  gprsSerial.println("AT+CSQ");  // Indica a qualidade do sinal. Entre 2 e 30 significa potencia entre -110 e -54dBm
                                 // potencia= (resposta-2)*2 - 110 dBm
  delay(1000);
   //ShowSerialData();
  Resposta="";
  Espera=1;
  Pos1=15;  //posicao do caracter inicial da resposta
  Pos2=19;  //posicao do caracter final  da resposta
  ShowFullSerialData();
  Serial.println(Resposta.toFloat());
  RX_Pwr=(Resposta.toFloat()-2)*2 - 110;
  Serial.print(RX_Pwr);
  Serial.println(" dBm"); 
  delay(1000);

  gprsSerial.println("AT+CREG?");  //Network Registration . Resposta +CREG: X, Y - X nao importa muito. Y=1 significa que o SIM800 esta registrado na rede.
  delay(1000);
  ShowSerialData();
  delay(1000);

  gprsSerial.println("AT+CGATT?"); //Conectar ou desconectar do serviço de GPRS. Resposta é +CGATT: X. Se X=0 esta desconetado, se X=1 esta conectado.
  delay(1000);
  ShowSerialData();
  delay(1000);

  gprsSerial.println("AT+CSTT?"); // Verifica se o APN. usuario e senha estao definidos
  delay(2000);
  //ShowSerialData();
   Resposta="";
   Espera=1;
   Pos1=20;  //posicao do caracter inicial da resposta
   Pos2=24;  //posicao do caracter final  da resposta
   ShowFullSerialData();
  delay(1000);

  // Aqio fazer um teste para ver se a resposta é CMNET. Se
  // resposta==CMNET, entao aplicar em CSTT o APN, login e senha. Senao
  // pular os comandos AT+CSTT= e AT+CIICR.
  if(Resposta=="CMNET"){ 
              gprsSerial.println("AT+CSTT=\"claro.com.br\", \"claro\", \"claro\""); // SETA o APN. usuario e senha 
              delay(2000);
              ShowSerialData();
              delay(1000);

              gprsSerial.println("AT+CIICR");  //Abre a conexão sem fio com GPRS
              delay(2000);
              ShowSerialData();
              delay(1000);

              gprsSerial.println("AT+CIFSR");  //Obtem o valor do IP da conexão
              delay(2000);
              ShowSerialData();
              delay(1000);
          }

  gprsSerial.println("AT+CIPSTART=\"TCP\",\"pdpa.herokuapp.com\",\"80\"");

 //  gprsSerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");     
 
  delay(4000);
  ShowSerialData();
  delay(1000);

  gprsSerial.println("AT+CREG?");  //Para testar se o SIM800 esta registrado na rede antes do comendo CIPSEND
  delay(1500);
  ShowSerialData();
  delay(1000);

  gprsSerial.println("AT+CIPSTATUS");  //Consulta o status da conexão atual. A resosta é OK STATE:X sEsera-se que  X=CONNECT OK
  delay(1500);
  ShowSerialData();
  delay(1000);
  
  Resposta="";
  gprsSerial.println("AT+CIPSEND");      //begin send data to remote server      
          delay(1000);
          Espera=1;
          Pos1=14;  //posicao do caracter inicial da resposta
          Pos2=14;  //posicao do caracter final  da resposta
          ShowFullSerialData();
          
  if(Resposta==">"){ 
            //GET https://api.thingspeak.com/update?api_key=09YNJD9P01Z7ZNSU&field1=0
            //String str="GET https://api.thingspeak.com/update?api_key=NKRNDHMOOAXH3GEF&field1=" + String(t) ;  //float h = dht.readHumidity(); t e h são as variaveis
            //String str="GET https://api.thingspeak.com/update?api_key=09YNJD9P01Z7ZNSU&field1=" + String(aca) ;  //float h = dht.readHumidity(); t e h são as variaveis
            // VErsao 2.2 - String str="GET https://api.thingspeak.com/update?api_key=09YNJD9P01Z7ZNSU&field1=" + String(aca)+"&field2=" + String(temp)+"&field3=" + String(h) ;  //float h = dht.readHumidity(); t e h são as variaveis
            //(Ate a versao 2.4) String str="GET https://api.thingspeak.com/update?api_key=09YNJD9P01Z7ZNSU&field1=" + String(aca)+"&field2=" + String(temp)+"&field3=" + String(h)+ "&field4=" + String(TempInt)+ "&field5=" + String(PressaoAtm) ;     
            //String do canal 1 - Euipamento da UFF//String str="GET https://api.thingspeak.com/update?api_key=09YNJD9P01Z7ZNSU&field1=" + String(aca_adj)+ "&field2=" + String(aca)+"&field3=" + String(temp)+"&field4=" + String(h)+ "&field5=" + String(TempInt)+ "&field6=" + String(PressaoAtm) ;        
            //String str="GET https://api.thingspeak.com/update?api_key=QXTY66XHQIPZB1OO&field1=" + String(aca_adj)+ "&field2=" + String(aca)+"&field3=" + String(temp)+"&field4=" + String(h)+ "&field5=" + String(TempInt)+ "&field6=" + String(PressaoAtm) ;        
            
            //String str="GET https://pdpa.herokuapp.com/?aca_adj=" + String(aca_adj)+ "&aca=" + String(aca)+"&temp=" + String(temp)+"&h=" + String(h)+ "&TempInt=" + String(TempInt)+ "&PressaoAtm=" + String(PressaoAtm);
            String str="https://pdpa.herokuapp.com/?aca_adj=" + String(aca_adj)+ "&aca=" + String(aca)+"&temp=" + String(temp)+"&h=" + String(h)+ "&TempInt=" + String(TempInt)+ "&PressaoAtm=" + String(PressaoAtm);
            
            Serial.println(str); 
            delay(1000);
            
            gprsSerial.println(str);     //begin send data to remote server
            delay(2000);
            ShowSerialData();
            
            //Aqui fazer um teste do recebimento do SEND OK
            gprsSerial.println((char)26);   //CTRL+Z envia dados e fecha a conexão TCP
            delay(6000);                    //waitting for reply, important! the time is base on the condition of internet 
            gprsSerial.println();
            //ShowSerialData();
            Resposta="";
            Espera=1;
            Pos1=6;  //posicao do caracter inicial da resposta
            Pos2=12;  //posicao do caracter final  da resposta
            ShowFullSerialData();

            gprsSerial.println("AT+CIFSR");  //Obtem o valor do IP da coexão
            delay(2000);
            //ShowSerialData();
            Resposta="";
            Espera=1;
            Pos1=12;  //posicao do caracter inicial da resposta
            Pos2=27;  //posicao do caracter final  da resposta
            ShowFullSerialData();
            
            delay(500);

            gprsSerial.println("AT+CIPSTATUS");      //CLose the TCP connection    
            delay(2000);
            ShowSerialData();

              
          }
  else{
            digitalWrite(RST_PIN, LOW);    // reset the SIM800  making the voltage LOW in reset pin
            delay(2000); 
            digitalWrite(RST_PIN, HIGH);   // set the SIM800  making the voltage HIGH in reset pin
            delay(15000);                  // Espera 15 segundos para a CLARO ser reconhecida no SIM800
                                 // Deve ser alterado por algum teste que indique que o SIM800 registrou na rede
      }
}


void EnviaDadosViaGSM_Fake(){
  //Envio dos dados FAKE via GSM
  Serial.println();
  Serial.println("----------------------------------------------------------");
  Serial.println("EnviaDadosViaGSM_Fake - Envio dos dados via GSM FAKE");
  Serial.println("----------------------------------------------------------");
  Serial.print("Altura da Coluna = ");
  Serial.println(aca);
}


void EnviaDadosViaGSM(){
//Envio dos dados via GSM
  Serial.println();
  Serial.println("----------------------------------------------------------");
  Serial.println("EnviaDadosViaGSM - Envio dos dados via GSM - ThingSpeaks");
  Serial.println("----------------------------------------------------------");
 gprsSerial.println("AT");
  delay(1000);
  //ShowSerialData();
          //------Inicio do ShowSerialData
          while(gprsSerial.available()!=0){ 
          Serial.write(gprsSerial.read());    
          }
          delay(3000);
          Serial.println();
          //------FIm do ShowSerialData
  delay(1000);

  gprsSerial.println("AT+CPIN?");  //Indica se uma senha sera requerida. READY significa que nao requer senha
  delay(1000);
  ShowSerialData();
  delay(1000);

  gprsSerial.println("AT+CSQ");  // Indica a qualidade do sinal. Entre 2 e 30 significa potencia entre -110 e -54dBm
                                 // potencia= (resposta-2)*2 - 110 dBm
  delay(1000);
   //ShowSerialData();
  Resposta="";
  Espera=1;
  Pos1=15;  //posicao do caracter inicial da resposta
  Pos2=19;  //posicao do caracter final  da resposta
  ShowFullSerialData();
  Serial.println(Resposta.toFloat());
  RX_Pwr=(Resposta.toFloat()-2)*2 - 110;
  Serial.print(RX_Pwr);
  Serial.println(" dBm"); 
  delay(1000);

  gprsSerial.println("AT+CREG?");  //Network Registration . Resposta +CREG: X, Y - X nao importa muito. Y=1 significa que o SIM800 esta registrado na rede.
  delay(1000);
  ShowSerialData();
  delay(1000);

  gprsSerial.println("AT+CGATT?"); //Conectar ou desconectar do serviço de GPRS. Resposta é +CGATT: X. Se X=0 esta desconetado, se X=1 esta conectado.
  delay(1000);
  ShowSerialData();
  delay(1000);

  gprsSerial.println("AT+CSTT?"); // Verifica se o APN. usuario e senha estao definidos
  delay(2000);
  //ShowSerialData();
   Resposta="";
   Espera=1;
   Pos1=20;  //posicao do caracter inicial da resposta
   Pos2=24;  //posicao do caracter final  da resposta
   ShowFullSerialData();
  delay(1000);

  // Aqio fazer um teste para ver se a resposta é CMNET. Se
  // resposta==CMNET, entao aplicar em CSTT o APN, login e senha. Senao
  // pular os comandos AT+CSTT= e AT+CIICR.
  if(Resposta=="CMNET"){ 
              gprsSerial.println("AT+CSTT=\"claro.com.br\", \"claro\", \"claro\""); // SETA o APN. usuario e senha 
              delay(2000);
              ShowSerialData();
              delay(1000);

              gprsSerial.println("AT+CIICR");  //Abre a conexão sem fio com GPRS
              delay(2000);
              ShowSerialData();
              delay(1000);

              gprsSerial.println("AT+CIFSR");  //Obtem o valor do IP da conexão
              delay(2000);
              ShowSerialData();
              delay(1000);
          }

  gprsSerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");

 
  delay(4000);
  ShowSerialData();
  delay(1000);

  gprsSerial.println("AT+CREG?");  //Para testar se o SIM800 esta registrado na rede antes do comendo CIPSEND
  delay(1500);
  ShowSerialData();
  delay(1000);

  gprsSerial.println("AT+CIPSTATUS");  //Consulta o status da conexão atual. A resosta é OK STATE:X sEsera-se que  X=CONNECT OK
  delay(1500);
  ShowSerialData();
  delay(1000);
  
  Resposta="";
  gprsSerial.println("AT+CIPSEND");      //begin send data to remote server      
          delay(1000);
          Espera=1;
          Pos1=14;  //posicao do caracter inicial da resposta
          Pos2=14;  //posicao do caracter final  da resposta
          ShowFullSerialData();
          
  if(Resposta==">"){ 
            //GET https://api.thingspeak.com/update?api_key=09YNJD9P01Z7ZNSU&field1=0
            //String str="GET https://api.thingspeak.com/update?api_key=NKRNDHMOOAXH3GEF&field1=" + String(t) ;  //float h = dht.readHumidity(); t e h são as variaveis
            //String str="GET https://api.thingspeak.com/update?api_key=09YNJD9P01Z7ZNSU&field1=" + String(aca) ;  //float h = dht.readHumidity(); t e h são as variaveis
            // VErsao 2.2 - String str="GET https://api.thingspeak.com/update?api_key=09YNJD9P01Z7ZNSU&field1=" + String(aca)+"&field2=" + String(temp)+"&field3=" + String(h) ;  //float h = dht.readHumidity(); t e h são as variaveis
            //(Ate a versao 2.4) String str="GET https://api.thingspeak.com/update?api_key=09YNJD9P01Z7ZNSU&field1=" + String(aca)+"&field2=" + String(temp)+"&field3=" + String(h)+ "&field4=" + String(TempInt)+ "&field5=" + String(PressaoAtm) ;     

            //String do canal 1 - Equipamento da UFF
            //String str="GET https://api.thingspeak.com/update?api_key=09YNJD9P01Z7ZNSU&field1=" + String(aca_adj)+ "&field2=" + String(aca)+"&field3=" + String(temp)+"&field4=" + String(h)+ "&field5=" + String(TempInt)+ "&field6=" + String(PressaoAtm) ;        

            //String do canal 2 - Equipamento de Teste
            String str="GET https://api.thingspeak.com/update?api_key=QXTY66XHQIPZB1OO&field1=" + String(aca_adj)+ "&field2=" + String(aca)+"&field3=" + String(temp)+"&field4=" + String(h)+ "&field5=" + String(TempInt)+ "&field6=" + String(PressaoAtm) ;        
            Serial.println(str);
            delay(100);
            
            gprsSerial.println(str);     //begin send data to remote server
            delay(2000);
            ShowSerialData();
            
            //Aqui fazer um teste do recebimento do SEND OK
            gprsSerial.println((char)26);   //CTRL+Z envia dados e fecha a conexão TCP
            delay(6000);                    //waitting for reply, important! the time is base on the condition of internet 
            gprsSerial.println();
            //ShowSerialData();
            Resposta="";
            Espera=1;
            Pos1=6;  //posicao do caracter inicial da resposta
            Pos2=12;  //posicao do caracter final  da resposta
            ShowFullSerialData();

            gprsSerial.println("AT+CIFSR");  //Obtem o valor do IP da coexão
            delay(2000);
            //ShowSerialData();
            Resposta="";
            Espera=1;
            Pos1=12;  //posicao do caracter inicial da resposta
            Pos2=27;  //posicao do caracter final  da resposta
            ShowFullSerialData();
            
            delay(500);

            gprsSerial.println("AT+CIPSTATUS");      //CLose the TCP connection    
            delay(2000);
            ShowSerialData();

            
          }
  else{
            digitalWrite(RST_PIN, LOW);    // reset the SIM800  making the voltage LOW in reset pin
            delay(2000); 
            digitalWrite(RST_PIN, HIGH);   // set the SIM800  making the voltage HIGH in reset pin
            delay(15000);                  // Espera 15 segundos para a CLARO ser reconhecida no SIM800
                                 // Deve ser alterado por algum teste que indique que o SIM800 registrou na rede
      }
}

void SetaFlagEnvio(){
  Serial.println();
  Serial.println("--------------------------------------------------");
  Serial.println("SetaFlagEnvio - Seta o FlagEnvio quando aca volta para ZERO");
  Serial.println("--------------------------------------------------");
  
  NivelAnterior = NivelAtual;
  NivelAtual=aca;
  FlagEnvio;
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
 delay(1000);
 //sensorReadings = httpPostdata(1);
 //Serial.println(sensorReadings);
 //Serial.read();
}
