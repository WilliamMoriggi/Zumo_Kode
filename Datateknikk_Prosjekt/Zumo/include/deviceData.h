#pragma once


float current_speed;
float average_speed_60s;
float speed_arr[6000];
float state_of_charge;
float state_of_health;
int charging_cycles;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];

float vehicle_distanceDriven;

enum zumoState {IDLE, CHARGING, LINE_FOLLOW, PIZZA};
enum  messageID {STATE, PID_P, PID_D, CURRENT_SPEED, SOC, SOH};

Zumo32U4ButtonA button;
Zumo32U4Motors motors;
Zumo32U4Encoders encoders;
Zumo32U4LineSensors lineSensors;
Zumo32U4IMU imu;

int vehicle_state;

int lastError;
const int maxSpeed = 200;

float pid_d_const;
float pid_p_const;

long millisOld;