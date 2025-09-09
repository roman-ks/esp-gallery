#ifndef __ESP_GALLERY_TFT_RENDERER_H__
#define __ESP_GALLERY_TFT_RENDERER_H__

#include "renderer.h"
#include <TFT_eSPI.h>

class TFTRenderer : public Renderer {
    public:
        TFTRenderer(TFT_eSPI &tft)
            : tft(tft) {}
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

        int drawImage(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *colors);


};
#endif