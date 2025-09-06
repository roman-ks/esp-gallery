#include "thumbnail_utils.h"
#include <PNGdec.h>
#include "log.h"

PNG thumbnailPng;

int pngDrawIntoArray(PNGDRAW *pDraw);
uint8_t removeGammaCorrection(uint8_t channel);
uint8_t applyGammaCorrection(uint8_t channel);

float removeGammaCorrectionFloat(float channel);
float applyGammaCorrectionFloat(float channel);

uint16_t *imageBytes;
int lineNum=0;
IMG_HOLDER readPngIntoArray(char* filepath){
  IMG_HOLDER imgHolder;
  LOG("Opening image..");
  imageBytes = (uint16_t*) ps_malloc(MAX_IMAGE_WIDTH*MAX_IMAGE_HEIGHT*sizeof(uint16_t));
  LOGF("Allocated at 0x%x\n", imageBytes);
  int16_t rc = thumbnailPng.open(filepath, pngOpen, pngClose, pngRead, pngSeek, pngDrawIntoArray);
  if (rc == PNG_SUCCESS) {
    LOGF("Opened %s\n", filepath);
    if (thumbnailPng.getWidth() > MAX_IMAGE_WIDTH) {
      LOG("Image too wide for allocated line buffer size!");
    } else {
      rc = thumbnailPng.decode(NULL, 0);
      imgHolder.width=thumbnailPng.getWidth();
      imgHolder.height=thumbnailPng.getHeight();
      LOGF("Decoded %s\n", filepath);
      imgHolder.imageBytes = imageBytes;
      // LOGF("Copied imageBytes\n");
      thumbnailPng.close();
    }
  }else{
    LOGF("Error opening image: %d", rc);
  }
  return imgHolder;
}

//=========================================v==========================================
//                                      pngDraw
//====================================================================================
// This next function will be called during decoding of the png file to
// render each image line to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.
// Callback function to draw pixels to the display
int pngDrawIntoArray(PNGDRAW *pDraw) {
  uint16_t y = pDraw->y;
  uint16_t w = pDraw->iWidth;
  uint32_t offset = y*w;
  // todo find a way to read directly into imageBytes
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];

  thumbnailPng.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  memcpy(imageBytes+offset, lineBuffer, w*sizeof(uint16_t));
  return 1;
}

// void writeFile(fs::FS &fs, const char *path, const uint8_t *buf, size_t size) {
//   LOGF("Writing file: %s\r\n", path);

//   File file = fs.open(path, FILE_WRITE);
//   if (!file) {
//     LOG("- failed to open file for writing");
//     return;
//   }
//   if (file.write(buf, size)) {
//     LOG("- file written");
//   } else {
//     LOG("- write failed");
//   }
//   file.close();
// }
