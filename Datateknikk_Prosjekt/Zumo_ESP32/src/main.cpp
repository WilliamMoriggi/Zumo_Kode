#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>


WiFiClient espClient;
PubSubClient clientHerman(espClient);
String message;
#define MESSAGE_LENGTH 20
char msgArr[MESSAGE_LENGTH];

byte data[7];

void connectToServer(); //putt i setup

void putThisInLoop(); // putt i loop
void callback(char* topic, byte* message, unsigned int length);
void reconnect();

int shit = 0;

enum  messageID {STATE, PID_P, PID_D, CURRENT_SPEED, SOC, SOH, CHARGE_CYCLES};
void setup()
{
  connectToServer();
  Serial.begin(9600);
}
char incomingByte;

void loop()
{
    String data_rx = "";
  if(Serial.available() > 0) {
      if(Serial.read() == '?'){
          while(!Serial.read() == '\0' && Serial.available()){
              data_rx += (String)Serial.read();
          }
      }
  }

  switch(data_rx[0]){
      data_rx.remove(0);
      message = data_rx;
      case STATE:{
          message = data_rx;
          message.toCharArray(msgArr, MESSAGE_LENGTH);
          clientHerman.publish("zumo/currentMode", msgArr);
          break;
      }
      case PID_P:{
          message.toCharArray(msgArr, MESSAGE_LENGTH);
          clientHerman.publish("zumo/currentP", msgArr);
          break;
      }
      case PID_D:{
          message.toCharArray(msgArr, MESSAGE_LENGTH);
          clientHerman.publish("zumo/currentD", msgArr);
          break;
      }
      case CURRENT_SPEED:{
          message.toCharArray(msgArr, MESSAGE_LENGTH);
          clientHerman.publish("zumo/speed", msgArr);
          break;
      }
      case SOC:{
          message.toCharArray(msgArr, MESSAGE_LENGTH);
          clientHerman.publish("zumo/SoC", msgArr);
          break;
      }
      case SOH:{
          message.toCharArray(msgArr, MESSAGE_LENGTH);
          clientHerman.publish("zumo/SoH", msgArr);
          break;
      }
  }

putThisInLoop();
}


void connectToServer(){ // putt i setup()

  // KOBLE SEG TIL INTERNETT
  const char* ssid = "FBI surveillance van"; // Internett navn
  const char* password = "dennis123"; // Internett passord
  const char* mqtt_server = "192.168.208.224"; // RaspberryPi IP
  
  delay(10);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(WiFi.localIP());

  // KOBLE SEG TIL MQTT SERVER / RaspberryPi
  clientHerman.setServer(mqtt_server, 1883);
  clientHerman.setCallback(callback);
}

void putThisInLoop(){
  if(!clientHerman.connected()){
    reconnect();
  }
  clientHerman.loop();
}

void callback(char* topic, byte* message, unsigned int length){
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
}
void reconnect(){ // her må alle paths legges til
  while(!clientHerman.connected()){ // Loop until we're reconnected
    if (clientHerman.connect("ESP8266Client")){ // Attempt to connect
    }
    else{
      delay(5000);
    }
  }
}