#include "controller.h"
#include "log.h"


Controller::~Controller() {
    detachInterrupt(SCROLL_LEFT_BUTTON);
    detachInterrupt(ENTER_BUTTON);
}

void Controller::init(){
    initButton(SCROLL_LEFT_BUTTON, leftISR);
    initButton(ENTER_BUTTON, enterISR);
}

void Controller::initButton(int pin, void isrFunc(void)){
    pinMode(pin, INPUT_PULLUP);
    buttonStates[pin] = HIGH;
    lastButtonPresses[pin] = 0;
    attachInterrupt(pin, isrFunc, FALLING);
}

void Controller::leftISR(){
    handleISR(SCROLL_LEFT_BUTTON);
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
    if(isButtonPressed(SCROLL_LEFT_BUTTON)){
        LOG("Scroll left button pressed");
        handleLeftButtonPress();
    } else if(isButtonPressed(ENTER_BUTTON)){
        LOG("Enter button pressed");
        handleEnterButtonPress();
    } else {
        // No button pressed, keep drawing current image if any
        gallery.draw();
    }
}

void Controller::handleLeftButtonPress(){
    if(gallery.isImageOpen()){
        gallery.nextImage();
    } else {
        gallery.nextHighlight();
    }
    // buttonStates[SCROLL_LEFT_BUTTON]--;
}

void Controller::handleEnterButtonPress(){
    if(gallery.isImageOpen()){
        gallery.closeImage();
    } else {
        gallery.openImage();
    }
    // buttonStates[ENTER_BUTTON]--;

}

bool Controller::isButtonPressed(int pin){
    bool pressed = buttonStates[pin] > 0;
    if(pressed){
        buttonStates[pin]--;
    } 
    return pressed;
}

