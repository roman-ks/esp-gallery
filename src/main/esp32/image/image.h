#ifndef __ESP_GALLERY_IMAGE_H__
#define __ESP_GALLERY_IMAGE_H__

#include <cstdint>
#include "../decoder/decoder.h"
#include <string>

// forward declare to break header circular dependency
class Renderer;

class Image {
    public:
        Image(Decoder &decoder, std::string filePath) 
            : Image(decoder, filePath, false) {}
        Image(Decoder &decoder, std::string filePath, bool cachable): 
            decoder(decoder), filePath(filePath), id(instanceCounter++),cachable(cachable) {} 
        virtual void render(Renderer &rendered) const = 0;

        virtual Decoder& getDecoder() const {
            return decoder;
        }

        void setPosition(uint16_t x, uint16_t y) { 
            xPos = x; yPos = y; 
        }
        const std::string filePath;
        uint16_t xPos = 0;
        uint16_t yPos = 0;
        const uint16_t id;
        const bool cachable;
    protected:
        Decoder& decoder;
    private:
        inline static uint16_t instanceCounter = 0;

};
#endif