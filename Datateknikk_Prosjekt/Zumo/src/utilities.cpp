#include "utilities.h"




void calcuateSpeed(vehicle_speed* vehicle_s){
    Zumo32U4Encoders encoders;


    unsigned long st = millis() + 2;
    int lc = encoders.getCountsLeft();
    int rc = encoders.getCountsRight();
    if(millis()<st){
    lc = encoders.getCountsLeft() - lc;
    rc = encoders.getCountsRight() - rc;
    }
    float average_count = ((float)lc+(float)rc)/2.0;
    vehicle_s->current_speed = 50.0*average_count; //in rotations per second 

}

void calculateDistanceSinceStart(){
  int c = 12234;
}
