#include <Arduino.h>

void setup()
{
    Serial.begin(9600);
    pinMode(13,OUTPUT);
}

void loop()
{
    Serial.println("Hello world!");
    digitalWrite(13,HIGH);
    delay(1000);
    digitalWrite(13,LOW);
}