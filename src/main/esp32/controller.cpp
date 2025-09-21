#include "controller.h"
#define LOG_LEVEL 2
#include "log.h"

// in micros
#define SHORT_PRESS_THRESHOLD 100000
#define LONG_PRESS_THRESHOLD 500000
#define LONG_PRESS_INTERVAL 500000
#define PRESS_TTL 10000


Controller::~Controller() {
    detachInterrupt(SCROLL_RIGHT_BUTTON);
    detachInterrupt(ENTER_BUTTON);
}

void Controller::init(){
    int pin = SCROLL_RIGHT_BUTTON;
    pinMode(pin, INPUT_PULLUP);
    presses[pin-BUTTON_INDEX_OFFSET][0]=0;
    presses[pin-BUTTON_INDEX_OFFSET][1]=0;
    buttonPressHandledTime[pin-BUTTON_INDEX_OFFSET]=0;
    buttonLongPressHandledTime[pin-BUTTON_INDEX_OFFSET]=0;
    buttonTimer = timerBegin(1000000);
    timerAttachInterrupt(buttonTimer, &onButtonTimer); 
    // Set alarm to call onTimer function every 5 seconds (value in microseconds).
    timerAlarm(buttonTimer, 2000, true, 0);

}


void Controller::onButtonTimer() {
    int pin = SCROLL_RIGHT_BUTTON;
    size_t pinIndex = pin-BUTTON_INDEX_OFFSET;
    bool pressed = digitalRead(pin) == LOW;
    uint64_t now = nowMicros(); // high-resolution timestamp
    if (pressed) {
        portENTER_CRITICAL_ISR(&timerMux);
        if (presses[pinIndex][0] == 0) {
            presses[pinIndex][0] = now;  // first time pressed
        }
        presses[pinIndex][1] = now;        // update last seen press
        portEXIT_CRITICAL_ISR(&timerMux);
    }
   
}

void Controller::initNavButton(int pin, void isrFallingFunc(void), void isrRisingFunc(void)){
    // initButton(pin, isrFallingFunc);
    // buttonRisingTime[pin] = 0;
    // attachInterrupt(pin, isrRisingFunc, RISING);
}

void Controller::initButton(int pin, void isrFallingFunc(void)){
}


void Controller::loop(){
    // LOGF_D("Timer count: %d\n", timerCount);
    resetPress(SCROLL_RIGHT_BUTTON);

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

void Controller::resetPress(int pin) {
    size_t pinIndex = pin-BUTTON_INDEX_OFFSET;
    uint64_t now = nowMicros();
    portENTER_CRITICAL(&timerMux);
    uint64_t end   = presses[pinIndex][1];
    portEXIT_CRITICAL(&timerMux);
    if(end == 0){
        return;
    }
    bool reset = false;
    if(now - end > PRESS_TTL){
        portENTER_CRITICAL(&timerMux);
        presses[pinIndex][0] = 0;
        presses[pinIndex][1] = 0;
        portEXIT_CRITICAL(&timerMux);
        reset = true;
    }
    LOGF_D("Pin: %d, reset: %d\n", pin, reset);
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
        gallery.nextPage();
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
    size_t pinIndex = pin-BUTTON_INDEX_OFFSET;
    portENTER_CRITICAL(&timerMux);
    uint64_t start = presses[pinIndex][0];
    uint64_t end   = presses[pinIndex][1];
    portEXIT_CRITICAL(&timerMux);
    
    // LOGF_D("Pin: %d, start: %llu, end: %llu, handled: %llu\n", 
        // pin, start, end, buttonPressHandledTime[pinIndex]);
    if (start != 0) {
        if(nowMicros() - end > PRESS_TTL && end - start > SHORT_PRESS_THRESHOLD){
            if(start > buttonPressHandledTime[pinIndex]){
                buttonPressHandledTime[pinIndex] = start;
                return true;
            }
        }
    }
    return false;
}

bool Controller::isButtonLongPressed(int pin){
    size_t pinIndex = pin-BUTTON_INDEX_OFFSET;
    portENTER_CRITICAL(&timerMux);
    uint64_t start = presses[pinIndex][0];
    uint64_t end   = presses[pinIndex][1];
    portEXIT_CRITICAL(&timerMux);
    
    if (start != 0) {
        if(end - start > LONG_PRESS_THRESHOLD){
            uint64_t now = nowMicros();
            if(now - buttonLongPressHandledTime[pinIndex] > LONG_PRESS_INTERVAL){
                buttonLongPressHandledTime[pinIndex] = now;
                return true;
            }
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
