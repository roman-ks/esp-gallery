#include "tft_renderer.h"

#include "offsetting_draw_target.h"
#include "../../core/configs.h"
#include "../log.h"


TFTRenderer::~TFTRenderer() {
}

void TFTRenderer::init() {
    tft.fillScreen(BG_COLOR);
}

void TFTRenderer::reset() {
    tft.fillScreen(BG_COLOR);
}

void TFTRenderer::render(const GIFImage &gifImage) {
    tft.startWrite(); // The TFT chip select is locked low

    gifImage.decoder.decode(gifImage.filePath, *this);
    tft.endWrite(); // The TFT chip select is locked low
}

void TFTRenderer::render(const PNGImage &pngImage) {
    static uint16_t lastRenderedId = 0;
    if(lastRenderedId == pngImage.id)
        return;
    OffsettingDrawTarget drawTarget(*this, pngImage.xPos, pngImage.yPos);
    tft.startWrite(); // The TFT chip select is locked low

    pngImage.decoder.decode(pngImage.filePath, drawTarget);
    tft.endWrite(); // The TFT chip select is locked low

    lastRenderedId = pngImage.id;
}

int TFTRenderer::drawImage(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *colors) {
    tft.pushImage(x, y, w, h, colors);
    return 1;
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

void TFTRenderer::setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h){
    // LOGF("Window addr (%d,%d), (%d,%d)\n", x,y,w,h);

    tft.setAddrWindow(x,y, w, h);
}

void TFTRenderer::pushPixels(const void *pixels, uint32_t count){
    // LOGF("Pushing pixels %d\n", count);

    tft.pushPixels(pixels, count);
}    

