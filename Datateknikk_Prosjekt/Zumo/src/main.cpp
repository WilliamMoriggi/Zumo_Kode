#include <Arduino.h>
#include <Zumo32U4.h>

#include "protothreads.h"

#include "deviceData.h"
#include "GridMovement.h"




// Threads that are responsible for speed and battery calculations
void serverCom();






void setup() {
    Serial1.begin(9600);
    Serial.begin(9600);

    millisOld = millis();
}

void loop() {
    if (millis() > millisOld + 250){
        serverCom();
        millisOld = millis();
    }     
}





void serverCom(){
    /*
        // Vehicle State //
        char state_TX[3];
        dtostrf(vehicle_state, -1,0, state_TX);
        Serial1.write('|');
        Serial1.write('A');
        Serial1.write(state_TX);
        Serial1.write(';');
    */
    
          // Current Speed //
          current_speed = 150;
        char current_speed_TX[7];
        dtostrf(current_speed, -6,2, current_speed_TX);
        Serial1.write('|');
        Serial1.write('B');
        Serial1.write(current_speed_TX);
        Serial1.write(';');

        Serial.print('|');
        Serial.print('B');
        Serial.print(current_speed_TX);
        Serial.print(';');
        Serial.print('\n');
        


          // SOC //
          state_of_charge = 89;
        char SOC_TX[7];
        dtostrf(state_of_charge, -6,2, SOC_TX);
        Serial1.write('|');
        Serial1.write('C');
        Serial1.write(SOC_TX);
        Serial1.write(';');

          // SOH //
          state_of_health = 0.234;
        Serial1.write(SOH);
        char SOH_TX[7];
        dtostrf(state_of_health, -6,5, SOH_TX);
        Serial1.write('|');
        Serial1.write('D');
        Serial1.write(SOH_TX);
        Serial1.write(';');

}
