#include "swBattery.h"

Battery::Battery(){
  state_of_charge = 100.00;
  state_of_health = 100;
  cycle_number = 0;
  emergency_charge_avalable = true; 
}


void Battery::calculate_state_of_charge(float distance_driven, float average_speed){
  state_of_charge = state_of_charge -(distance_driven*average_speed*0.004);//random formula this needs to be updated.
}

void Battery::emergency_charge(){
  if(emergency_charge_avalable){
    while(state_of_charge <= 20.00){
      //drive backwords
      //charge
      state_of_charge++;
    }
    emergency_charge_avalable = false;
  }
}
