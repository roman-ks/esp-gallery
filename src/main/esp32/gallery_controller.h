#ifndef __ESP_GALLERY_CONTROLLER_H__
#define __ESP_GALLERY_CONTROLLER_H__

#include "gallery.h"
#include "i_controller.h"


class GalleryController : public IController {
    public:
        GalleryController(Gallery &gallery) : gallery(gallery){}
        ~GalleryController();

        void onPause() override {};
        void onResume() override {};

        void loop() override;  

        void handleRightButtonPress() override;
        void handleRightButtonLongPress() override;

        void handleEnterButtonPress() override;

        void handleLeftButtonPress() override;
        void handleLeftButtonLongPress() override;
    private:
        Gallery &gallery;

};

#endif