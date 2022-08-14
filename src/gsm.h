#include <SoftwareSerial.h>

String apn = "java.claro.com.br";                    //APN
String apn_u = "claro";                     //APN-Username
String apn_p = "claro";                     //APN-Password
String url = "api-esp.herokuapp.com/datalogsensor/";  //URL of Server


SoftwareSerial SWserial(33, 12); // RX, TX


void gsm_send_serial(String command) {
  Serial.println("Send ->: " + command);
  SWserial.println(command);
  long wtimer = millis();
  while (wtimer + 3000 > millis()) {
    while (SWserial.available()) {
      Serial.write(SWserial.read());
    }
  }
  Serial.println();
}

void gsm_config_gprs() {
  Serial.println(" --- CONFIG GPRS --- ");
  gsm_send_serial("AT+SAPBR=3,1,Contype,GPRS");
  gsm_send_serial("AT+SAPBR=3,1,APN," + apn);
  if (apn_u != "") {
    gsm_send_serial("AT+SAPBR=3,1,USER," + apn_u);
  }
  if (apn_p != "") {
    gsm_send_serial("AT+SAPBR=3,1,PWD," + apn_p);
  }
}

void gsm_http_post(String Dados_Sensor) {

  String httpRequestData =  Dados_Sensor;
  Serial.println(" --- Start GPRS & HTTP --- ");
  gsm_send_serial("AT+SAPBR=1,1");
  gsm_send_serial("AT+SAPBR=2,1");
  gsm_send_serial("AT+HTTPINIT");
  gsm_send_serial("AT+HTTPPARA=CID,1");
  gsm_send_serial("AT+HTTPPARA=URL," + url);
  gsm_send_serial("AT+HTTPPARA=USERDATA,Authorization: Basic bWFyY2lvZ2FycmlkbzozMTUwMjA=");
  gsm_send_serial("AT+HTTPPARA=CONTENT,application/json");
  gsm_send_serial("AT+HTTPDATA="+String(httpRequestData.length())+",5000");
  gsm_send_serial(httpRequestData);
  gsm_send_serial("AT+HTTPACTION=1");
  gsm_send_serial("AT+HTTPREAD");
  gsm_send_serial("AT+HTTPTERM");
  gsm_send_serial("AT+SAPBR=0,1");
}