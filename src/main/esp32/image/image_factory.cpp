#include "image_factory.h"
#include "../decoder/png_decoder.h"
#include "../decoder/gif_decoder.h"
#include "../decoder/downscale_draw_target.h"
#include "../decoder/delegating_draw_target.h"
#include "../decoder/overlay_draw_target.h"
#include "../decoder/clipping_draw_target.h"
#include "png_image.h"
#include "gif_image.h"
#define LOG_LEVEL 2
#include "../log.h"
#include "../../core/configs.h"
#include "play_icon_overlay.h"

Image* ImageFactory::createImage(const char* filePath) {
    std::string ext = getExtension(filePath);
    LOGF_D("Creating image from file %s, ext: %s\n", filePath, filePath);
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
    LOGF_D("Creating image from file %s, ext: %s\n", filePath, filePath);
    if (ext == ".png") {
        static std::vector<DelegatingDrawTargetFactory> delegatingFactories;
        delegatingFactories.push_back([](IDrawTarget *delegate)->DelegatingDrawTarget* { 
                return new ClippingDrawTarget(delegate, GRID_ELEMENT_WIDTH, GRID_ELEMENT_HEIGHT);
            });
        delegatingFactories.push_back([](IDrawTarget *delegate)->DelegatingDrawTarget* { 
                return new DownscaleDrawTarget(delegate, 
                    THUMBNAIL_WIDTH_SCALE_FACTOR, THUMBNAIL_HEIGHT_SCALE_FACTOR);
        });
        static PNGDecoder pngDecoder(delegatingFactories);
        return new PNGImage(pngDecoder, strdup(filePath), true);
    } else if (ext == ".gif"){
        static std::vector<DelegatingDrawTargetFactory> delegatingFactories;
        if(delegatingFactories.empty()){
            delegatingFactories.push_back([](IDrawTarget *delegate)->DelegatingDrawTarget* { 
                return new OverlayDrawTarget(delegate, PlayIcon24x24, 24, 24, 0,0);
            });
            delegatingFactories.push_back([](IDrawTarget *delegate)->DelegatingDrawTarget* { 
                return new ClippingDrawTarget(delegate, GRID_ELEMENT_WIDTH, GRID_ELEMENT_HEIGHT);
            });
            delegatingFactories.push_back([](IDrawTarget *delegate)->DelegatingDrawTarget* { 
                return new DownscaleDrawTarget(delegate, 
                    THUMBNAIL_WIDTH_SCALE_FACTOR, THUMBNAIL_HEIGHT_SCALE_FACTOR);
            });
        }
        static GIFDecoder gifDecoder(delegatingFactories);
        return new GIFImage(gifDecoder, strdup(filePath), true);
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
