#ifndef __ESP_GALLERY_GIF_IMAGE_H__
#define __ESP_GALLERY_GIF_IMAGE_H__

#include "image.h"
#include "../decoder/advancable_decoder.h"

class GIFImage : public Image {
    public:
        GIFImage(AdvancableDecoder &decoder, std::string filePath): 
            Image(decoder, filePath){}
        GIFImage(AdvancableDecoder &decoder, std::string filePath, bool cachable): 
            Image(decoder, filePath, cachable){}
        ~GIFImage() = default;

        void render(Renderer &rendered) const;
        AdvancableDecoder& getDecoder() const {
            return static_cast<AdvancableDecoder&>(decoder);
        }
};

#endif