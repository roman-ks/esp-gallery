#ifndef __ESP_GALLERY_CONTROLLER_H__
#define __ESP_GALLERY_CONTROLLER_H__

#include "gallery.h"

#define BUTTON_COUNT 2

class Controller {
    public:
        Controller(Gallery &gallery) : gallery(gallery){}
        ~Controller();

        void init();
        void loop();  
    private:
        Gallery &gallery;
        // pairs (press-release)
        inline static volatile uint32_t enterStart = 0;
        inline static volatile uint32_t enterEnd = 0;
        inline static volatile uint32_t scrollRightStart = 0;
        inline static volatile uint32_t scrollRightEnd = 0;
        // inline static volatile uint32_t presses[BUTTON_COUNT][2];
        volatile uint32_t buttonPressHandledTime[BUTTON_COUNT];
        volatile uint32_t buttonLongPressHandledTime[BUTTON_COUNT];
        inline static hw_timer_t *buttonTimer = nullptr;
        inline static portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
        inline static volatile uint16_t timerCount = 0;

        
        void handleRightButtonPress();
        void handleRightButtonLongPress();

        void handleEnterButtonPress();

        bool isScrollRightPressed();
        bool isEnterPressed();
        bool isButtonLongPressed(int pin);

        void initButton(int pin);
        void resetPressScrollLeft();
        void resetPressEnter();


        static void IRAM_ATTR onButtonTimer();
};

#endif