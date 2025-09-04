#ifndef _ESP_GALLERY_RENDERER_H__
#define _ESP_GALLERY_RENDERER_H__

// #include "../image/png_image.h"
// #include "../image/gif_image.h"

// forward declaration to break circular dependency
// class PNGImage;

class Renderer {
    public:
        ~Renderer() = default;

        virtual void init();
        virtual void reset();
        // virtual void render(const GIFImage &gifImage);
        // virtual void render(const PNGImage &pngImage);
    private:
};

#endif