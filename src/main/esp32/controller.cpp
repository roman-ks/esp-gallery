#include "controller.h"
#define LOG_LEVEL 2
#include "log.h"
#include "pins_config.h"


// in micros
#define SHORT_PRESS_THRESHOLD 100000
#define LONG_PRESS_THRESHOLD 500000
#define LONG_PRESS_INTERVAL 500000
#define PRESS_TTL 10000


Controller::~Controller() {
    if(buttonTimer!=nullptr)
        timerEnd(buttonTimer);
}

void Controller::init(){
    initButton(SCROLL_RIGHT_BUTTON);
    initButton(ENTER_BUTTON);

    buttonTimer = timerBegin(1000000);
    timerAttachInterrupt(buttonTimer, &onButtonTimer); 
    timerAlarm(buttonTimer, 2000, true, 0);

}


void Controller::onButtonTimer() {
    int pin = SCROLL_RIGHT_BUTTON;
    size_t pinIndex = pin-BUTTON_INDEX_OFFSET;
    bool pressed = digitalRead(pin) == LOW;
    uint32_t now = micros(); // high-resolution timestamp
    if (pressed) {
        portENTER_CRITICAL_ISR(&timerMux);
        if (presses[pinIndex][0] == 0) {
            presses[pinIndex][0] = now;  // first time pressed
        }
        presses[pinIndex][1] = now;        // update last seen press
        portEXIT_CRITICAL_ISR(&timerMux);
        timerCount++;
    }

    pin = ENTER_BUTTON;
    pinIndex = pin-BUTTON_INDEX_OFFSET;
    pressed = digitalRead(pin) == LOW;
    now = micros(); // high-resolution timestamp
    if (pressed) {
        portENTER_CRITICAL_ISR(&timerMux);
        if (presses[pinIndex][0] == 0) {
            presses[pinIndex][0] = now;  // first time pressed
        }
        presses[pinIndex][1] = now;        // update last seen press
        portEXIT_CRITICAL_ISR(&timerMux);
    }
   
}

void Controller::initButton(int pin){
    pinMode(pin, INPUT_PULLUP);
    presses[pin-BUTTON_INDEX_OFFSET][0]=0;
    presses[pin-BUTTON_INDEX_OFFSET][1]=0;
    buttonPressHandledTime[pin-BUTTON_INDEX_OFFSET]=0;
    buttonLongPressHandledTime[pin-BUTTON_INDEX_OFFSET]=0;
}


void Controller::loop(){
    if(isButtonLongPressed(SCROLL_RIGHT_BUTTON)){
        LOG("Scroll right button long pressed");
        handleRightButtonLongPress();
    }
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
    LOGF_D("Timer count: %d\n", timerCount);
    resetPress(SCROLL_RIGHT_BUTTON);
    resetPress(ENTER_BUTTON);

}

void Controller::resetPress(int pin) {
    size_t pinIndex = pin-BUTTON_INDEX_OFFSET;
    uint32_t now = micros();
    portENTER_CRITICAL(&timerMux);
    uint32_t start = presses[pinIndex][0];
    uint32_t end   = presses[pinIndex][1];
    portEXIT_CRITICAL(&timerMux);
    if(end == 0){
        return;
    }
    bool reset = false;
    if(now - end > PRESS_TTL && start == buttonPressHandledTime[pinIndex]){
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
    uint32_t start = presses[pinIndex][0];
    uint32_t end   = presses[pinIndex][1];
    portEXIT_CRITICAL(&timerMux);
    
    LOGF_D("Pin: %d, start: %lu, end: %lu, handled: %lu\n", 
        pin, start, end, buttonPressHandledTime[pinIndex]);
    if (start != 0) {
        if(micros() - end > PRESS_TTL && end - start > SHORT_PRESS_THRESHOLD){
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
    uint32_t start = presses[pinIndex][0];
    uint32_t end   = presses[pinIndex][1];
    portEXIT_CRITICAL(&timerMux);
    
    if (start != 0) {
        if(end - start > LONG_PRESS_THRESHOLD){
            uint32_t now = micros();
            if(now - buttonLongPressHandledTime[pinIndex] > LONG_PRESS_INTERVAL){
                buttonLongPressHandledTime[pinIndex] = now;
                buttonPressHandledTime[pinIndex] = start;
                return true;
            }
        }
    }
    return false;
}

