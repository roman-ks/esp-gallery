#include "controller.h"
#include "log.h"


Controller::~Controller() {
    detachInterrupt(SCROLL_RIGHT_BUTTON);
    detachInterrupt(ENTER_BUTTON);
}

void Controller::init(){
    initButton(SCROLL_RIGHT_BUTTON, rightISR);
    initButton(ENTER_BUTTON, enterISR);
}

void Controller::initButton(int pin, void isrFunc(void)){
    pinMode(pin, INPUT_PULLUP);
    buttonStates[pin] = 0;
    lastButtonPresses[pin] = 0;
    attachInterrupt(pin, isrFunc, FALLING);
}

void Controller::rightISR(){
    handleISR(SCROLL_RIGHT_BUTTON);
}

void Controller::enterISR(){
    handleISR(ENTER_BUTTON);
}

void Controller::handleISR(int pin){
    uint32_t time = millis();
    // wait debounce time
    if(time - lastButtonPresses[pin] > 200 ){
        buttonStates[pin]++;
        lastButtonPresses[pin]=time;
    }
}

void Controller::loop(){
    if(isButtonPressed(SCROLL_RIGHT_BUTTON)){
        LOG("Scroll right button pressed");
        handleRightButtonPress();
    } else if(isButtonPressed(ENTER_BUTTON)){
        LOG("Enter button pressed");
        handleEnterButtonPress();
    } else {
        // No button pressed, keep drawing current image if any
        gallery.draw();
    }
}

void Controller::handleRightButtonPress(){
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
    bool pressed = buttonStates[pin] > 0;
    if(pressed){
        buttonStates[pin]--;
    } 
    return pressed;
}

