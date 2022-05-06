#pragma once

#include <Wire.h>
#include "deviceData.h"
#include "Arduino.h"

const uint16_t turnSpeed = 150;
const uint16_t sensorThreshold = 150;

// constants For turning
const int32_t turnAngle45    = 0x20000000;
const int32_t turnAngle90    = 2 * turnAngle45;

uint32_t turnAngle = 0;
int16_t turnRate;
int16_t gyroOffset;
uint16_t gyroLastUpdate = 0;

void turnSensorReset(){
    turnAngle = 0;
}

void turnSensorUpdate(){

    imu.readGyro();
    turnRate = imu.g.z - gyroOffset;
    uint16_t m = micros();
    uint16_t dt = m - gyroLastUpdate;

    gyroLastUpdate = m;
    int32_t d = (int32_t)turnRate * dt;
    turnAngle += (int64_t)d * 14680064 / 17578125;
}

void turnSensorSetup()
{
  Wire.begin();
  imu.init();
  imu.enableDefault();
  imu.configureForTurnSensing();
  
  delay(500);

  int32_t total = 0;

  for (uint16_t i = 0; i < 1024; i++){
    while(!imu.gyroDataReady()) {}
    imu.readGyro();
    total += imu.g.z;
  }

}

uint16_t readSensors()
{
  return lineSensors.readLine(lineSensorValues);
}

bool aboveLine(uint8_t sensorIndex)
{
  return lineSensorValues[sensorIndex] > sensorThreshold;
}

void turn(char dir)
{
  turnSensorReset();
  switch(dir)
  {
  case 'B':
    // Turn left 125 degrees using the gyro.
    motors.setSpeeds(-turnSpeed, turnSpeed);
    while((uint32_t)turnAngle < (uint32_t)2*turnAngle90){
        digitalWrite(13,HIGH);
        turnSensorUpdate();
    }
    digitalWrite(13,LOW);
    break;

  case 'L':
    // Turn left 45 degrees using the gyro.
    motors.setSpeeds(-turnSpeed, turnSpeed);
    while((int32_t)turnAngle < turnAngle90){
        digitalWrite(13,HIGH);
        turnSensorUpdate();
    }
    digitalWrite(13,LOW);
    break;

  case 'R':
    // Turn right 45 degrees using the gyro.
    motors.setSpeeds(turnSpeed, -turnSpeed);
    while((int32_t)turnAngle > -turnAngle45){
        digitalWrite(13,HIGH);
        turnSensorUpdate();
    }
    digitalWrite(13,LOW);
    break;

  default:
    // This should not happen.
    return;
  }
}