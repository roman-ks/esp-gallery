#ifndef __ESP_GALLERY_PNG_IMAGE_H__
#define __ESP_GALLERY_PNG_IMAGE_H__

#include "image.h"

class PNGImage : public Image {
    public:
        PNGImage(Decoder &decoder, std::string filePath) 
            : Image(decoder, filePath) {}
        PNGImage(Decoder &decoder, std::string filePath, bool cachable) 
            : Image(decoder, filePath, cachable) {}
        ~PNGImage()=default;

        void render(Renderer &rendered) const;
};

#endif