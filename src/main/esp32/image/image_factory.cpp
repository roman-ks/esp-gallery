#include "image_factory.h"
#include "FS.h"
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
#define MAX_BUF 8196

Image* ImageFactory::createImage(const std::string &filePath, std::string *ext, bool cachable) {
    if(!ext || ext->empty())
        *ext = getExtension(filePath);

    LOGF_D("Creating image from file %s, ext: %s\n", filePath.c_str(), ext->c_str());
    if (*ext == ".jpg"){
        static JPGDecoder jpgDecoder;
        return new PNGImage(jpgDecoder, filePath, cachable);
    } else if(*ext == ".png") {
        static PNGDecoder pngDecoder;
        return new PNGImage(pngDecoder, filePath, cachable);
    } else if (*ext == ".gif"){
        static GIFDecoder gifDecoder;
        return new GIFImage(gifDecoder, filePath, cachable);
    }
    // Add more formats as needed
    return nullptr; // Unsupported format
}

Image* ImageFactory::createDownscaledImage(const std::string &filePath, std::string *ext){
     if(!ext || ext->empty())
        *ext = getExtension(filePath);
    LOGF_D("Creating image from file %s, ext: %s\n", filePath.c_str(), ext->c_str());
    // todo add more jpg extensions like jpeg,..
    if (*ext == ".jpg" || *ext == ".png") {
        // todo find a way to share these targets between multiple image types
        static ClippingDrawTarget clippingTarget(nullptr, GRID_ELEMENT_WIDTH, GRID_ELEMENT_HEIGHT);
        static DownscaleDrawTarget downscaleTarget(&clippingTarget, 
            THUMBNAIL_WIDTH_SCALE_FACTOR, THUMBNAIL_HEIGHT_SCALE_FACTOR);
        if(*ext == ".jpg"){
            static JPGDecoder jpgDecoder(&downscaleTarget, &clippingTarget);
            // todo make PNGImage reusable for jpg
            return new PNGImage(jpgDecoder, filePath, false);
        } else {
            static PNGDecoder pngDecoder(&downscaleTarget, &clippingTarget);
            return new PNGImage(pngDecoder, filePath, true);
        }
    } else if (*ext == ".gif"){
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
    // todo convert to lowercase
    return pathStr.substr(extStart, pathStr.length()-extStart);
}

bool ImageFactory::getImageSize(fs::FS &fileSys, const std::string &filePath, const std::string &ext, uint16_t *width, uint16_t *height){
    if(!width || !height) return false;
    *width = *height = 0;

    fs::File f = fileSys.open(filePath.c_str(), FILE_READ);
    if(!f) return false;

    if(!buf)
        buf = new uint8_t[MAX_BUF];

    if(ext == ".png"){
        // Read first 24 bytes in one call: 8-byte signature + 4-byte length + 4-byte 'IHDR' + 8 bytes width/height
        uint8_t buf[24];
        if(f.read(buf, sizeof(buf)) < (int)sizeof(buf)) return false;
        // verify signature
        if(!(buf[0]==0x89 && buf[1]==0x50 && buf[2]==0x4E && buf[3]==0x47 && buf[4]==0x0D && buf[5]==0x0A && buf[6]==0x1A && buf[7]==0x0A)) return false;
        // IHDR should start at buf[8+4..]
        // buf[12..15] == 'IHDR'
        if(!(buf[12]=='I' && buf[13]=='H' && buf[14]=='D' && buf[15]=='R')) return false;
        uint32_t w = (uint32_t)buf[16]<<24 | (uint32_t)buf[17]<<16 | (uint32_t)buf[18]<<8 | (uint32_t)buf[19];
        uint32_t h = (uint32_t)buf[20]<<24 | (uint32_t)buf[21]<<16 | (uint32_t)buf[22]<<8 | (uint32_t)buf[23];
        *width = (uint16_t)w; *height = (uint16_t)h;
        return true;
    }

    if(ext == ".gif"){
        // Read first 10 bytes (6-byte signature + 4 bytes logical screen descriptor)
        uint8_t buf[10];
        if(f.read(buf, sizeof(buf)) < (int)sizeof(buf)) return false;
        if(!(buf[0]=='G' && buf[1]=='I' && buf[2]=='F' && buf[3]=='8' && (buf[4]=='7' || buf[4]=='9') && buf[5]=='a')) return false;
        uint16_t w = (uint16_t)buf[6] | (uint16_t)buf[7]<<8;
        uint16_t h = (uint16_t)buf[8] | (uint16_t)buf[9]<<8;
        *width = (uint16_t)w; *height = (uint16_t)h;
        return true;
    }

    if(ext == ".jpg" || ext == ".jpeg"){
        // Use a fixed-size stack buffer. Be careful: large stack allocations can risk overflow.
        // If that's a concern, switch to a static/global buffer.
        const size_t CHUNK = 512;
        
        size_t bufCap = MAX_BUF;

        int r = f.read(buf, (int)CHUNK);
        if(r <= 0) return false;
        size_t dataLen = (size_t)r;

        // Ensure starts with SOI
        if(dataLen < 2 || buf[0]!=0xFF || buf[1]!=0xD8) return false;

        size_t pos = 2;
        auto ensure_more = [&](size_t need)->bool{
            if(pos + need <= dataLen) return true;
            if(dataLen >= bufCap) return false; // cannot grow beyond MAX_BUF
            size_t want = std::max(CHUNK, need);
            size_t canRead = std::min(want, bufCap - dataLen);
            if(canRead == 0) return false;
            int got = f.read(buf + dataLen, (int)canRead);
            if(got > 0){
                dataLen += (size_t)got;
                return (pos + need <= dataLen);
            }
            return false;
        };

        bool result = false;
        while(true){
            if(!ensure_more(4)) break; // need marker and length
            // find 0xFF
            while(pos < dataLen && buf[pos] != 0xFF) pos++;
            if(pos+1 >= dataLen){ if(!ensure_more(1)) break; }
            // skip any 0xFF padding
            while(pos < dataLen && buf[pos] == 0xFF) pos++;
            if(pos >= dataLen){ if(!ensure_more(1)) break; }
            if(pos >= dataLen) break;
            uint8_t marker = buf[pos++];
            // markers without length
            if(marker >= 0xD0 && marker <= 0xD9) continue;
            if(marker == 0x00) continue; // stuffed byte

            if(!ensure_more(2)) break;
            uint16_t segLen = (uint16_t)buf[pos]<<8 | (uint16_t)buf[pos+1];
            if(segLen < 2) break;
            pos += 2;
            // SOF markers
            if(marker==0xC0 || marker==0xC1 || marker==0xC2 || marker==0xC3 || marker==0xC5 || marker==0xC6 || marker==0xC7 || marker==0xC9 || marker==0xCA || marker==0xCB || marker==0xCD || marker==0xCE || marker==0xCF){
                if(!ensure_more(5)) break;
                uint16_t h = (uint16_t)buf[pos+1]<<8 | (uint16_t)buf[pos+2];
                uint16_t w = (uint16_t)buf[pos+3]<<8 | (uint16_t)buf[pos+4];
                *width = (uint16_t)w; *height = (uint16_t)h;
                result = true;
                break;
            } else {
                // skip this segment (segLen-2 bytes)
                size_t skip = segLen - 2;
                pos += skip;
            }
        }

        return result;
    }

    return false; // unsupported extension
}
