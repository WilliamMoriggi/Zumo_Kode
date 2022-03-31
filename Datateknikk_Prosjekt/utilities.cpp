#include "utilities.h"
#include <Zumo32U4Encoders.h>

void calcuateSpeed(){
  
  int st = millis() + 2;
  int lc = getCountsLeft();
  int rc = getCountsRight();
  while(millis()<st){}
  lc = getCountsLeft() - lc;
  rc = getCountsRight() - rc;
  
  float average_count = ((float)lc+(float)rc)/2.0;
  vehicle_speed.current_speed = 50.0*average_count; //in rotations per second 

}

void calculatedistanceSinceStart(){
  
}
