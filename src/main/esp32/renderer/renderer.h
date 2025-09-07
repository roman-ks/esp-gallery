#ifndef _ESP_GALLERY_RENDERER_H__
#define _ESP_GALLERY_RENDERER_H__

#include "../image/png_image.h"
#include "../image/gif_image.h"

class Renderer {
    public:
        virtual ~Renderer();

        virtual void init() = 0;
        virtual void reset() = 0;
        virtual void render(const GIFImage &gifImage) = 0;
        virtual void render(const PNGImage &pngImage) = 0;
        virtual void renderBorder(uint16_t x, uint16_t y, uint16_t w, uint16_t h, 
                                  uint16_t thickness, uint32_t color);
    private:
};


inline Renderer::~Renderer(){};

#endif