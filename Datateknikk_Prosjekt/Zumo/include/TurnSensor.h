#pragma once

#include <Wire.h>

const int32_t turnAngle45    = 0x20000000;
const int32_t turnAngle90    = 2 * turnAngle45;
const int32_t turnAngle180   = 2 * turnAngle90;


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
}