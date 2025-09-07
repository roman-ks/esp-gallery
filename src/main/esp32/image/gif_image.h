#ifndef __ESP_GALLERY_GIF_IMAGE_H__
#define __ESP_GALLERY_GIF_IMAGE_H__

#include "image.h"

class GIFImage : public Image {
    public:
        GIFImage(Decoder &decoder, char* filePath): 
            Image(decoder, filePath){}
        ~GIFImage() = default;

        void render(Renderer &rendered) const;
    private:
};

#endif