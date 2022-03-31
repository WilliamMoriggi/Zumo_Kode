#include "drawDriver.h"
#include <Zumo32U4.h>

Zumo32U4OLED display;

void displayClear(){
  display.clear();
}

void drawText(int x, int y, char output){
  display.gotoXY(x,y);
  display.print(output);
}