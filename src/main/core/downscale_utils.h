#ifndef __DOWNSCALE_UTILS_H__
#define __DOWNSCALE_UTILS_H__
#include "img_holder.h"
#include <cstdint>

#define THUMBNAIL_WIDTH_SCALE_FACTOR 3
#define THUMBNAIL_HEIGHT_SCALE_FACTOR 3

#define MAX_IMAGE_WIDTH 240
#define MAX_IMAGE_HEIGHT 320

void convertRgb565ToRgb888(uint16_t rgb565, uint8_t &r, uint8_t &g, uint8_t &b);

uint16_t convertRgb888ToRgb565(uint8_t r, uint8_t g, uint8_t b);

IMG_HOLDER createThumbnail(IMG_HOLDER *imgHolder);

IMG_HOLDER createThumbnailFloat(IMG_HOLDER *imgHolder);

IMG_HOLDER createThumbnailNearest(IMG_HOLDER *imgHolder);

#endif