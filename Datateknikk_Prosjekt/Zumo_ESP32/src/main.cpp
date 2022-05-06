#include <Arduino.h>
#include <string.h>
#include <WiFi.h>
#include <PubSubClient.h>

enum  messageID {STATE, PID_P, PID_D, CURRENT_SPEED, SOC, SOH, CHARGE_CYCLES};

WiFiClient espClient;
PubSubClient client(espClient);

void connectToServer();
void callback(char* topic, byte* message, unsigned int length);
void reconnect();
void checkConection();


void setup(){
  connectToServer();
  Serial.begin(9600);
}


void loop(){
    checkConection();

              //Lokale variabler som brukes for å lese av og sende videre dataen som kommer inn via RX og TX
    char data_rx;                                       
    char message[10];
    int counter = 0;
    bool messageReceved = false;
    bool start = false;

    while(Serial.available()>0 && !messageReceved) {         // Sjekker det er noe i bufferet, og om det leses en melding
        data_rx = Serial.read();                             // leser en byte ut fra bufferet
        if(data_rx > 32 && data_rx < 126 && start){          // sjekker om byten man leser av vil tillsvare en bokstav, tall eller symboler
            if(data_rx != ';'){                              // sjekker om det er enden av meldingen
                message[counter] = data_rx;                  // dersom det ikke er enden av meldingen legges den inn i ett char array
                counter++;
            }
            else{                                            
                message[counter] = '\0';                     // dersom meldingen er ferding legged det ved en \0 som ender char arrayet
                messageReceved = true;                       // gjør om messageReceved til true, dette vil få koden ut av while loopen
                Serial.flush();                              // tømmer bufferet for å slippe å overflowe
            }
        }
        if(data_rx == '|'){start = true;}                    // sjekker om det er starten av meldingen og tillater koden å lese det neste som kommer
    } 
    
delay(100);

char ID = message[0];                                        // finner IDen til meldingen som brukes for å bestemme hvor den sendes
  switch(ID){
      case 'A':{
          client.publish("zumo/currentMode", message);
          break;
      }
      case 'X':{
          client.publish("zumo/currentP", message);
          break;
      }
      case 'Y':{
          client.publish("zumo/currentD", message);
          break;
      }
      case 'B':{
          client.publish("zumo/speed", message);
          break;
      }
      case 'C':{
          client.publish("zumo/SoC", message);
          break;
      }
      case 'D':{
          client.publish("zumo/SoH", message);
          break;
      }
      delay(100);
  }
}

void connectToServer(){ // putt i setup()

  // KOBLE SEG TIL INTERNETT
  const char* ssid = "FBI surveillance van"; // Internett navn
  const char* password = "dennis123"; // Internett passord
  const char* mqtt_server = "192.168.212.224"; // RaspberryPi IP
  delay(10);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // KOBLE SEG TIL MQTT SERVER / RaspberryPi
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void checkConection(){
  if(!client.connected()){
    reconnect();
  }
  client.loop();
}

// --------------------------------------------------------
// Funksjoner som brukes i andre. Trengs ikke å brukes, med unntak av lesing av meldinger i callback.
// --------------------------------------------------------
void callback(char* topic, byte* message, unsigned int length){
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }

}

void reconnect(){ // her må alle paths legges til
  while(!client.connected()){ // Loop until we're reconnected
    if (client.connect("ESP8266Client")){ // Attempt to connect
    }
    else{
      delay(5000);
    }
  }
}