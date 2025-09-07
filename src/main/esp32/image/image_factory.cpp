#include "image_factory.h"
#include "../decoder/png_decoder.h"
#include "../decoder/gif_decoder.h"
#include "../decoder/png_downscale_decoder.h"
#include "png_image.h"
#include "gif_image.h"
#include "../log.h"

Image* ImageFactory::createImage(const char* filePath) {
    std::string ext = getExtension(filePath);
    LOGF("Creating image from file %s, ext: %s\n", filePath, filePath);
    if (ext == ".png") {
        static PNGDecoder pngDecoder;
        return new PNGImage(pngDecoder, strdup(filePath));
    } else if (ext == ".gif"){
        static GIFDecoder gifDecoder;
        return new GIFImage(gifDecoder, strdup(filePath));
    }
    // Add more formats as needed
    return nullptr; // Unsupported format
}

Image* ImageFactory::createDownscaledImage(const char* filePath){
    std::string ext = getExtension(filePath);
    LOGF("Creating image from file %s, ext: %s\n", filePath, filePath);
    if (ext == ".png") {
        static PNGDownscaleDecoder pngDecoder;
        return new PNGImage(pngDecoder, strdup(filePath));
    } 
    // Add more formats as needed
    return nullptr; // Unsupported format
}

std::string ImageFactory::getExtension(const char* filePath){
    std::string pathStr(filePath);
    size_t extStart = pathStr.rfind('.');
    size_t fileNameStart = pathStr.rfind('/');
    // check if ext is in filename, and not in parent folder name
    if(fileNameStart>=extStart)
        return std::string();
    return pathStr.substr(extStart, pathStr.length()-extStart);
}
