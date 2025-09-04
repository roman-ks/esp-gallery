#ifndef __ESP_GALLERY_IMAGE_H__
#define __ESP_GALLERY_IMAGE_H__

#include <cstdint>
#include "../renderer/renderer.h"
#include "../decoder/decoder.h"


class Image {
    public:
        Image(Decoder &decoder, char* filePath) 
            : decoder(decoder), filePath(filePath) {}
        virtual void render(Renderer &rendered) const = 0;

        void setPosition(uint16_t x, uint16_t y) { 
            xPos = x; yPos = y; 
        }
        // // todo make final?
        Decoder& decoder;
        char* filePath;
        uint16_t xPos = 0;
        uint16_t yPos = 0;
};
#endif