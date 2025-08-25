#ifndef __DOWNSCALE_UTILS_H__
#define __DOWNSCALE_UTILS_H__
#include "img_holder.h"
#include <cstdint>

#define THUMBNAIL_WIDTH_SCALE_FACTOR 3
#define THUMBNAIL_HEIGHT_SCALE_FACTOR 3

#define MAX_IMAGE_WIDTH 240
#define MAX_IMAGE_HEIGHT 320

IMG_HOLDER createThumbnail(IMG_HOLDER *imgHolder);

IMG_HOLDER createThumbnailFloat(IMG_HOLDER *imgHolder);

IMG_HOLDER createThumbnailNearest(IMG_HOLDER *imgHolder);

#endif