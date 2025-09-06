#include "png_decoder.h"
#include "../../core/configs.h"
#include <LittleFS.h>
#include "../log.h"

PNGDecoder::PNGDecoder() {
}

PNGDecoder::~PNGDecoder() {
  PNGDecoder::s_drawCallback = nullptr;
}
void PNGDecoder::init() {
    // Initialize PNG decoder if needed
}

void PNGDecoder::decode(char* filepath, DrawCallbackFunc drawCallback) {

    PNGDecoder::png.close();
    PNGDecoder::s_drawCallback = drawCallback;
    LOG("Opening image..");
    int16_t rc = PNGDecoder::png.open(filepath, &PNGDecoder::pngOpen, &PNGDecoder::pngClose, 
        &PNGDecoder::pngRead, &PNGDecoder::pngSeek, &PNGDecoder::pngDraw);
    if (rc == PNG_SUCCESS) {
        LOGF("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
        uint32_t dt = millis();
        if (PNGDecoder::png.getWidth() > MAX_IMAGE_WIDTH) {
          LOG("Image too wide for allocated line buffer size!");
        }
        else {
          rc = PNGDecoder::png.decode(NULL, 0);
          PNGDecoder::png.close();
        }
        // How long did rendering take...
        LOGF("Decoded in %dms", millis()-dt);
    }
    s_drawCallback = nullptr;
}

void * PNGDecoder::pngOpen(const char *filename, int32_t *size) {
  LOGF("Attempting to open %s\n", filename);
  PNGDecoder::file = std::make_unique<File>(LittleFS.open(filename, "r"));
  *size = PNGDecoder::file->size();
  LOGF("File size: %d\n", *size);
  return &PNGDecoder::file;
}

void PNGDecoder::pngClose(void *handle) {
  if (PNGDecoder::file) {
    PNGDecoder::file->close();
    PNGDecoder::file = nullptr;
  }
}

int32_t PNGDecoder::pngRead(PNGFILE *page, uint8_t *buffer, int32_t length) {
  LOG("pngRead");
  if (!PNGDecoder::file) return 0;
  page = page; // Avoid warning
  return PNGDecoder::file->read(buffer, length);
}

int32_t PNGDecoder::pngSeek(PNGFILE *page, int32_t position) {
  LOG("pngSeek");
  if (!PNGDecoder::file) return 0;
  page = page; // Avoid warning
  return PNGDecoder::file->seek(position);
}

// wrapper to have function delegeate to std::function and erase param type
int PNGDecoder::pngDraw(PNGDRAW *pDraw) {
    PNGDecoder::png.getLineAsRGB565(pDraw, PNGDecoder::lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);

//   if (!file) return 0;
  if (PNGDecoder::s_drawCallback) {
      return PNGDecoder::s_drawCallback(pDraw->y, pDraw->iWidth, 1, PNGDecoder::lineBuffer);
  }
  return 0;
}

