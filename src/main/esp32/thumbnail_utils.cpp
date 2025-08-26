#include "thumbnail_utils.h"
#include <PNGdec.h>

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
  Serial.println("Opening image..");
  imageBytes = (uint16_t*) ps_malloc(MAX_IMAGE_WIDTH*MAX_IMAGE_HEIGHT*sizeof(uint16_t));
  Serial.printf("Allocated at 0x%x\n", imageBytes);
  int16_t rc = thumbnailPng.open(filepath, pngOpen, pngClose, pngRead, pngSeek, pngDrawIntoArray);
  if (rc == PNG_SUCCESS) {
    Serial.printf("Opened %s\n", filepath);
    if (thumbnailPng.getWidth() > MAX_IMAGE_WIDTH) {
      Serial.println("Image too wide for allocated line buffer size!");
    } else {
      rc = thumbnailPng.decode(NULL, 0);
      imgHolder.width=thumbnailPng.getWidth();
      imgHolder.height=thumbnailPng.getHeight();
      Serial.printf("Decoded %s\n", filepath);
      imgHolder.imageBytes = imageBytes;
      // Serial.printf("Copied imageBytes\n");
      thumbnailPng.close();
    }
  }else{
    Serial.printf("Error opening image: %d", rc);
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

