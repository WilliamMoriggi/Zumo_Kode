#pragma once

class Battery{
  private: 
      float state_of_charge;
      int state_of_health;                //between 0 and 100
      int cycle_number;
      int critical_discharge_amout;       // critical discharge is anny time the battery is at 5% capacity 
      
      
      bool emergency_charge_avalable;


  public:

      Battery();
      const int get_state_of_charge(){return state_of_charge;}
      const int get_state_of_health(){return state_of_health;}
      const int get_cycle_number(){return cycle_number;}

      void calculate_state_of_charge(float distance_driven, float average_speed);
      void calculate_state_of_health(float average_speed, float max_speed, float time_at_topp_speed);

      void emergency_charge();
      
};
