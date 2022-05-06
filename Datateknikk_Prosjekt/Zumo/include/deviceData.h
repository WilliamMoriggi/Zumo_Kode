#pragma once

struct vehicle_speed{
  float current_speed;
  float average_speed_60s;
  float speed_arr[6000];
};

struct vehicle_battery{
    float state_of_charge;
    float state_of_health;
    int charging_cycles;
};

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];

float vehicle_distanceDriven;
vehicle_speed vehicle_s;
vehicle_battery vehicle_b;

enum zumoState {IDLE, CHARGING, LINE_FOLLOW, PIZZA, FIND_BACK};

Zumo32U4ButtonA button;
Zumo32U4Motors motors;
Zumo32U4Encoders encoders;
Zumo32U4LineSensors lineSensors;
Zumo32U4IMU imu;

int vehicle_state;

int lastError;
const int maxSpeed = 150;

float pid_d_const;
float pid_p_const;