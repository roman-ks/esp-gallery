#ifndef __ESP_GALLERY_PNG_IMAGE_H__
#define __ESP_GALLERY_PNG_IMAGE_H__

#include "image.h"

class PNGImage : public Image {
    public:
        PNGImage(Decoder &decoder, char* filePath) 
            : Image(decoder, filePath) {}
        ~PNGImage()=default;

        void render(Renderer &rendered) const;
    private:
};

#endif