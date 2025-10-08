#pragma once

#include <cstdint>
#include "i_controller.h"
#include <vector>
#include <memory>
#include "Arduino.h"

#define BUTTON_COUNT 3

class MainController {
    public:
        MainController(std::vector<std::unique_ptr<IController>>&& controllers) 
            : controllers(std::move(controllers)) {};
        ~MainController();

        void init();
        void loop();  
    private:
        std::vector<std::unique_ptr<IController>> controllers;
        size_t activeControllerIndex = 0;
        // pairs (press-release)
        inline static volatile uint32_t presses[BUTTON_COUNT][2];
        volatile uint32_t buttonPressHandledTime[BUTTON_COUNT];
        volatile uint32_t buttonLongPressHandledTime[BUTTON_COUNT];
        inline static hw_timer_t *buttonTimer = nullptr;
        inline static portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

        bool isButtonPressed(int pin);
        bool isButtonLongPressed(int pin);

        void initButton(int pin);
        void resetPress(int pin);

        static void IRAM_ATTR onButtonTimer();

};