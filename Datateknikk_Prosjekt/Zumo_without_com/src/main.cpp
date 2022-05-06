#include <Arduino.h>
#include <Zumo32U4.h>

#include "protothreads.h"

#include "deviceData.h"
#include "GridMovement.h"



//Deklarering av structs som kreves for Protothreads
pt serverCom;
pt ptSpeed;
pt ptCalculateAvSpeed;
pt ptBattery;
pt ptLineFollow;
pt ptFindBack;
pt ptPizza;

//Deklarering  av Funksjoner brukt
void Master();
int speedAndBatteryThread(struct pt* pt);
int avSpeedThread(struct pt* pt);
int lineFollowThread(struct pt* pt);
int batteryThread(struct pt* pt);
int findBackThread(struct pt* pt);
int pizzaThread(struct pt* pt);


void calibrateLineSensors(){            // funksjon for calibrering av linje sensorene

    lineSensors.resetCalibration();     // tilbakestiller Calibrering for å starte på nytt
    
    for(int i = 0; i < 120; i++){       // for loop som får bilen til å snurre hver vei for så for å calibrere sensorene basert
    if (i > 30 && i <= 90){             // på hva som er under Zumoen
      motors.setSpeeds(-200, 200);
    }
    else{
      motors.setSpeeds(200, -200);
    }

    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);               // Skrur motorene av igjen
}

void setup() {
    delay(100);                        // delay slik at man kan flytte hånden på oppstart
    vehicle_state = IDLE;              // setter Vehicle state på oppstart, denne variablen skal bli oppdatert av serveren
    vehicle_distanceDriven = 0;        
    lastError = 0;

    // P og D konstanter 
    pid_d_const = 2;                   // start verdier for P og D for å ha noe som utgangspunkt for koden til å kjøre
    pid_p_const = 1/6;


    lineSensors.initFiveSensors();    //initialiserer Linjesensorene
    turnSensorSetup();                //initialisere IMUen
    
    calibrateLineSensors();           //kaller på kalibrering funksjoner
    delay(20);


    pinMode(13,OUTPUT);              // 
                                     //
    digitalWrite(13,HIGH);           //blinker ledd for å signalisere oppstart
    delay(100);                      //
    digitalWrite(13,LOW);            //


    PT_INIT(&ptSpeed);               //initialiserer protothreadsene 
    PT_INIT(&ptLineFollow);
    PT_INIT(&ptPizza);
}

void loop() {
    Master();               // kjører Master som kontrolerer hva Zumoen gjør i diverse states
}




void Master(){                                                 //Kjører en switch case hvow selektive tråder blir schedulet 
    switch(vehicle_state){
        case IDLE:{
            motors.setSpeeds(0, 0);
            //
            break;
        }
        case CHARGING:{
            motors.setSpeeds(0, 0);
            // komunikation med server om batte
            break;
        }
        case LINE_FOLLOW:{
            PT_SCHEDULE(speedAndBatteryThread(&ptSpeed));
            PT_SCHEDULE(lineFollowThread(&ptLineFollow));
            break;
        }
        case PIZZA:{
            PT_SCHEDULE(speedAndBatteryThread(&ptSpeed));
            PT_SCHEDULE(lineFollowThread(&ptLineFollow));
            PT_SCHEDULE(pizzaThread(&ptPizza));
            break;
        }
        case FIND_BACK:{
            PT_SCHEDULE(speedAndBatteryThread(&ptSpeed));
            PT_SCHEDULE(lineFollowThread(&ptLineFollow));
            PT_SCHEDULE(findBackThread(&ptFindBack));
            break;
        }
        default:
        break;
    }
}



int speed_counter = 0;
int speedAndBatteryThread(struct pt* pt){ 
    PT_BEGIN(pt);
    for(;;){
        float lc = encoders.getCountsLeft();                                         // leser av enkoder vinklene
        float rc = encoders.getCountsRight();
        PT_SLEEP(pt,10);                                                             // venter 10ms slik at man kan regne ut differansen mellom t=0 og t = 10
        lc = encoders.getCountsLeft() - lc;
        rc = encoders.getCountsRight() - rc;

        float average_count = (lc+rc)/2.0;                                           // finner snittet av enkoder venstre og høyre
        
        vehicle_distanceDriven += average_count * 1,36986301369863e-4;               // regner ut avstand i m basert på tester vi har gjennomført

        if (average_count >= 7300){                                                  // resetter encoder verdien slik at det ikke overflower ved lengre kjøring
            encoders.getCountsAndResetLeft();
            encoders.getCountsAndResetRight(); 
        }

        vehicle_s.current_speed = 10.0*average_count;                                // regner ut farten basert på snittet av enkoder verdiene 


        if(speed_counter == 5999){                                                   // dersom man har funnet 6000 hastighets verdier så kan man spawne Snitt hastighet 
            PT_SPAWN(pt, &ptCalculateAvSpeed, avSpeedThread(&ptCalculateAvSpeed));   // utregne tråden.
            speed_counter = 0;
        }

        PT_YIELD(pt);
    }
    PT_END(pt);
}

int avSpeedThread(struct pt* pt){                                                    // regner ut sittet av 6000 float verdier
    PT_BEGIN(pt);
    for(;;){
        uint32_t sum = 0;
        for(int i = 0; i>6000; i++){
            sum += vehicle_s.speed_arr[i];
        }

        vehicle_s.average_speed_60s = sum/6000.00;

        PT_YIELD(pt);
    }
    PT_END(pt);
}

int batteryThread(struct pt* pt){                                                                   // tråden regner ut bateri state of charge basert på hastigheten de siste 60s
    PT_BEGIN(pt);
    for(;;){
        vehicle_b.state_of_charge = vehicle_b.state_of_charge*(0.08*vehicle_s.average_speed_60s);
        PT_YIELD(pt);
    }
    PT_END(pt);
}


// threads that do the normal line following stuff
int lineFollowThread(struct pt* pt){
    PT_BEGIN(pt);
    for(;;){
        int position = lineSensors.readLine(lineSensorValues);                             // leser av linje posisjon sensor verdien
        if (position > 1500|| position > 2500){                                            // sjekker om det er en linje under bilen om ikke hopper koden videre
            int error = position - 2000;                                                   // finner differansen fra det optimale
            int speedDifference = error*pid_d_const + pid_d_const * (error - lastError);   // bruker en PD regulator for å bestemme hvor mye pådrag som skal påføres
            lastError = error;

            int leftSpeed = (int)maxSpeed + speedDifference;                               // pådraget blir påført i form av differansen mellom de to beltene
            int rightSpeed = (int)maxSpeed - speedDifference;

            leftSpeed = constrain(leftSpeed, 0, (int)maxSpeed);                            // setter verdien regnet ut ovenfor til ett tall mellom 0 og max hastighets verdien
            rightSpeed = constrain(rightSpeed, 0, (int)maxSpeed);
            
            motors.setSpeeds(leftSpeed, rightSpeed);                                       // setter hastigheten som ble utregnet ovenfor
        }
        else{
            motors.setSpeeds(100,100);                                                     // dersom det ikke er en linje under bilen setter den motor hastigheten til 100
        }
        
        PT_YIELD(pt);
    }
    PT_END(pt);
}

int x = 2; 
int y = 2;
int xDriven = 0;
int yDriven = 0;
bool turnher = true;
//Threads that do the pizza shit
int pizzaThread(struct pt* pt){
    PT_BEGIN(pt);
    for(;;){
        readSensors();                                
        if(aboveLine(0) || aboveLine(4)){              //sjeker om bilen er ved ett kryss

            if(x - xDriven > 0){                       // dersom den ikke har kjørt ferdig i x retning skal den kjøre i x retning
                xDriven++;                             
            }
            else if(y - yDriven > 0){                  // dersom den ikke har kjørt ferdig i y retning skal den kjøre i y retning
                yDriven++;
            }
            
            if(y - yDriven == 0 ){                     // Zumoen har kommet frem til riktif kordinat
                turn('L');
                turn('L');
                xDriven = 0;
                yDriven = 0;
                turnher = true;
                vehicle_state = FIND_BACK;
            }
            else if (x - xDriven == 0 && turnher){    //sjekker om bilen har kommet til enden av x retningen
                turn('L');
                turnher = false;
            }
        }
        PT_SLEEP(pt,20);
        PT_YIELD(pt);
    }
    PT_END(pt);
}

int findBackThread(struct pt* pt){                    // gjør det samme som PIZZA tråden men i revers
    PT_BEGIN(pt);
    for(;;){
        
        readSensors();
        if(aboveLine(0) || aboveLine(4)){

            PT_SLEEP(pt,200);

            if(y - yDriven > 0){
                yDriven++;
            }
            else if(x - xDriven > 0){
                xDriven++;
            }
            
            if(x - xDriven == 0 ){
                turn('B');
                turn('B');
                PT_SLEEP(pt,2600);
                vehicle_state = IDLE;
            }
            else if (y - yDriven == 0 && turnher){
                turn('R');
                turnher = false;
            }
        }
        PT_SLEEP(pt,20);

        PT_YIELD(pt);
    }
    PT_END(pt);
}