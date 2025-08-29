#include "controller.h"

Controller::Controller(Gallery &gallery) : gallery(gallery), buttonStates() {
}

Controller::~Controller() {
}

void Controller::init(){
    pinMode(SCROLL_LEFT_BUTTON, INPUT_PULLUP);
    pinMode(ENTER_BUTTON, INPUT_PULLUP);

    buttonStates[SCROLL_LEFT_BUTTON] = HIGH;
    buttonStates[ENTER_BUTTON] = HIGH;
}

void Controller::loop(){
    if(isButtonPressed(SCROLL_LEFT_BUTTON)){
        Serial.println("Scroll left button pressed");
        handleLeftButtonPress();
    } else if(isButtonPressed(ENTER_BUTTON)){
        Serial.println("Enter button pressed");
        handleEnterButtonPress();
    }
}

void Controller::handleLeftButtonPress(){
    if(gallery.isImageOpen()){
        gallery.nextImage();
    } else {
        gallery.nextHighlight();
    }
}

void Controller::handleEnterButtonPress(){
    if(gallery.isImageOpen()){
        gallery.closeImage();
    } else {
        gallery.openImage();
    }
}

bool Controller::isButtonPressed(int pin){
  int buttonState = digitalRead(pin);
  bool pressed = false;
  bool oldButtonState = buttonStates[pin];
  if(oldButtonState == HIGH && buttonState == LOW){
    pressed = true;
  }
  if(oldButtonState != buttonState){
    buttonStates[pin] = buttonState;
  }
  oldButtonState = buttonState;
  return pressed;
}

