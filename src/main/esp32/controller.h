#ifndef __ESP_GALLERY_CONTROLLER_H__
#define __ESP_GALLERY_CONTROLLER_H__

#include <map>
#include "gallery.h"

#define SCROLL_LEFT_BUTTON 5
#define ENTER_BUTTON 4

class Controller {
    public:
        Controller(Gallery &gallery);
        ~Controller();

        void init();
        void loop();  
    private:
        Gallery &gallery;
        std::map<int, bool> buttonStates;
        
        void handleLeftButtonPress();
        void handleEnterButtonPress();
        bool isButtonPressed(int pin);

};

#endif