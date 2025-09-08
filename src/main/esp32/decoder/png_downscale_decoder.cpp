#include "png_downscale_decoder.h"
#include "../log.h"


PNGDownscaleDecoder::PNGDownscaleDecoder(){
}

PNGDownscaleDecoder::~PNGDownscaleDecoder(){
}

void PNGDownscaleDecoder::decode(const char *filepath, IDrawTarget &iDrawTarget) {
    LOG("Decoding with downscale");
    PNGDecoder::decode(filepath, iDrawTarget, &PNGDecoder::pngOpen, &PNGDecoder::pngClose,
        &PNGDecoder::pngRead, &PNGDecoder::pngSeek, &PNGDownscaleDecoder::pngDrawDownscaling);
}

int PNGDownscaleDecoder::pngDrawDownscaling(PNGDRAW *pDraw) {
    // use nearest neigbour algo, so read only one line per THUMBNAIL_HEIGHT_SCALE_FACTOR
    if(lineN == 0){
        uint16_t *buf= PNGDecoder::getLineBuffer();
        PNGDecoder::getPng().getLineAsRGB565(pDraw, buf, PNG_RGB565_BIG_ENDIAN, 0xffffffff);

        uint16_t downScaledWidth = 0;
        for(uint16_t i=0; i < pDraw->iWidth; i+=THUMBNAIL_WIDTH_SCALE_FACTOR){
            resultBuf[downScaledWidth++] = buf[i];
        }
        uint16_t y = pDraw->y / THUMBNAIL_HEIGHT_SCALE_FACTOR;
        getIDrawTarget().setAddrWindow(0, y, downScaledWidth, 1);
        getIDrawTarget().pushPixels(resultBuf, downScaledWidth);
        return 1;
    }

    lineN++;
    if(lineN == THUMBNAIL_HEIGHT_SCALE_FACTOR){
        lineN=0;
    }
    return 1;
}