#ifndef __ESP_GALLERY_CONTROLLER_H__
#define __ESP_GALLERY_CONTROLLER_H__

#include "gallery.h"

#define SCROLL_RIGHT_BUTTON 5
#define ENTER_BUTTON 4
#define BUTTON_INDEX_OFFSET 4
#define BUTTON_COUNT 2
// #define PRESSES_HISTORY 20

class Controller {
    public:
        Controller(Gallery &gallery) : gallery(gallery){}
        ~Controller();

        void init();
        void loop();  
    private:
        Gallery &gallery;
        // pairs (press-release)
        inline static volatile uint64_t presses[BUTTON_COUNT][2];
        volatile uint64_t buttonPressHandledTime[BUTTON_COUNT];
        volatile uint64_t buttonLongPressHandledTime[BUTTON_COUNT];
        inline static hw_timer_t *buttonTimer = nullptr;
        inline static portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
        inline static volatile uint16_t timerCount = 0;

        
        void handleRightButtonPress();
        void handleRightButtonLongPress();

        void handleEnterButtonPress();

        bool isButtonPressed(int pin);
        bool isButtonLongPressed(int pin);

        void initButton(int pin, void isrFunc(void));
        void initNavButton(int pin, void isrFallingFunc(void), void isrRisingFunc(void));
        void resetPress(int pin);

        static uint64_t IRAM_ATTR nowMicros();

        template<typename T>
        static T IRAM_ATTR wrap(T value, T size){
            T ret = value % size;
            if( ret >= 0)
                return ret;
            return ret + size;
        }

        static void IRAM_ATTR onButtonTimer();
};

#endif