#include "downscale_utils.h"
#include <cmath>

#ifndef LOCAL_TESTING
#include <Arduino.h>
#else
#define ps_malloc malloc
#endif

uint8_t removeGammaCorrection(uint8_t channel);
uint8_t applyGammaCorrection(uint8_t channel);

float removeGammaCorrectionFloat(float channel);
float applyGammaCorrectionFloat(float channel);


IMG_HOLDER createThumbnail(IMG_HOLDER *imgHolder){
  // calc thumbnali width and heighth
  int tnW = imgHolder->width / THUMBNAIL_WIDTH_SCALE_FACTOR;
  int tnH = imgHolder->height / THUMBNAIL_HEIGHT_SCALE_FACTOR;
  uint16_t *tnBytes = (uint16_t*)ps_malloc(tnW*tnH*sizeof(uint16_t));
  int BLOCK_SIZE = THUMBNAIL_WIDTH_SCALE_FACTOR * THUMBNAIL_HEIGHT_SCALE_FACTOR;

  uint16_t *imgBytes = imgHolder->imageBytes;

  for(int i = 0; i< tnH; i++){
    for(int j=0; j< tnW; j++){
      uint32_t tnRedSum8=0;
      uint32_t tnGreenSum8=0;
      uint32_t tnBlueSum8=0;

      // for (j,i) pixel of thumbnail get THUMBNAIL_WIDTH_SCALE_FACTOR * THUMBNAIL_HEIGHT_SCALE_FACTOR pixels from original image
      for(int n=0; n< THUMBNAIL_HEIGHT_SCALE_FACTOR; n++){
        for(int k=0; k< THUMBNAIL_WIDTH_SCALE_FACTOR; k++){
          uint16_t imgX = j*THUMBNAIL_WIDTH_SCALE_FACTOR + k;
          uint16_t imgY = i*THUMBNAIL_HEIGHT_SCALE_FACTOR + n;
          uint16_t imgColors =  imgBytes[imgY*imgHolder->width + imgX];
          uint8_t imgRed5 = (imgColors >> 11) & 0x1F;
          uint8_t imgGreen6 = (imgColors >> 5) & 0x3F;
          uint8_t imgBlue5 = imgColors & 0x1F;

          // Scale to 8 bits
          uint8_t imgRed8 = (imgRed5 << 3) | (imgRed5 >> 2);
          uint8_t imgGreen8 = (imgGreen6 << 2) | (imgGreen6 >> 4);
          uint8_t imgBlue8 = (imgBlue5 << 3) | (imgBlue5 >> 2);

          tnRedSum8 += removeGammaCorrection(imgRed8);
          tnGreenSum8 += removeGammaCorrection(imgGreen8);
          tnBlueSum8 += removeGammaCorrection(imgBlue8);
        }
      }

      uint8_t tnRedAvg8 = applyGammaCorrection(tnRedSum8 / BLOCK_SIZE);
      uint8_t tnGreenAvg8 = applyGammaCorrection(tnGreenSum8 / BLOCK_SIZE);
      uint8_t tnBlueAvg8 = applyGammaCorrection(tnBlueSum8 / BLOCK_SIZE);

      // Scale back to 5/6 bits
      uint8_t tnRed5 = (tnRedAvg8 * 31 + 127) / 255;
      uint8_t tnGreen6 = (tnGreenAvg8 * 63 + 127) / 255;
      uint8_t tnBlue5 = (tnBlueAvg8 * 31 + 127) / 255;

      uint16_t tnColors = (tnRed5 << 11) | (tnGreen6 << 5) | tnBlue5;
      tnBytes[i*tnW + j] = tnColors;
    }
  }

  return IMG_HOLDER{tnW, tnH, tnBytes};
}

// Remove gamma correction: sRGB to linear
uint8_t removeGammaCorrection(uint8_t channel) {
    float normalized = channel / 255.0f;
    float linear = powf(normalized, 2.2f);
    return (uint8_t)(linear * 255.0f + 0.5f);
}

// Apply gamma correction: linear to sRGB
uint8_t applyGammaCorrection(uint8_t channel) {
    float normalized = channel / 255.0f;
    float corrected = powf(normalized, 1.0f / 2.2f);
    return (uint8_t)(corrected * 255.0f + 0.5f);
}

IMG_HOLDER createThumbnailFloat(IMG_HOLDER *imgHolder){
  int tnW = imgHolder->width / THUMBNAIL_WIDTH_SCALE_FACTOR;
  int tnH = imgHolder->height / THUMBNAIL_HEIGHT_SCALE_FACTOR;
  uint16_t *tnBytes = (uint16_t*)ps_malloc(tnW*tnH*sizeof(uint16_t));
  int BLOCK_SIZE = THUMBNAIL_WIDTH_SCALE_FACTOR * THUMBNAIL_HEIGHT_SCALE_FACTOR;

  uint16_t *imgBytes = imgHolder->imageBytes;

  for(int i = 0; i< tnH; i++){
    for(int j=0; j< tnW; j++){
      float tnRedSum = 0.0f;
      float tnGreenSum = 0.0f;
      float tnBlueSum = 0.0f;

      for(int n=0; n< THUMBNAIL_HEIGHT_SCALE_FACTOR; n++){
        for(int k=0; k< THUMBNAIL_WIDTH_SCALE_FACTOR; k++){
          uint16_t imgX = j*THUMBNAIL_WIDTH_SCALE_FACTOR + k;
          uint16_t imgY = i*THUMBNAIL_HEIGHT_SCALE_FACTOR + n;
          uint16_t imgColors =  imgBytes[imgY*imgHolder->width + imgX];
          uint8_t imgRed5 = (imgColors >> 11) & 0x1F;
          uint8_t imgGreen6 = (imgColors >> 5) & 0x3F;
          uint8_t imgBlue5 = imgColors & 0x1F;

          // Scale to 8 bits
          float imgRed8 = (imgRed5 << 3) | (imgRed5 >> 2);
          float imgGreen8 = (imgGreen6 << 2) | (imgGreen6 >> 4);
          float imgBlue8 = (imgBlue5 << 3) | (imgBlue5 >> 2);

          // Normalize to [0,1]
          float r = imgRed8 / 255.0f;
          float g = imgGreen8 / 255.0f;
          float b = imgBlue8 / 255.0f;

          // Remove gamma correction (sRGB to linear)
          r = removeGammaCorrectionFloat(r);
          g = removeGammaCorrectionFloat(g);
          b = removeGammaCorrectionFloat(b);

          tnRedSum += r;
          tnGreenSum += g;
          tnBlueSum += b;
        }
      }

      // Average in linear space
      float rAvg = tnRedSum / BLOCK_SIZE;
      float gAvg = tnGreenSum / BLOCK_SIZE;
      float bAvg = tnBlueSum / BLOCK_SIZE;

      // Apply gamma correction (linear to sRGB)
      rAvg = applyGammaCorrectionFloat(rAvg);
      gAvg = applyGammaCorrectionFloat(gAvg);
      bAvg = applyGammaCorrectionFloat(bAvg);

      // Scale back to 8 bits
      uint8_t tnRed8 = (uint8_t)(rAvg * 255.0f + 0.5f);
      uint8_t tnGreen8 = (uint8_t)(gAvg * 255.0f + 0.5f);
      uint8_t tnBlue8 = (uint8_t)(bAvg * 255.0f + 0.5f);

      // Scale to 5/6 bits
      uint8_t tnRed5 = (tnRed8 * 31 + 127) / 255;
      uint8_t tnGreen6 = (tnGreen8 * 63 + 127) / 255;
      uint8_t tnBlue5 = (tnBlue8 * 31 + 127) / 255;

      uint16_t tnColors = (tnRed5 << 11) | (tnGreen6 << 5) | tnBlue5;
      tnBytes[i*tnW + j] = tnColors;
    }
  }

  return IMG_HOLDER{tnW, tnH, tnBytes};
}

float applyGammaCorrectionFloat(float c) {
    float srgb;
    if (c <= 0.0031308f)
        srgb = c * 12.92f;
    else
        srgb = 1.055f * powf(c, 1.0f / 2.4f) - 0.055f;
    return srgb;
}

float removeGammaCorrectionFloat(float c) {
    float linear;
    if (c <= 0.04045f)
        linear = c / 12.92f;
    else
        linear = powf((c + 0.055f) / 1.055f, 2.4f);
    return linear;
}

IMG_HOLDER createThumbnailNearest(IMG_HOLDER *imgHolder){
  int tnW = imgHolder->width / THUMBNAIL_WIDTH_SCALE_FACTOR;
  int tnH = imgHolder->height / THUMBNAIL_HEIGHT_SCALE_FACTOR;
  uint16_t *tnBytes = (uint16_t*)ps_malloc(tnW*tnH*sizeof(uint16_t));
  uint16_t *imgBytes = imgHolder->imageBytes;

  for(int i = 0; i < tnH; i++){
    for(int j = 0; j < tnW; j++){
      // Find the nearest pixel in the source image
      int srcY = i * THUMBNAIL_HEIGHT_SCALE_FACTOR;
      int srcX = j * THUMBNAIL_WIDTH_SCALE_FACTOR;
      tnBytes[i * tnW + j] = imgBytes[srcY * imgHolder->width + srcX];
    }
  }

  return IMG_HOLDER{tnW, tnH, tnBytes};
}

