#include "png_decoder.h"
#include "../../core/configs.h"
#define LOG_LEVEL 2
#include "../log.h"

void PNGDecoder::decode(uint8_t *fileBytes, size_t bytesCount, IDrawTarget &iDrawTarget) {
    PNG &png = PNGDecoder::getPng();
    png.close();
    setIDrawTarget(wrapWithDelegates(&iDrawTarget));
    LOG_D("Opening image..");
    int16_t rc = png.openRAM(fileBytes, bytesCount, &PNGDecoder::pngDraw);
    if (rc == PNG_SUCCESS) {
        LOGF_D("image specs: (%d x %d), %d bpp, pixel type: %d\n", 
            png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
        uint32_t dt = millis();
        if (png.getWidth() > MAX_IMAGE_WIDTH) {
          LOG_I("Image too wide for allocated line buffer size!");
        }
        else {
          rc = png.decode(NULL, 0);
          png.close();
        }
        // How long did rendering take...
        LOGF_D("Decoded in %dms\n", millis()-dt);
    }
}

int PNGDecoder::pngDraw(PNGDRAW *pDraw) {
    PNG &png = PNGDecoder::getPng();
    png.getLineAsRGB565(pDraw, PNGDecoder::getLineBuffer(), PNG_RGB565_BIG_ENDIAN, 0xffffffff);

    iDrawTarget->setAddrWindow(0, pDraw->y, pDraw->iWidth, 1);
    iDrawTarget->pushPixels(getLineBuffer(), pDraw->iWidth);
    return 1;
}

