#include "png_downscale_decoder.h"
#include "../log.h"


PNGDownscaleDecoder::PNGDownscaleDecoder(){
}

PNGDownscaleDecoder::~PNGDownscaleDecoder(){   
}

void PNGDownscaleDecoder::decode(char* filepath, DrawCallbackFunc &drawCallback) {
    LOG("Decoding with downscale");
    PNGDecoder::decode(filepath, drawCallback, &PNGDecoder::pngOpen, &PNGDecoder::pngClose, 
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
        if (PNGDecoder::getDrawCallback()) {
            uint16_t y = pDraw->y/THUMBNAIL_HEIGHT_SCALE_FACTOR;
            return PNGDecoder::getDrawCallback()(y, 
                downScaledWidth, 1, PNGDownscaleDecoder::resultBuf);
        }
    }

    lineN++;
    if(lineN == THUMBNAIL_HEIGHT_SCALE_FACTOR){
        lineN=0;
    }
    return 1;
}