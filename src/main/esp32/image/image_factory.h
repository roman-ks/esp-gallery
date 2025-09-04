#ifndef __ESP_GALLERY_IMAGE_FACTORY_H__
#define __ESP_GALLERY_IMAGE_FACTORY_H__

#include "image.h"

class ImageFactory {
    public:
        static Image* createImage(const char* filePath);
    private:
        ImageFactory() = default;
};

#endif