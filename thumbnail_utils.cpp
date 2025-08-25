#include "thumbnail_utils.h"
#include <PNGdec.h>

PNG thumbnailPng;

int pngDrawIntoArray(PNGDRAW *pDraw);

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

IMG_HOLDER createThumbnail(IMG_HOLDER *imgHolder){
  // calc thumbnali width and heighth
  int tnW = imgHolder->width / THUMBNAIL_WIDTH_SCALE_FACTOR;
  int tnH = imgHolder->heigth / THUMBNAIL_HEIGHT_SCALE_FACTOR;
  uint16_t *tnBytes = (uint16_t*)ps_malloc(tnW*tnH*sizeof(uint16_t));

  uint16_t imgBytes = imgHolder->imageBytes;

  for(int i = 0; i< tnH; i++){
    for(int j=0; j< tnW; j++){
      uint8_t tnRedSum;
      uint8_t tnGreenSum;
      uint8_t tnBlueSum;

      // for (j,i) pixel of thumbnail get THUMBNAIL_WIDTH_SCALE_FACTOR * THUMBNAIL_HEIGHT_SCALE_FACTOR pixels from original image
      for(int n=0; n< THUMBNAIL_HEIGTH_SCALE_FACTOR; n++){
        for(int k=0; k< THUMBNAIL_WIDTH_SCALE_FACTOR; k++){
          uint16_t 
        }
      }
    }
  }
}
