#include "tft_renderer.h"

#include "offsetting_draw_target.h"
#include "capturing_draw_target.h"
#include "pixels_holder.h"
#include "../../core/configs.h"
#define LOG_LEVEL 2
#include "../log.h"
#include <memory>


TFTRenderer::~TFTRenderer() {
}

void TFTRenderer::init() {
    tft.fillScreen(BG_COLOR);
}

void TFTRenderer::reset() {
    tft.fillScreen(BG_COLOR);
}

void TFTRenderer::render(const GIFImage &gifImage) {
    static uint16_t lastRenderedId = 0;
    OffsettingDrawTarget offsettingDrawTarget(this, gifImage.xPos, gifImage.yPos);

    uint16_t id = gifImage.id;
    if (gifImage.cachable) {
        renderCachable(offsettingDrawTarget, gifImage);
    } else {
        LOGF_D("Not cachable %s: %d\n",gifImage.filePath, id);
        tft.startWrite(); // The TFT chip select is locked low
        if (lastRenderedId != id) {
            gifImage.getDecoder().decode(gifImage.filePath, offsettingDrawTarget);
            lastRenderedId = id;
        } else {
            gifImage.getDecoder().advance(offsettingDrawTarget);
        }
        tft.endWrite(); // The TFT chip select is locked low
    }
}

void TFTRenderer::renderCachable(IDrawTarget &delegate, const Image &image){
    uint16_t id = image.id;
    if (cache.exists(image.filePath)) {
        PixelsHolder &cached = cache.get(image.filePath);
        LOGF_D("Using cached%s: %d(w:%d, h%d, count: %d), \n",image.filePath, id, cached.width, cached.height, cached.pixels.size());
        
        tft.startWrite(); // The TFT chip select is locked low
        delegate.setAddrWindow(0,0, cached.width, cached.height);
        auto pixels = cached.pixels;
        delegate.pushPixels(pixels.data(), pixels.size());
        tft.endWrite(); // The TFT chip select is locked low
    } else {
        CapturingDrawTarget capturingDrawTarget(&delegate);
         tft.startWrite(); // The TFT chip select is locked low
        image.getDecoder().decode(image.filePath, capturingDrawTarget);
        tft.endWrite(); // The TFT chip select is locked low

        cache.put(image.filePath, std::move(capturingDrawTarget.getCaptured()));
        PixelsHolder &cached = cache.get(image.filePath);
        LOGF_D("Cached %s: %d (w:%d, h%d)\n",image.filePath, id, cached.width, cached.height);
    }
}

void TFTRenderer::render(const PNGImage &pngImage) {
    static uint16_t lastRenderedId = 0;
    if(lastRenderedId == pngImage.id)
        return;
    OffsettingDrawTarget offsettingDrawTarget(this, pngImage.xPos, pngImage.yPos);

    if (pngImage.cachable) {
        renderCachable(offsettingDrawTarget, pngImage);
    } else {
        tft.startWrite(); // The TFT chip select is locked low
        pngImage.getDecoder().decode(pngImage.filePath, offsettingDrawTarget);
        tft.endWrite(); // The TFT chip select is locked low
    }

    lastRenderedId = pngImage.id;
}

void TFTRenderer::renderBorder(uint16_t x, uint16_t y, uint16_t w, uint16_t h, 
                          uint16_t thickness, uint32_t color){

    // vertical lines
  tft.fillRect(x, y, thickness, h, color);  
  tft.fillRect(x+w-thickness, y, thickness, h, color);
  // horizontal lines
  tft.fillRect(x+thickness, y, w-2*thickness, thickness, color);  
  tft.fillRect(x+thickness, y+h-thickness, w-2*thickness, thickness, color);

}


// iDrawTarget

void TFTRenderer::setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h){
    LOGF_D("Window addr (%d,%d), (%d,%d)\n", x,y,w,h);

    tft.setAddrWindow(x,y, w, h);
}

void TFTRenderer::pushPixels(const void *pixels, uint32_t count){
    LOGF_D("Pushing pixels %d\n", count);

    tft.pushPixels(pixels, count);
}    

