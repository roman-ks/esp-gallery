#ifndef __ESP_GALLERY_TFT_RENDERER_H__
#define __ESP_GALLERY_TFT_RENDERER_H__

#include "renderer.h"
#include <TFT_eSPI.h>
#include <map>
#include "pixels_holder.h"

class TFTRenderer : public Renderer {
    public:
        TFTRenderer(TFT_eSPI &tft)
            : tft(tft), cache() {}
        ~TFTRenderer();

        void init() override;
        void reset() override;
        void render(const GIFImage &gifImage) override;
        void render(const PNGImage &pngImage) override;
        void renderBorder(uint16_t x, uint16_t y, uint16_t w, uint16_t h, 
                          uint16_t thickness, uint32_t color) override;

        void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) override;
        void pushPixels(const void *pixels, uint32_t count) override;

    private:
        TFT_eSPI &tft;
        std::map<uint16_t, std::unique_ptr<PixelsHolder>> cache;

        void renderCachable(IDrawTarget &delegate, const Image &image);


};
#endif