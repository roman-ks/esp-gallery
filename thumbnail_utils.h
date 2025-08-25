#ifndef __THUMBNAIL_UTILS__
#define __THUMBNAIL_UTILS__
#include "PNG_support.h"

#define THUMBNAIL_WIDTH_SCALE_FACTOR 3
#define THUMBNAIL_HEIGHT_SCALE_FACTOR 3

#define MAX_IMAGE_WIDTH 240
#define MAX_IMAGE_HEIGHT 320

typedef struct img_holder {
  int width;
  int height;
  uint16_t *imageBytes;
} IMG_HOLDER;

IMG_HOLDER createThumbnail(IMG_HOLDER *imgHolder);

IMG_HOLDER readPngIntoArray(char* filepath);

#endif