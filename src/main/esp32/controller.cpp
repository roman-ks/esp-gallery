#include "controller.h"
#define LOG_LEVEL 1
#include "log.h"
#include "pins_config.h"


// in micros
#define SHORT_PRESS_THRESHOLD 100000
#define LONG_PRESS_THRESHOLD 500000
#define LONG_PRESS_INTERVAL 500000
#define PRESS_TTL 10000
// time to wait before reacting after button release
#define PRESS_DELAY 10000


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
    int scrollRightVal = digitalRead(pin);
    uint32_t now = micros(); // high-resolution timestamp
    if (scrollRightVal == LOW) {
        portENTER_CRITICAL_ISR(&timerMux);
        if (scrollRightStart == 0) {
           scrollRightStart = now;  // first time pressed
        }
        scrollRightEnd = now;        // update last seen press
        portEXIT_CRITICAL_ISR(&timerMux);
    }

    pin = ENTER_BUTTON;
    pinIndex = pin-BUTTON_INDEX_OFFSET;
    int enterVal = digitalRead(pin) ;
    now = micros(); // high-resolution timestamp
    if (enterVal == LOW) {
        portENTER_CRITICAL_ISR(&timerMux);
        if (enterStart == 0) {
            enterStart = now;  // first time pressed
        }
        enterEnd = now;        // update last seen press
        portEXIT_CRITICAL_ISR(&timerMux);
    }

}

void Controller::initButton(int pin){
    pinMode(pin, INPUT_PULLUP);
    buttonPressHandledTime[pin-BUTTON_INDEX_OFFSET]=0;
    buttonLongPressHandledTime[pin-BUTTON_INDEX_OFFSET]=0;
}


void Controller::loop(){
    // if(isButtonLongPressed(SCROLL_RIGHT_BUTTON)){
    //     LOG_I("Scroll right button long pressed");
    //     handleRightButtonLongPress();
    // }
    if(isScrollRightPressed()){
        LOG_I("Scroll right button pressed");
        handleRightButtonPress();
    } else if(isEnterPressed()){
        LOG_I("Enter button pressed");
        handleEnterButtonPress();
    } else {
        // No button pressed, keep drawing current image if any
        gallery.draw();
    }
    resetPressEnter();
    resetPressScrollLeft();
}

void Controller::resetPressEnter() {
    uint32_t now = micros();
    portENTER_CRITICAL(&timerMux);
    uint32_t start = enterStart;
    uint32_t end   = enterEnd;
    portEXIT_CRITICAL(&timerMux);
    if(end == 0){
        return;
    }
    bool reset = false;
    if(now - end > PRESS_TTL && buttonPressHandledTime[ENTER_BUTTON-BUTTON_INDEX_OFFSET] == start){
        portENTER_CRITICAL(&timerMux);
        enterStart = 0;
        enterEnd = 0;
        portEXIT_CRITICAL(&timerMux);
        reset = true;
    }
#if defined(LOG_LEVEL) && LOG_LEVEL <=1
    LOGF_D("Pin: enter, reset: %d\n", reset);
#endif
}

void Controller::resetPressScrollLeft() {
    uint32_t now = micros();
    portENTER_CRITICAL(&timerMux);
    uint32_t start = scrollRightStart;
    uint32_t end   = scrollRightEnd;
    portEXIT_CRITICAL(&timerMux);
    if(end == 0){
        return;
    }
    bool reset = false;
    if(now - end > PRESS_TTL && buttonPressHandledTime[SCROLL_RIGHT_BUTTON-BUTTON_INDEX_OFFSET] == start){
        portENTER_CRITICAL(&timerMux);
        scrollRightStart = 0;
        scrollRightEnd = 0;
        portEXIT_CRITICAL(&timerMux);
        reset = true;
    }
#if defined(LOG_LEVEL) && LOG_LEVEL <=1
    LOGF_D("Pin: scrollRight, reset: %d\n", reset);
#endif
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

bool Controller::isEnterPressed(){

    portENTER_CRITICAL(&timerMux);
    uint32_t start = enterStart;
    uint32_t end   = enterEnd;
    portEXIT_CRITICAL(&timerMux);
    size_t pinIndex = ENTER_BUTTON-BUTTON_INDEX_OFFSET;
// #if defined(LOG_LEVEL) && LOG_LEVEL <=1
    
    // LOGF_D("Pin: enter, start: %lu, end: %lu, handled: %lu, sinceEnd: %lu, durr: %lu \n", 
    //     start, end, buttonPressHandledTime[pinIndex], micros() - end, end - start);

// #endif
    if (start != 0) {
        
        if(micros() - end > PRESS_DELAY && end - start > SHORT_PRESS_THRESHOLD){
            if(start > buttonPressHandledTime[pinIndex]){
                buttonPressHandledTime[pinIndex] = start;
                return true;
            }
        }
    }
    return false;
}
bool Controller::isScrollRightPressed(){
    portENTER_CRITICAL(&timerMux);
    uint32_t start = scrollRightStart;
    uint32_t end   = scrollRightEnd;
    portEXIT_CRITICAL(&timerMux);
    size_t pinIndex = SCROLL_RIGHT_BUTTON-BUTTON_INDEX_OFFSET;

#if defined(LOG_LEVEL) && LOG_LEVEL <=1
    // if(pin == SCROLL_RIGHT_BUTTON){
    //         LOGF_D("Pin: %d, start: %lu, end: %lu, handled: %lu\n", 
    //     pin, start, end, buttonPressHandledTime[pinIndex]);
    // }
#endif
    if (start != 0) {
        if(micros() - end > PRESS_DELAY && end - start > SHORT_PRESS_THRESHOLD){
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
    // uint32_t start = presses[pinIndex][0];
    // uint32_t end   = presses[pinIndex][1];
    portEXIT_CRITICAL(&timerMux);
    
    // if (start != 0) {
    //     if(end - start > LONG_PRESS_THRESHOLD){
    //         uint32_t now = micros();
    //         if(now - buttonLongPressHandledTime[pinIndex] > LONG_PRESS_INTERVAL){
    //             buttonLongPressHandledTime[pinIndex] = now;
    //             return true;
    //         }
    //     }
    // }
    return false;
}

