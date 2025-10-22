#include "tft_renderer.h"

#include "offsetting_draw_target.h"
#include "capturing_draw_target.h"
#include "pixels_holder.h"
#include "../../core/configs.h"
#define LOG_LEVEL 2
#include "../log.h"
#include <memory>


TFTRenderer::~TFTRenderer() {
    if(fileBytes){
        free(fileBytes);
        fileBytes = nullptr;
    }
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
        if (lastRenderedId != id) {
            size_t imgBytes = readImage(gifImage);

            tft.startWrite(); // The TFT chip select is locked low
            gifImage.getDecoder().decode(fileBytes, imgBytes, offsettingDrawTarget);
            lastRenderedId = id;
        } else {
            tft.startWrite(); // The TFT chip select is locked low
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
        size_t imgBytes = readImage(image);

        tft.startWrite(); // The TFT chip select is locked low
        image.getDecoder().decode(fileBytes, imgBytes, capturingDrawTarget);
        tft.endWrite(); // The TFT chip select is locked low

        auto captured = capturingDrawTarget.getCaptured();
        if (captured) {
            cache.put(image.filePath, std::move(captured));
            PixelsHolder &cached = cache.get(image.filePath);
            LOGF_D("Cached %s: %d (w:%d, h%d)\n",image.filePath.c_str(), id, cached.width, cached.height);
        } else {
            LOGF_W("TFTRenderer: failed to capture image %s, skipping cache\n", image.filePath.c_str());
        }
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
        size_t imgBytes = readImage(pngImage);

        tft.startWrite(); // The TFT chip select is locked low
        pngImage.getDecoder().decode(fileBytes, imgBytes, offsettingDrawTarget);
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

size_t TFTRenderer::readImage(const Image &image){
    if(!fileBytes){
        LOG_W("No memory for file bytes");
        return 0;
    }
    fs::File file = fileSys.open(image.filePath.c_str(), FILE_READ);
    if(!file){
        LOGF_W("Failed to open %s\n", image.filePath.c_str());
        return 0;
    }
    size_t readBytes = file.read(fileBytes, MAX_FILE_SIZE);
    if(readBytes == 0){
        LOGF_W("Failed to read from %s\n", image.filePath.c_str());
        file.close();
        return 0;
    }
    LOGF_D("Read %d bytes from %s\n", readBytes, image.filePath.c_str());
    file.close();
    return readBytes;
}


// iDrawTarget

void TFTRenderer::setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h){
    LOGF_T("Window addr (%d,%d), (%d,%d)\n", x,y,w,h);

    tft.setAddrWindow(x,y, w, h);
}

void TFTRenderer::pushPixels(const void *pixels, uint32_t count){
    LOGF_T("Pushing %d pixels: \n", count);
    if(LOG_LEVEL ==0 ){
        LOG_ARRAY("0x%x", static_cast<const uint16_t*>(pixels), count);
    }
    tft.pushPixels(pixels, count);
}    

