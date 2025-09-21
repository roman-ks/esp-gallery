#include "controller.h"
#include "log.h"

// in micros
#define SHORT_PRESS_THRESHOLD 100000
#define LONG_PRESS_THRESHOLD 400000
#define LONG_PRESS_INTERVAL 500000
#define DEBOUNCE_DURATION 10000


Controller::~Controller() {
    detachInterrupt(SCROLL_RIGHT_BUTTON);
    detachInterrupt(ENTER_BUTTON);
}

void Controller::init(){
    // initNavButton(SCROLL_RIGHT_BUTTON, rightFallingISR, rightRisingISR);

    pinMode(SCROLL_RIGHT_BUTTON, INPUT_PULLUP);
    buttonStates[SCROLL_RIGHT_BUTTON] = 0;
    buttonFallingTime[SCROLL_RIGHT_BUTTON] = 0;
    buttonRisingTime[SCROLL_RIGHT_BUTTON] = 0;
    attachInterrupt(SCROLL_RIGHT_BUTTON, rightChangeISR, CHANGE);
    // initButton(ENTER_BUTTON, enterISR);
}

void Controller::initNavButton(int pin, void isrFallingFunc(void), void isrRisingFunc(void)){
    // initButton(pin, isrFallingFunc);
    // buttonRisingTime[pin] = 0;
    // attachInterrupt(pin, isrRisingFunc, RISING);
}

void Controller::initButton(int pin, void isrFallingFunc(void)){
    // pinMode(pin, INPUT_PULLUP);
    // buttonStates[pin] = 0;
    // buttonFallingTime[pin] = 0;
    // attachInterrupt(pin, isrFallingFunc, FALLING);
}

void Controller::rightFallingISR(){
    // buttonFallingTime[SCROLL_RIGHT_BUTTON]=millis();
}

void Controller::rightRisingISR(){
    // buttonRisingTime[SCROLL_RIGHT_BUTTON]=millis();
}

void Controller::rightChangeISR(){
    int signal = digitalRead(SCROLL_RIGHT_BUTTON);
    if(signal == LOW){
        // LOW means pressed
        buttonFallingTime[SCROLL_RIGHT_BUTTON] = nowMicros();
    } else {
        buttonRisingTime[SCROLL_RIGHT_BUTTON] = nowMicros();
    }
}


// void Controller::enterISR(){
//     handleISR(ENTER_BUTTON);
// }

void Controller::handleISR(int pin){
    // uint32_t time = millis();
    // // wait debounce time
    // if(time - lastButtonPresses[pin] > 200 ){
    //     buttonStates[pin]++;
    //     lastButtonPresses[pin]=time;
    // }
}

void Controller::loop(){
    if(isButtonLongPressed(SCROLL_RIGHT_BUTTON)){
        LOG("Scroll right button long pressed");
        handleRightButtonLongPress();
    }
    if(isButtonPressed(SCROLL_RIGHT_BUTTON)){
        LOG("Scroll right button pressed");
        handleRightButtonPress();
    } /*else if(isButtonPressed(ENTER_BUTTON)){
        LOG("Enter button pressed");
        handleEnterButtonPress();
    } */else {
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

void Controller::handleRightButtonLongPress(){
    if(!gallery.isImageOpen()){
        // static uint32_t lastHandledTime=0;

        // uint32_t now = millis();
        // if(now- buttonEventHandledTime[SCROLL_RIGHT_BUTTON] > LONG_PRESS_INTERVAL){
            gallery.nextPage();
            // buttonEventHandledTime[SCROLL_RIGHT_BUTTON] = now;
        // }
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
    uint64_t lastFalling = buttonFallingTime[pin];
    uint64_t lastRising = buttonRisingTime[pin];

    LOGF_D("Pin: %d, Last falling: %llu, raising: %llu, r-f: %llu\n", 
            pin, lastFalling, lastRising, lastRising-lastFalling);
    if(lastRising==0){
        return false;
    }
    if(lastFalling > lastRising){
        // button hasnt been released yet
        return false;
    }
    uint64_t lastPressDurr = lastRising - lastFalling;

    // LOGF_D("Pin: %d, lastPressDurr: %llu, > SHORT_PRESS_THRESHOLD: %d, < LONG_PRESS_THRESHOLD: %d\n", 
    //     pin, lastPressDurr, lastPressDurr > SHORT_PRESS_THRESHOLD, lastPressDurr < LONG_PRESS_THRESHOLD);
    if(lastPressDurr > SHORT_PRESS_THRESHOLD 
        && lastPressDurr < LONG_PRESS_THRESHOLD){
            // LOGF_D("Pin: %d, Duration ok\n", pin);
            if(buttonPressHandledTime[pin]!=lastRising){
                // LOGF_D("Pin: %d, Registering press\n", pin);
                buttonPressHandledTime[pin]=lastRising;
                return true;
            }
        }
    return false;
}

bool Controller::isButtonLongPressed(int pin){
    uint64_t lastFalling = buttonFallingTime[pin];
    uint64_t lastRising = buttonRisingTime[pin];

    // LOGF_D("Last falling: %llu, raising: %llu\n", lastFalling, lastRising);
    if(lastFalling==0){
        return false;
    }

    if(lastRising >= lastFalling){
        return false;
    }
    uint64_t now = nowMicros();
    if(now-lastFalling > LONG_PRESS_THRESHOLD){
        if(now - buttonLongPressHandledTime[pin] > LONG_PRESS_INTERVAL){
            buttonLongPressHandledTime[pin] = now;
            return true;
        }
    }
    return false;
}


uint64_t Controller::nowMicros(){
    // drop last digit
    uint64_t ms = millis()/10;
    // get last 4 digits, containing up to 9 full millis and up to 999 micros
    uint32_t mcrs =  micros() % 10000;

    return ms * 10000 + mcrs;
}
