#ifndef __ESP_GALLERY_IMAGE_FACTORY_H__
#define __ESP_GALLERY_IMAGE_FACTORY_H__

#include "image.h"
// forward-declare fs::FS to avoid including FS.h in header (reduce include dependencies)
namespace fs { class FS; }

class ImageFactory {
    public:
        static Image* createImage(const std::string &filePath, std::string *ext, bool cachable=false);
        static Image* createDownscaledImage(const std::string &filePath, std::string *ext);

        static bool getImageSize(fs::FS &fileSys, const std::string &filePath, const std::string &ext, uint16_t *width, uint16_t *height);
    private:
        inline static uint8_t *buf;
        ImageFactory();

        static std::string getExtension(const std::string &filePath);
};

#endif