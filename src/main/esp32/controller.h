#ifndef __ESP_GALLERY_CONTROLLER_H__
#define __ESP_GALLERY_CONTROLLER_H__

#include <map>
#include "gallery.h"
#include <atomic>

#define SCROLL_LEFT_BUTTON 5
#define ENTER_BUTTON 4

class Controller {
    public:
        Controller(Gallery &gallery) : gallery(gallery){}
        ~Controller();

        void init();
        void loop();  
    private:
        Gallery &gallery;
        inline static std::map<int, std::atomic<int>> buttonStates;
        inline static std::map<int, uint32_t> lastButtonPresses;
        
        void handleLeftButtonPress();
        void handleEnterButtonPress();
        bool isButtonPressed(int pin);
        void initButton(int pin, void isrFunc(void));

        static void IRAM_ATTR leftISR();
        static void IRAM_ATTR enterISR();
        static void IRAM_ATTR handleISR(int pin);
};

#endif