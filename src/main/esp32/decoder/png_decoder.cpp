#include "png_decoder.h"
#include "../../core/configs.h"
#include <LittleFS.h>
#define LOG_LEVEL 2
#include "../log.h"

void PNGDecoder::init() {
    // Initialize PNG decoder if needed
}

void PNGDecoder::decode(const char* filepath, IDrawTarget &iDrawTarget) {
    decode(filepath, iDrawTarget, &PNGDecoder::pngOpen, &PNGDecoder::pngClose,
        &PNGDecoder::pngRead, &PNGDecoder::pngSeek, &PNGDecoder::pngDraw);
}

void PNGDecoder::decode(const char* filepath, IDrawTarget &iDrawTarget,
                        PNG_OPEN_CALLBACK *openCB, PNG_CLOSE_CALLBACK *closeCB,
                        PNG_READ_CALLBACK *readCB, PNG_SEEK_CALLBACK *seekCB, PNG_DRAW_CALLBACK *drawCB) {
    PNG &png = PNGDecoder::getPng();
    png.close();
    setIDrawTarget(wrapWithDelegates(&iDrawTarget));
    LOG_D("Opening image..");
    int16_t rc = png.open(filepath, openCB, closeCB, readCB, seekCB, drawCB);
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




void * PNGDecoder::pngOpen(const char *filename, int32_t *size) {
  LOGF_D("Attempting to open %s\n", filename);
  PNGDecoder::file = std::make_unique<File>(LittleFS.open(filename, "r"));
  *size = PNGDecoder::file->size();
  LOGF_D("File size: %d\n", *size);
  return &PNGDecoder::file;
}

void PNGDecoder::pngClose(void *handle) {
  if (PNGDecoder::file) {
    PNGDecoder::file->close();
    PNGDecoder::file = nullptr;
  }
}

int32_t PNGDecoder::pngRead(PNGFILE *page, uint8_t *buffer, int32_t length) {
  LOG_D("pngRead");
  if (!PNGDecoder::file) return 0;
  page = page; // Avoid warning
  return PNGDecoder::file->read(buffer, length);
}

int32_t PNGDecoder::pngSeek(PNGFILE *page, int32_t position) {
  LOG_D("pngSeek");
  if (!PNGDecoder::file) return 0;
  page = page; // Avoid warning
  return PNGDecoder::file->seek(position);
}

int PNGDecoder::pngDraw(PNGDRAW *pDraw) {
    PNG &png = PNGDecoder::getPng();
    png.getLineAsRGB565(pDraw, PNGDecoder::getLineBuffer(), PNG_RGB565_BIG_ENDIAN, 0xffffffff);

    iDrawTarget->setAddrWindow(0, pDraw->y, pDraw->iWidth, 1);
    iDrawTarget->pushPixels(getLineBuffer(), pDraw->iWidth);
    return 1;
}

