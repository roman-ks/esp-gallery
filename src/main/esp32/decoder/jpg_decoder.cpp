#include "jpg_decoder.h"
#define LOG_LEVEL 2
#include "../log.h"

JPGDecoder::JPGDecoder(DelegatingDrawTarget *firstDelegate, DelegatingDrawTarget *lastDelegate): Decoder(firstDelegate, lastDelegate){
    TJpgDec.setJpgScale(1);
    // TJpgDec.setSwapBytes(true);
    TJpgDec.setCallback(tftOutput);
}

void JPGDecoder::decode(uint8_t *fileBytes, size_t bytesCount, IDrawTarget &iDrawTarget){
    setIDrawTarget(wrapWithDelegates(&iDrawTarget));

    uint16_t w = 0, h = 0, scale;
    TJpgDec.getJpgSize(&w, &h, fileBytes, bytesCount);
    LOGF_D("Image w: %d, h %d\n", w, h);
    TJpgDec.drawJpg(0, 0, fileBytes, bytesCount);
}
        
bool JPGDecoder::tftOutput(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap){
    iDrawTarget->setAddrWindow(x, y, w, h);
    iDrawTarget->pushPixels(bitmap, w * h);
    return 1;
}