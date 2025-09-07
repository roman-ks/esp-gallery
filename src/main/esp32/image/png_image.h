#ifndef __ESP_GALLERY_PNG_IMAGE_H__
#define __ESP_GALLERY_PNG_IMAGE_H__

#include "image.h"

class PNGImage : public Image {
    public:
        const uint16_t id;

        PNGImage(Decoder &decoder, char* filePath) 
            : Image(decoder, filePath), id(instanceCounter++) {}
        ~PNGImage()=default;

        void render(Renderer &rendered) const;

    private:
        inline static uint16_t instanceCounter = 0;
};

#endif