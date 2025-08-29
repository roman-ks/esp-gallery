#ifndef __ESP_GALLERY_CONTROLLER_H__
#define __ESP_GALLERY_CONTROLLER_H__

#include "gallery.h"

class Controller {
    public:
        Controller(Gallery &gallery);
        ~Controller();

        void init();
        void loop();  
    private:
        int oldButtonState=HIGH; // default to HIGH
        Gallery &gallery;
        bool isButtonPressed();

};

#endif