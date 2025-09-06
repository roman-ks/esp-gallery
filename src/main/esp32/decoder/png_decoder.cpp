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

void PNGDecoder::decode(char* filepath, std::function<int(void*)> drawCallback, void* context) {

    PNG &png = *static_cast<PNG*>(context);
    PNGDecoder::s_drawCallback = drawCallback;
    LOG("Opening image..");
    int16_t rc = png.open(filepath, &PNGDecoder::pngOpen, &PNGDecoder::pngClose, 
        &PNGDecoder::pngRead, &PNGDecoder::pngSeek, &PNGDecoder::pngDraw);
    if (rc == PNG_SUCCESS) {
        LOGF("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
        uint32_t dt = millis();
        if (png.getWidth() > MAX_IMAGE_WIDTH) {
          LOG("Image too wide for allocated line buffer size!");
        }
        else {
          rc = png.decode(NULL, 0);
          png.close();
        }
        // How long did rendering take...
        LOGF("Decoded in %dms", millis()-dt);
    }
    s_drawCallback = nullptr;
}

void * PNGDecoder::pngOpen(const char *filename, int32_t *size) {
  LOGF("Attempting to open %s\n", filename);
  File file = LittleFS.open(filename, "r");
  *size = file.size();
  return &file;
}

void PNGDecoder::pngClose(void *handle) {
  File *file = static_cast<File*>(handle);
  if (file) file->close();
}

int32_t PNGDecoder::pngRead(PNGFILE *page, uint8_t *buffer, int32_t length) {
  File *file = static_cast<File*>(page->fHandle);
  if (!file) return 0;
  page = page; // Avoid warning
  return file->read(buffer, length);
}

int32_t PNGDecoder::pngSeek(PNGFILE *page, int32_t position) {
  File *file = static_cast<File*>(page->fHandle);
  if (!file) return 0;
  page = page; // Avoid warning
  return file->seek(position);
}

// wrapper to have function delegeate to std::function and erase param type
int PNGDecoder::pngDraw(PNGDRAW *pDraw) {
//   if (!file) return 0;
  if (PNGDecoder::s_drawCallback) {
      return PNGDecoder::s_drawCallback((void*)pDraw);
  }
  return 0;
}

