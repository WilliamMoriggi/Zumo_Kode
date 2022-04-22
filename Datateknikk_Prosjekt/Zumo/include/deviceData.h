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

float vehicle_distanceDriven;
vehicle_speed vehicle_s;
vehicle_battery vehicle_b;
