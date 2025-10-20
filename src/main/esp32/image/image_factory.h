#ifndef __ESP_GALLERY_IMAGE_FACTORY_H__
#define __ESP_GALLERY_IMAGE_FACTORY_H__

#include "image.h"

class ImageFactory {
    public:
        static Image* createImage(const std::string &filePath, bool cachable=false);
        static Image* createDownscaledImage(const std::string &filePath);
    private:
        ImageFactory() = default;

        static std::string getExtension(const std::string &filePath);
};

#endif