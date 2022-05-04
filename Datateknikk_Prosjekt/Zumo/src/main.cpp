#include <Arduino.h>
#include <Zumo32U4.h>

#include "protothreads.h"

#include "deviceData.h"
#include "TurnSensor.h"
#include "GridMovement.h"




pt serverCom;

pt ptSpeed;
pt ptCalculateAvSpeed;
pt ptBattery;

pt ptLineFollow;
pt ptFindBack;

pt ptPizza;

// Declaration of the controll function
void Master();

// Threads that are responsible for speed and battery calculations
int serverComThread(struct pt* pt);
int speedAndBatteryThread(struct pt* pt);
int avSpeedThread(struct pt* pt);

// Threads that are responsible for normal line following code
int lineFollowThread(struct pt* pt);
int batteryThread(struct pt* pt);
int findBackThread(struct pt* pt);

int pizzaThread(struct pt* pt);



void calibrateLineSensors(){
    delay(500);

    lineSensors.resetCalibration();
    
    for(int i = 0; i < 120; i++){
    if (i > 30 && i <= 90){
      motors.setSpeeds(-200, 200);
    }
    else{
      motors.setSpeeds(200, -200);
    }

    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);   
}

void setup() {
    delay(100);
    vehicle_state = PIZZA;
    vehicle_distanceDriven = 0;
    lastError = 0;

    pid_d_const = 2;
    pid_p_const = 1/4;


    lineSensors.initFiveSensors();
    turnSensorSetup();
    
    calibrateLineSensors();
    delay(20);


    pinMode(13,OUTPUT);

    digitalWrite(13,HIGH);
    delay(100);
    digitalWrite(13,LOW);

    PT_INIT(&serverCom);
    PT_INIT(&ptSpeed);
    PT_INIT(&ptLineFollow);
    PT_INIT(&ptPizza);


    
}

void loop() {
    PT_SCHEDULE(serverComThread(&serverCom));
    //Master();
}




// State machine
void Master(){
    switch(vehicle_state){
        case IDLE:{
            motors.setSpeeds(0, 0);
            break;;
        }
        case CHARGING:{
            motors.setSpeeds(0, 0);
            // do nothing update the battery state
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
        default:
        break;
    }
}


// the server com thread that constantly runns to make sure stuff works
int serverComThread(struct pt* pt){
    PT_BEGIN(pt);
    for(;;){
        u8 tx_data[5];
        enum  messageID {STATE, PID_P, PID_D, CURRENT_SPEED, SOC, SOH, CHARGE_CYCLES};
        /*vehicle_state, 
          pid_p_const, 
          pid_d_const, 
          vehicle_s.current_speed, 
          vehicle_b.state_of_charge, 
          vehicle_b.state_of_health, 
          vehicle_b.charging_cycles*/

        typedef union {
            float floatingPoit;
            byte binary[4];
        } binaryFloat;

        binaryFloat pid_d;
        pid_d.floatingPoit = pid_d_const;

        Serial.write(PID_D);
        Serial.write(pid_d.binary,4);


        PT_YIELD(pt);
    }
    PT_END(pt);
}

// threads and Threads that are responsible for calculating the Speed and battery stuff
int speed_counter = 0;
int speedAndBatteryThread(struct pt* pt){
    PT_BEGIN(pt);
    for(;;){
        float lc = encoders.getCountsLeft();
        float rc = encoders.getCountsRight();
        PT_SLEEP(pt,10);                    
        lc = encoders.getCountsLeft() - lc;
        rc = encoders.getCountsRight() - rc;

        float average_count = (lc+rc)/2.0;
        
        vehicle_distanceDriven += average_count * 1,36986301369863e-4;

        if (average_count >= 7300){
            encoders.getCountsAndResetLeft();
            encoders.getCountsAndResetRight(); 
        }

        vehicle_s.current_speed = 10.0*average_count;


        if(speed_counter == 5999){
            PT_SPAWN(pt, &ptCalculateAvSpeed, avSpeedThread(&ptCalculateAvSpeed));
            speed_counter = 0;
        }

        PT_YIELD(pt);
    }
    PT_END(pt);
}

int avSpeedThread(struct pt* pt){
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

int batteryThread(struct pt* pt){
    PT_BEGIN(pt);
    for(;;){
        vehicle_b.state_of_charge = vehicle_b.state_of_charge*(0.08*vehicle_s.average_speed_60s);
        Serial.println(vehicle_b.state_of_charge);

        PT_YIELD(pt);
    }
    PT_END(pt);
}


// threads that do the normal line following stuff
int lineFollowThread(struct pt* pt){
    PT_BEGIN(pt);
    for(;;){
        int position = lineSensors.readLine(lineSensorValues);
        if (position > 1500|| position > 2500){
            int error = position - 2000;
            int speedDifference = error*pid_d_const + pid_d_const * (error - lastError);
            lastError = error;

            int leftSpeed = (int)maxSpeed + speedDifference;
            int rightSpeed = (int)maxSpeed - speedDifference;

            leftSpeed = constrain(leftSpeed, 0, (int)maxSpeed);
            rightSpeed = constrain(rightSpeed, 0, (int)maxSpeed);
            
            motors.setSpeeds(leftSpeed, rightSpeed);
        }
        else{
            motors.setSpeeds(100,100);
        }
        
        PT_YIELD(pt);
    }
    PT_END(pt);
}

int findBackThread(struct pt* pt){

}

int x = 2; 
int y = 3;
int xDriven = 0;
int yDriven = 0;
bool turnher = true;
//Threads that do the pizza shit
int pizzaThread(struct pt* pt){
    PT_BEGIN(pt);
    for(;;){

        //get X and Y from somewhere??
        // made these just to have something
        

        readSensors();
        if(aboveLine(0) || aboveLine(4)){

            PT_SLEEP(pt,200);

            if(x - xDriven > 0){
                xDriven++;
            }
            else if(y - yDriven > 0){
                yDriven++;
            }
            

            if(y - yDriven == 0 ){
                vehicle_state = IDLE;
            }
            else if (x - xDriven == 0 && turnher){
                turn('L');
                turnher = false;
            }
        }
        PT_SLEEP(pt,20);
        PT_YIELD(pt);
    }
    PT_END(pt);
}