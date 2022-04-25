#include <Arduino.h>
#include <Zumo32U4.h>

#include "protothreads.h"

#include "deviceData.h"
#include "TurnSensor.h"

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];

int lastError = 0;
int maxSpeed = 300;
float pid_d_const = 3;
float pid_p_const = 1/3;

//speed calculation variables
int speed_counter = 0;



Zumo32U4ButtonA button;
Zumo32U4Motors motors;
Zumo32U4Encoders encoders;
Zumo32U4LineSensors lineSensors;
Zumo32U4IMU imu;

pt ptDrive;
pt ptSpeed;
pt ptCalculateAvSpeed;
pt ptBattery;
pt ptFindBack;

int speedThread(struct pt* pt);
int avSpeedThread(struct pt* pt);
int driveThread(struct pt* pt);
int batteryThread(struct pt* pt);
int findBackThread(struct pt* pt);

int speedThread(struct pt* pt){
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


int driveThread(struct pt* pt){
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

int batteryThread(struct pt* pt){
    PT_BEGIN(pt);
    for(;;){
        vehicle_b.state_of_charge = vehicle_b.state_of_charge*(0.08*vehicle_s.average_speed_60s);
        Serial.println(vehicle_b.state_of_charge);

        PT_YIELD(pt);
    }
    PT_END(pt);
}

void setup() {
    delay(500);
    vehicle_distanceDriven = 0;
    lineSensors.initFiveSensors();
    imu.configureForTurnSensing();
    
    calibrateLineSensors();

    PT_INIT(&ptSpeed);
    PT_INIT(&ptDrive);
}

void loop() {
    PT_SCHEDULE(speedThread(&ptSpeed));
    PT_SCHEDULE(driveThread(&ptDrive));

    

}