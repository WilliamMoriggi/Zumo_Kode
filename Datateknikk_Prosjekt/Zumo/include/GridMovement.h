#pragma once

#include <Wire.h>
#include "deviceData.h"
#include "TurnSensor.h"
#include "Arduino.h"

const uint16_t turnSpeed = 150;

const uint16_t sensorThreshold = 150;

// Takes calibrated readings of the lines sensors and stores them
// in lineSensorValues.  Also returns an estimation of the line
// position.
uint16_t readSensors()
{
  return lineSensors.readLine(lineSensorValues);
}

// Returns true if the sensor is seeing a line.
// Make sure to call readSensors() before calling this.
bool aboveLine(uint8_t sensorIndex)
{
  return lineSensorValues[sensorIndex] > sensorThreshold;
}


void turn(char dir)
{
  turnSensorReset();
  uint8_t sensorIndex;
  switch(dir)
  {
  case 'B':
    // Turn left 125 degrees using the gyro.
    motors.setSpeeds(-turnSpeed, turnSpeed);
    while((int32_t)turnAngle < turnAngle180){
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
  /*
    while(1)
  {
    if (readSensors() <= 2200 || readSensors() >= 1800)
    {
      break;
    }
  }
  */
}