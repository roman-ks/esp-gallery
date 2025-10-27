#ifndef __ESP_GALLERY_TFT_RENDERER_H__
#define __ESP_GALLERY_TFT_RENDERER_H__

#include "renderer.h"
#include <TFT_eSPI.h>
#include <map>
#include "pixels_holder.h"
#include "renderer_cache.h"
#define MAX_FILE_SIZE (256*1024) // 256KB

class TFTRenderer : public Renderer {
    public:
        TFTRenderer(TFT_eSPI &tft, RendererCache &cache, fs::FS &fileSys)
            : tft(tft), cache(cache), fileSys(fileSys) {
                fileBytes = static_cast<uint8_t*>(ps_malloc(MAX_FILE_SIZE * sizeof(uint8_t)));
        }
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
        RendererCache &cache;
        fs::FS &fileSys;
        uint8_t *fileBytes = nullptr;
        uint16_t lastRenderedId = 0;

        void renderCachable(IDrawTarget &delegate, const Image &image);
        size_t readImage(const Image &image);


};
#endif