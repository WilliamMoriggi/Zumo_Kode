#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>




WiFiClient espClient;
PubSubClient clientHerman(espClient);
String message;
char msgArr[50];

byte data[7];

void connectToServer(); //putt i setup

void putThisInLoop(); // putt i loop
void callback(char* topic, byte* message, unsigned int length);
void reconnect();

int shit = 0;
void setup()
{
  connectToServer();
  Serial.begin(9600);


    
}
char incomingByte;

typedef union {
  float floatingPoit;
  char binary[2];
} binaryFloat;

int counter = 0;
binaryFloat value;


void loop()
{
  int i = 0;
  while(Serial.available() > 0) {
    value.binary[i] = Serial.read();
    i++;
  }
  i = 0;
  
putThisInLoop();

  message = String(shit);
  message += ((String)value.floatingPoit);
  message.toCharArray(msgArr, 50);
  clientHerman.publish("zumo/debug", msgArr);
  shit++;
  if(shit > 10){shit =0;}
  delay(100);
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