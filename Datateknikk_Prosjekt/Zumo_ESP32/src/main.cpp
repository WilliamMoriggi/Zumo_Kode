#include <Arduino.h>

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    Serial.read();
    byte incomingByte;
    while(Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    Serial.println(incomingByte);
    
    }

/*
    byte data[] = {vehicle_state, 
                       pid_p_const, 
                       pid_d_const, 
                       vehicle_s.current_speed, 
                       vehicle_b.state_of_charge, 
                       vehicle_b.state_of_health, 
                       vehicle_b.charging_cycles};



    char values[7];
    int state;
    int i;
    
    while (Serial.available() > 0) {
        i = Serial.readBytesUntil('\n', values, sizeof(values) - 1);
        values[i] = '\0';                 // Now it's a string, so
        Serial.println(values);           // ...show it
    }
    */
}