#include "controller.h"
#include "log.h"

// in micros
#define SHORT_PRESS_THRESHOLD 100000
#define LONG_PRESS_THRESHOLD 1000000
#define LONG_PRESS_INTERVAL 1000000
#define DEBOUNCE_DURATION 10000


Controller::~Controller() {
    detachInterrupt(SCROLL_RIGHT_BUTTON);
    detachInterrupt(ENTER_BUTTON);
}

void Controller::init(){
    // initNavButton(SCROLL_RIGHT_BUTTON, rightFallingISR, rightRisingISR);

    int pin = SCROLL_RIGHT_BUTTON;
    pinMode(pin, INPUT_PULLUP);
    buttonStates[pin] = 0;
    buttonFallingTime[pin] = 0;
    buttonRisingTime[pin] = 0;
    presses[pin] = std::vector<std::pair<uint64_t, uint64_t>>(PRESSES_HISTORY);
    for(int i = 0; i< PRESSES_HISTORY; ++i){
        presses[pin][i] = std::pair(0,0);
    }
    pressIndexes[pin] = 0;
    attachInterrupt(pin, rightChangeISR, CHANGE);
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
    int pin = SCROLL_RIGHT_BUTTON;
    int signal = digitalRead(pin);
    int index = pressIndexes[pin];
    if(signal == LOW){
        // LOW means pressed
        presses[pin][index].first = nowMicros();
        buttonFallingTime[pin] = nowMicros();
    } else {
        presses[pin][index].second = nowMicros();
        pressIndexes[pin]=(index+1) % PRESSES_HISTORY;
        buttonRisingTime[pin] = nowMicros();
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
    auto& history = presses[pin];
    size_t pressIndex = wrap(pressIndexes[pin]-1, PRESSES_HISTORY);

    uint64_t now = nowMicros();

    // Walk backwards through press history
    for(int i = 0; i < PRESSES_HISTORY; ++i){
        auto pressPair = history[wrap(pressIndex-i, PRESSES_HISTORY)];
        if(pressPair.first == 0 || pressPair.second == 0) {
            // Empty entry
            continue;
        }

        uint64_t dur = pressPair.second - pressPair.first;
        if(dur > SHORT_PRESS_THRESHOLD && dur < LONG_PRESS_THRESHOLD){
            // Only trigger once per rising edge
            if(pressPair.second > buttonPressHandledTime[pin]){
                buttonPressHandledTime[pin] = pressPair.second; // mark handled
                return true;
            }
        }
    }

    return false;
}

// bool Controller::isButtonPressed(int pin){
//     auto history = presses[pin];
//     size_t pressIndex = wrap(pressIndexes[pin]-1, PRESSES_HISTORY);
//     if(history[pressIndex].first==0||history[pressIndex].second==0){
//         pressIndex = wrap(pressIndex-1, PRESSES_HISTORY);
//     }
//     if(history[pressIndex].first==0||history[pressIndex].second==0){
//         // previous also 0, no presses yet
//         return false;
//     }

//     LOGF_D("Pin: %d, i: %d, history: ", pin, pressIndex);
//     for(int i = 0;i<PRESSES_HISTORY;++i){
//         size_t ind = i;
//         auto pressPair = history[ind];
//         Serial.printf("%d: %llu-%llu ", ind, pressPair.first, pressPair.second);
//     }
//     Serial.println();
//     uint64_t now = nowMicros();

//     // find max press duration over DEBOUNCE_DURATION
//     uint64_t maxDur = 0;
//     uint64_t maxRising = 0;
//     for(int i = 0;i<PRESSES_HISTORY;++i){
//         auto pressPair = history[wrap(pressIndex-i, PRESSES_HISTORY)];
//         if(pressPair.second < buttonPressHandledTime[pin]){
//             // LOGF_D("Pin: %d, Last raise(%llu) was too long ago: %llu\n", 
//             //     pin, pressPair.second, now-pressPair.second);
//             // return false;
//             // reached already handled pair
//             break;
//         }
//         uint64_t dur = pressPair.second - pressPair.first;
//         LOGF_D("Pin: %d, dur: %llu, > SHORT_PRESS_THRESHOLD: %d, < LONG_PRESS_THRESHOLD: %d\n", 
//             pin, dur, dur > SHORT_PRESS_THRESHOLD, dur < LONG_PRESS_THRESHOLD);
//         if(dur > SHORT_PRESS_THRESHOLD && dur < LONG_PRESS_THRESHOLD){
//             // if(buttonPressHandledTime[pin]!=maxRising){
//                 // LOGF_D("Pin: %d, Registering press\n", pin);
//                 // buttonPressHandledTime[pin]=maxRising;
//                 return true;
//             // }
//         }
//         // if(dur>maxDur){
//         //     maxDur = dur;
//         //     maxRising = pressPair.second;
//         // }
//     }

//     // LOGF_D("Pin: %d, maxDur: %llu, > SHORT_PRESS_THRESHOLD: %d, < LONG_PRESS_THRESHOLD: %d\n", 
//     //     pin, maxDur, maxDur > SHORT_PRESS_THRESHOLD, maxDur < LONG_PRESS_THRESHOLD);
//     // if(maxDur > SHORT_PRESS_THRESHOLD 
//     //     && maxDur < LONG_PRESS_THRESHOLD){
//     //         // LOGF_D("Pin: %d, Duration ok\n", pin);
//     //     if(buttonPressHandledTime[pin]!=maxRising){
//     //             // LOGF_D("Pin: %d, Registering press\n", pin);
//     //         buttonPressHandledTime[pin]=maxRising;
//     //         return true;
//     //     }
//     // }
//     return false;
// }

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
