#include "tft_renderer.h"
#include "../../core/configs.h"


TFTRenderer::~TFTRenderer() {
}

void TFTRenderer::init() {
    tft.fillScreen(TFT_BLACK);
}

void TFTRenderer::reset() {
    tft.fillScreen(TFT_BLACK);
    staticImgDrawn = false;
}

// void TFTRenderer::render(const GIFImage &gifImage) {
//     // Implementation for rendering GIF images
//     // This is a placeholder; actual implementation would depend on the GIF library used
// }

void TFTRenderer::render(const PNGImage &pngImage) {
    if(staticImgDrawn) return; // Draw static image once

    std::function<int(uint16_t, uint16_t, uint16_t, uint16_t*)> drawPngCallback = 
        [this, pngImage](uint16_t y, int16_t w, int16_t h, uint16_t* colors){
            return this->drawImage(pngImage.xPos, pngImage.yPos + y, w, h, colors);
    };

    pngImage.decoder.decode(pngImage.filePath, drawPngCallback);
    staticImgDrawn = true;
}


// int TFTRenderer::drawPng(PNG *png, const PNGImage &pngImage, PNGDRAW *pDraw) {
//     // uint16_t lineBuffer[MAX_IMAGE_WIDTH];
//     // png->getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
//     tft.pushImage(pngImage.xPos, pngImage.yPos + pDraw->y, pDraw->iWidth, 1, lineBuffer);
//     return 1;
// }

int TFTRenderer::drawImage(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *colors) {
    tft.pushImage(x, y, w, h, colors);
    return 1;
}

