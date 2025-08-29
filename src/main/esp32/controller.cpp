#include "controller.h"

Controller::Controller(Gallery &gallery) : gallery(gallery) {
}

Controller::~Controller() {
}

void Controller::init(){
    pinMode(5, INPUT_PULLUP);
}

void Controller::loop(){
    if(isButtonPressed()){
        gallery.hightlightNext();
  }
}

bool Controller::isButtonPressed(){
  int buttonState = digitalRead(5);
  bool pressed = false;
  if(oldButtonState == HIGH && buttonState == LOW){
    pressed = true;
  }
  oldButtonState = buttonState;
  return pressed;
}

