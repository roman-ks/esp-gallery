#include "tft_renderer.h"
#include "../../core/configs.h"
#include "../log.h"


TFTRenderer::~TFTRenderer() {
}

void TFTRenderer::init() {
    tft.fillScreen(TFT_BLACK);
}

void TFTRenderer::reset() {
    tft.fillScreen(TFT_BLACK);
}

// void TFTRenderer::render(const GIFImage &gifImage) {
//     // Implementation for rendering GIF images
//     // This is a placeholder; actual implementation would depend on the GIF library used
// }

void TFTRenderer::render(const PNGImage &pngImage) {
    static uint16_t lastRenderedId = 0;
    if(lastRenderedId == pngImage.id)
        return;

    std::function<int(uint16_t, uint16_t, uint16_t, uint16_t*)> drawPngCallback = 
        [this, pngImage](uint16_t y, int16_t w, int16_t h, uint16_t* colors){
            return this->drawImage(pngImage.xPos, pngImage.yPos + y, w, h, colors);
    };
    pngImage.decoder.decode(pngImage.filePath, drawPngCallback);
    lastRenderedId = pngImage.id;
}


int TFTRenderer::drawImage(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *colors) {
    tft.pushImage(x, y, w, h, colors);
    return 1;
}

