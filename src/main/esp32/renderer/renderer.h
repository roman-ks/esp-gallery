#ifndef _ESP_GALLERY_RENDERER_H__
#define _ESP_GALLERY_RENDERER_H__

#include "../image/png_image.h"
#include "../image/gif_image.h"

class Renderer {
    public:
        virtual ~Renderer();

        virtual void init() = 0;
        virtual void reset() = 0;
        // virtual void render(const GIFImage &gifImage);
        virtual void render(const PNGImage &pngImage) = 0;
    private:
};


inline Renderer::~Renderer(){};

#endif