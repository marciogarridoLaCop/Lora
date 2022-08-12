
#include <SoftwareSerial.h>

String URL="pdpa-dashboard.herokuapp.com/GET?aca_adj";
SoftwareSerial gprsSerial(33, 12); //(RX, TX da Esp32 para se comunicar com o MODEM GSM SIM800)
String str, Resposta;
int Pos1, Pos2, ContPos, Espera;
unsigned long StartTime, LastTime;
int retorno=0;

float RX_Pwr;
float temp = 25.0;
float h = 60.0;
float TempInt=29.0;
int PressaoAtm=101705;
float aca=0.8;
float aca_adj=0.2;

void ShowSerialData(){
  while(gprsSerial.available()!=0){ 
    Serial.write(gprsSerial.read());    
    }
  delay(500);
  Serial.println();
}


void TemperaturaUmidadeMonitor(){ 
  Serial.println();
  Serial.println("----------------------------------------------------------");
  Serial.println("TemperaturaUmidadeMonitor - dados de temperatura e umidade");
  Serial.println("----------------------------------------------------------");     
//==== Mostra no Monitor Serial os dados de temperatura e umidade ==
  Serial.println("--------------------------------------------------");
  Serial.println("DHT11 - Temperatura Externa e Umidade");
  Serial.print(F("Umidade: "));
  Serial.print(h);
  Serial.print(F("%  Temperatura: "));
  Serial.print(temp);
  Serial.print(F("°C "));
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

void EnviaDadosViaGSM9(){
//Envio dos dados via GSM
  Serial.println();
  Serial.println("----------------------------------------------------------");
  Serial.println("EnviaDadosViaGSM - Envio dos dados via GSM - HostGator com sequencia de AT+HTTPxxxxx com GET");
  Serial.println("                                 SEIS         Dados enviados como parametros");
  Serial.println("----------------------------------------------------------");
  gprsSerial.println("AT");
  delay(1000);
          while(gprsSerial.available()!=0){ 
          Serial.write(gprsSerial.read());    
          }
          delay(3000);
          Serial.println();
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
  gprsSerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""); //Abre a conexao GPRS
  delay(1000);
  ShowSerialData();
  delay(1000);
  gprsSerial.println("AT+CSTT?"); // Verifica se o APN. usuario e senha estao definidos
  delay(2000);
   Resposta="";
   Espera=1;
   Pos1=20;  //posicao do caracter inicial da resposta
   Pos2=24;  //posicao do caracter final  da resposta
   ShowFullSerialData();
  delay(1000);

  if(Resposta=="CMNET"){
//======= Seta APN usuario e senha           
              gprsSerial.println("AT+SAPBR=3,1,\"APN\",\"java.claro.com.br\""); // SETA o APN. 
              delay(2000);
              ShowSerialData();
              delay(1000);

              gprsSerial.println("AT+SAPBR=3,1,\"USER\",\"claro\""); // SETA o usuario. 
              delay(2000);
              ShowSerialData();
              delay(1000);

              gprsSerial.println("AT+SAPBR=3,1,\"PWD\",\"claro\""); // SETA a senha. 
              delay(2000);
              ShowSerialData();
              delay(1000);
//=================================

//========Abre a conexao GPRS ========
              gprsSerial.println("AT+SAPBR=1,1");  //Abre a conexão sem fio com GPRS
              delay(3000);
              ShowSerialData();
              delay(1000);

              gprsSerial.println("AT+SAPBR=2,1");  //consulta o status e Obtem o valor do IP da conexão
              delay(2000);
              ShowSerialData();
              delay(1000);
//=================================
  }

  gprsSerial.println("AT+HTTPINIT");   // Initialize HTTP service 
  delay(4000);
  ShowSerialData();
  delay(1000);

  gprsSerial.println("AT+HTTPPARA=\"CID\",1");  // Set parameters for HTTP session 
  delay(1000);
  ShowSerialData();
  delay(1000);
  String str="AT+HTTPPARA=\"URL\",\""+String(URL)+String(aca_adj)+"&aca="+String(aca)+"&temp="+String(temp)+"&h="+String(h)+"&tempInt="+String(TempInt)+"&PressaoAtm="+String(PressaoAtm)+String("\"");
  //Serial.println(str);     
  gprsSerial.println(str);// Set parameters for HTTP session  
  delay(3000);
  ShowSerialData();
  delay(1000);
  gprsSerial.println("AT+HTTPACTION=0");  /* Start GET session */
  delay(3000);
   Resposta="";
   Espera=1;
   Pos1=1;  //posicao do caracter inicial da resposta
   Pos2=50;  //posicao do caracter final  da resposta
   ShowFullSerialData();
  delay(1000);
  gprsSerial.println("AT+HTTPREAD");  /* leitura */
  delay(3000);
  ShowSerialData();
  delay(1000);
  gprsSerial.println("AT+HTTPTERM");  /* Terminate HTTP service */
  delay(4000);
  ShowSerialData();
  delay(1000);
  gprsSerial.println("AT+SAPBR=0,1"); /* Close GPRS context */
  delay(4000);
  ShowSerialData();
  delay(1000);      
}