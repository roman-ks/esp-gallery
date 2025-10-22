#include "main_controller.h"

#define LOG_LEVEL 2
#include "log.h"
#include "pins_config.h"

// in micros
#define SHORT_PRESS_THRESHOLD 100000
#define LONG_PRESS_THRESHOLD 500000
#define LONG_PRESS_INTERVAL 500000
#define PRESS_TTL 10000

MainController::~MainController() {
    if(buttonTimer!=nullptr)
        timerEnd(buttonTimer);
}

void MainController::init(){
    LOGF_D("presses: %d, buttonPressHandledTime: %d, buttonLongPressHandledTime: %d\n", 
        sizeof(presses), sizeof(buttonPressHandledTime), sizeof(buttonLongPressHandledTime));
    initButton(SCROLL_LEFT_BUTTON);
    initButton(ENTER_BUTTON);
    initButton(SCROLL_RIGHT_BUTTON);

    buttonTimer = timerBegin(1000000);
    timerAttachInterrupt(buttonTimer, &onButtonTimer); 
    timerAlarm(buttonTimer, 2000, true, 0);

    controllers[activeControllerIndex]->onResume();
}


void MainController::onButtonTimer() {
    int pin = SCROLL_RIGHT_BUTTON;
    size_t pinIndex = GET_BUTTON_INDEX(pin);
    bool pressed = digitalRead(pin) == LOW;
    uint32_t now = micros(); // high-resolution timestamp
    if (pressed) {
        portENTER_CRITICAL_ISR(&timerMux);
        if (presses[pinIndex][0] == 0) {
            presses[pinIndex][0] = now;  // first time pressed
        }
        presses[pinIndex][1] = now;        // update last seen press
        portEXIT_CRITICAL_ISR(&timerMux);
    }

    pin = ENTER_BUTTON;
    pinIndex = GET_BUTTON_INDEX(pin);
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
    
    pin = SCROLL_LEFT_BUTTON;
    pinIndex = GET_BUTTON_INDEX(pin);
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

void MainController::initButton(int pin){
    pinMode(pin, INPUT_PULLUP);
    presses[GET_BUTTON_INDEX(pin)][0]=0;
    presses[GET_BUTTON_INDEX(pin)][1]=0;
    buttonPressHandledTime[GET_BUTTON_INDEX(pin)]=0;
    buttonLongPressHandledTime[GET_BUTTON_INDEX(pin)]=0;
}


void MainController::loop(){
    if (isButtonLongPressed(SCROLL_RIGHT_BUTTON)){
        LOG_I("Scroll right button long pressed");
        controllers[activeControllerIndex]->handleRightButtonLongPress();
    } else if (isButtonLongPressed(SCROLL_LEFT_BUTTON)){
        LOG_I("Scroll left button long pressed");
        controllers[activeControllerIndex]->handleLeftButtonLongPress();
    } else if (isButtonLongPressed(ENTER_BUTTON)){
        LOG_I("Enter button long pressed");
        switchController();
    } else if (isButtonPressed(SCROLL_RIGHT_BUTTON)){
        LOG_I("Scroll right button pressed");
        controllers[activeControllerIndex]->handleRightButtonPress();
    } else if (isButtonPressed(SCROLL_LEFT_BUTTON)){
        LOG_I("Scroll left button pressed");
        controllers[activeControllerIndex]->handleLeftButtonPress();
    } else if (isButtonPressed(ENTER_BUTTON)){
        LOG_I("Enter button pressed");
        controllers[activeControllerIndex]->handleEnterButtonPress();
    } else {
        controllers[activeControllerIndex]->loop();
    }
    resetPress(SCROLL_RIGHT_BUTTON);
    resetPress(ENTER_BUTTON);
    resetPress(SCROLL_LEFT_BUTTON);
}

void MainController::resetPress(int pin) {
    size_t pinIndex = GET_BUTTON_INDEX(pin);
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
        LOGF_D("Pin: %d, reset: %d\n", pin, reset);
    }
}

bool MainController::isButtonPressed(int pin){
    size_t pinIndex = GET_BUTTON_INDEX(pin);
    portENTER_CRITICAL(&timerMux);
    uint32_t start = presses[pinIndex][0];
    uint32_t end   = presses[pinIndex][1];
    portEXIT_CRITICAL(&timerMux);
    
    
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

bool MainController::isButtonLongPressed(int pin){
    size_t pinIndex = GET_BUTTON_INDEX(pin);
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

void MainController::switchController(){
    LOG_I("Switching controller");
    controllers[activeControllerIndex]->onPause();
    LOGF_I("Paused controller %d\n", activeControllerIndex);

    activeControllerIndex = (activeControllerIndex + 1) % controllers.size();

    LOGF_I("Resuming controller %d\n", activeControllerIndex);
    controllers[activeControllerIndex]->onResume();
    LOG_I("Switched controller");
}