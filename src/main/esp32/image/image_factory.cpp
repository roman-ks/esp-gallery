#include "image_factory.h"
#include "../decoder/png_decoder.h"
#include "../decoder/gif_decoder.h"
#include "../decoder/jpg_decoder.h"
#include "../decoder/downscale_draw_target.h"
#include "../decoder/overlay_draw_target.h"
#include "../decoder/clipping_draw_target.h"
#include "png_image.h"
#include "gif_image.h"
#define LOG_LEVEL 2
#include "../log.h"
#include "../../core/configs.h"
#include "play_icon_overlay.h"

Image* ImageFactory::createImage(const std::string &filePath, bool cachable) {
    std::string ext = getExtension(filePath);
    LOGF_D("Creating image from file %s, ext: %s\n", filePath.c_str(), ext.c_str());
    if (ext == ".png") {
        static PNGDecoder pngDecoder;
        return new PNGImage(pngDecoder, filePath, cachable);
    } else if (ext == ".gif"){
        static GIFDecoder gifDecoder;
        return new GIFImage(gifDecoder, filePath, cachable);
    }
    // Add more formats as needed
    return nullptr; // Unsupported format
}

Image* ImageFactory::createDownscaledImage(const std::string &filePath){
    std::string ext = getExtension(filePath);
    LOGF_D("Creating image from file %s, ext: %s\n", filePath.c_str(), ext.c_str());
    // todo add more jpg extensions like jpeg,..
    if (ext == ".jpg" || ext == ".png") {
        // todo find a way to share these targets between multiple image types
        static ClippingDrawTarget clippingTarget(nullptr, GRID_ELEMENT_WIDTH, GRID_ELEMENT_HEIGHT);
        static DownscaleDrawTarget downscaleTarget(&clippingTarget, 
            THUMBNAIL_WIDTH_SCALE_FACTOR, THUMBNAIL_HEIGHT_SCALE_FACTOR);
        if(ext == ".jpg"){
            static JPGDecoder jpgDecoder(&downscaleTarget, &clippingTarget);
            // todo make PNGImage reusable for jpg
            return new PNGImage(jpgDecoder, filePath, false);
        } else {
            static PNGDecoder pngDecoder(&downscaleTarget, &clippingTarget);
            return new PNGImage(pngDecoder, filePath, true);
        }
    } else if (ext == ".gif"){
        static OverlayDrawTarget overlayTarget(nullptr, PlayIcon24x24, 24, 24, 0, 0);
        static ClippingDrawTarget clippingTarget(&overlayTarget, GRID_ELEMENT_WIDTH, GRID_ELEMENT_HEIGHT);
        static DownscaleDrawTarget downscaleTarget(&clippingTarget, 
            THUMBNAIL_WIDTH_SCALE_FACTOR, THUMBNAIL_HEIGHT_SCALE_FACTOR); 

        static GIFDecoder gifDecoder(&downscaleTarget, &overlayTarget);
        return new GIFImage(gifDecoder, filePath, true);
    }
    // Add more formats as needed
    return nullptr; // Unsupported format
}

std::string ImageFactory::getExtension(const std::string &pathStr){
    size_t extStart = pathStr.rfind('.');
    size_t fileNameStart = pathStr.rfind('/');
    // check if ext is in filename, and not in parent folder name
    if(fileNameStart>=extStart)
        return std::string();
    return pathStr.substr(extStart, pathStr.length()-extStart);
}
