#include "image_factory.h"
#include "PNGDec.h"
#include "../decoder/png_decoder.h"
#include "png_image.h"

Image* ImageFactory::createImage(const char* filePath) {
    // Simple check based on file extension
    std::string pathStr(filePath);
    if (pathStr.size() >= 4 && pathStr.substr(pathStr.size() - 4) == ".png") {
        static PNGDecoder pngDecoder;
        return new PNGImage(pngDecoder, strdup(filePath));
    } 
    // Add more formats as needed
    return nullptr; // Unsupported format
}