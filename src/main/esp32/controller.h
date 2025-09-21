#ifndef __ESP_GALLERY_CONTROLLER_H__
#define __ESP_GALLERY_CONTROLLER_H__

#include <map>
#include "gallery.h"
#include <atomic>

#define SCROLL_RIGHT_BUTTON 5
#define ENTER_BUTTON 4

class Controller {
    public:
        Controller(Gallery &gallery) : gallery(gallery), 
            buttonPressHandledTime(), buttonLongPressHandledTime(){}
        ~Controller();

        void init();
        void loop();  
    private:
        Gallery &gallery;
        inline static std::map<int, bool> buttonPressed;
        inline static std::map<int, uint64_t> buttonRisingTime;
        inline static std::map<int, uint64_t> buttonFallingTime;
        inline static std::map<int, std::atomic<int>> buttonStates;
        std::map<int, uint64_t> buttonPressHandledTime;
        std::map<int, uint64_t> buttonLongPressHandledTime;

        
        void handleRightButtonPress();
        void handleRightButtonLongPress();

        void handleEnterButtonPress();

        bool isButtonPressed(int pin);
        bool isButtonLongPressed(int pin);

        void initButton(int pin, void isrFunc(void));
        void initNavButton(int pin, void isrFallingFunc(void), void isrRisingFunc(void));

        static void IRAM_ATTR rightFallingISR();
        static void IRAM_ATTR rightRisingISR();
        static void IRAM_ATTR rightChangeISR();

        // static void IRAM_ATTR enterISR();
        static void IRAM_ATTR handleISR(int pin);

        static uint64_t IRAM_ATTR nowMicros();
};

#endif