#ifndef __IMG_HOLDER_H__
#define __IMG_HOLDER_H__

#include <cstdint>

typedef struct img_holder {
  int width;
  int height;
  uint16_t *imageBytes;
} IMG_HOLDER;

#endif