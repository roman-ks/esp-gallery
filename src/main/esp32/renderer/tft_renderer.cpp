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

void TFTRenderer::render(const GIFImage &gifImage) {
    // Implementation for rendering GIF images
    // This is a placeholder; actual implementation would depend on the GIF library used
}

void TFTRenderer::render(const PNGImage &pngImage) {
    if(staticImgDrawn) return; // Draw static image once
    static PNG *png = new PNG();

    std::function<int(void *)> drawPngCallback = [this, &png, pngImage](void *p){
       return this->drawPng(png, pngImage, static_cast<PNGDRAW*>(p));
    };

    pngImage.decoder.decode(pngImage.filePath, drawPngCallback, 
        static_cast<void*>(png));
    staticImgDrawn = true;
}


int TFTRenderer::drawPng(PNG *png, const PNGImage &pngImage, PNGDRAW *pDraw) {
    uint16_t lineBuffer[MAX_IMAGE_WIDTH];
    png->getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
    tft.pushImage(pngImage.xPos, pngImage.yPos + pDraw->y, pDraw->iWidth, 1, lineBuffer);
    return 1;
}

