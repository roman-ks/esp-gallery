#include "renderer_cache.h"
#define LOG_LEVEL 2
#include "../log.h"
#include <cstdLib>


RendererCache::RendererCache(fs::FS fileSys):fileSys(fileSys), cache(), written(){
    createDir(std::string("/c"));
}

void RendererCache::createDir(const std::string &filepath){
    LOGF_D("Creating Dir: %s\n", filepath.c_str());
    if (fileSys.mkdir(filepath.c_str())) {
        LOG_D("Dir created");
    } else {
        LOG_D("mkdir failed");
    }
}

bool RendererCache::exists(const std::string &key){
    if(cache.find(key)!=cache.end())
        return true;

    uint32_t start = millis();
    std::unique_ptr<PixelsHolder> loaded = load(key);
    if(loaded == nullptr){
        return false;
    } else {
        LOGF_I("loaded %s from FS in %dms\n", key.c_str(), millis()-start);
        cache[key] = std::move(loaded);
        return true;
    }
}

PixelsHolder &RendererCache::get(const std::string &key){
    return *cache[key];
}

void RendererCache::put(const std::string &key, std::unique_ptr<PixelsHolder> value){
    cache[std::string(key)] = std::move(value);

    uint32_t start = millis();
    write(key, *cache[key]);
    LOGF_I("written %s to FS in %dms\n", key.c_str(), millis()-start);

}


void RendererCache::write(const std::string &key, const PixelsHolder &pixelsHolder){
    if(written.contains(key))
        return;

    std::string binFilename = std::string("/c")+key+".b";
    // std::string mdFilename = std::string("/c")+key+".m";

    if(fileSys.exists(binFilename.c_str()) /*&& fileSys.exists(mdFilename.c_str())*/){
        written.insert(std::string(key));
        return;        
    }

    LOGF_D("Starting to write %s to write %d pixels\n", binFilename.c_str(), pixelsHolder.pixels.size());

    uint32_t start = millis();

    fs::File file = fileSys.open(binFilename.c_str(), FILE_WRITE, true);
    byte mdBuf[2];
    mdBuf[0]=pixelsHolder.width & 0xff;
    mdBuf[1]= pixelsHolder.width >> 8;
    LOGF_D("Writing 0x%x 0x%x", mdBuf[0], mdBuf[1]);
    file.write(mdBuf, sizeof(mdBuf));
    mdBuf[0]=pixelsHolder.height & 0xff;
    mdBuf[1]= pixelsHolder.height >> 8;
    LOGF_D("Writing 0x%x 0x%x", mdBuf[0], mdBuf[1]);
    file.write(mdBuf, sizeof(mdBuf));
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(pixelsHolder.pixels.data());
    file.write(bytes, pixelsHolder.pixels.size()*sizeof(uint16_t));
    file.close();

    uint32_t end = millis();
    float speed = pixelsHolder.pixels.size()*sizeof(uint16_t)/(static_cast<float>(end-start)/1000);
    LOGF_D("Written %s in %dms (%.1f B/s)\n", binFilename.c_str(), end-start, speed);

    // start = millis();
    // file = fileSys.open(mdFilename.c_str(), FILE_WRITE);
    // char buf[16];
    // int len = sprintf(buf, "%dx%d", pixelsHolder.width, pixelsHolder.height)+1;
    // LOGF_D("Writing %s (%d bytes)\n", buf, len);

    // if(!file.print(buf)){
    //     LOG_I("failed to write");
    // }
    // file.close();

    // end = millis();
    // LOGF_D("Written %s in %dms\n", mdFilename.c_str(), end-start);

    written.insert(std::string(key));
}


std::unique_ptr<PixelsHolder> RendererCache::load(const std::string &key){
    std::string binFilename = std::string("/c")+key+".b";

    if(!fileSys.exists(binFilename.c_str())){
        LOGF_I("One of the cache files for %s is missing\n", key.c_str());
        return nullptr;        
    }

    byte mdBuf[2];
    uint32_t start = millis();
    fs::File file = fileSys.open(binFilename.c_str());
    LOGF_D("Opened %s in %dms\n", binFilename.c_str(), millis()-start);

    if(!file){
        LOGF_I("failed to open %s", binFilename.c_str());
    }
    file.read(mdBuf, 2);
    uint16_t w = mdBuf[1]<<8 | mdBuf[0];
    
    file.seek(2);
    file.read(mdBuf, 2);
    uint16_t h = mdBuf[1]<<8 | mdBuf[0];
    LOGF_D("W: %d, H: %d\n", w,h);

    file.seek(4);
    auto pixels = psram_vector<uint16_t>(w*h);
    start = millis();
    file.read(reinterpret_cast<uint8_t*>(pixels.data()), w*h*sizeof(uint16_t));
    file.close();
    uint32_t end = millis();
    float speed = pixels.size()*sizeof(uint16_t)/(static_cast<float>(end-start)/1000);
    LOGF_D("Read %s in %dms (%.1f B/s)\n", binFilename.c_str(), end-start, speed);

    return std::make_unique<PixelsHolder>(w,h, std::move(pixels));
}

// std::unique_ptr<PixelsHolder> RendererCache::load(const std::string &key){
//     std::string binFilename = std::string("/c")+key+".b";
//     std::string mdFilename = std::string("/c")+key+".m";

//     if(!fileSys.exists(binFilename.c_str()) || !fileSys.exists(mdFilename.c_str())){
//         LOGF_I("One of the cache files for %s is missing", key.c_str());
//         return nullptr;        
//     }

//     byte mdBuf[16];
//     uint32_t start = millis();
//     fs::File file = fileSys.open(mdFilename.c_str());
//     LOGF_D("Opened %s in %dms\n", mdFilename.c_str(), millis()-start);

//     if(!file){
//         LOGF_I("failed to open %s", mdFilename.c_str());
//     }
//     file.read(mdBuf, 16);
//     file.close();

//     const char *charBuf = reinterpret_cast<const char*>(mdBuf);
//     char *delimiterPtr = strchr(charBuf, 'x');

//     uint16_t w = atoi(charBuf);
//     uint16_t h = atoi(delimiterPtr+1);
//     uint32_t end = millis();
//     LOGF_D("Read and parsed %s in %dms\n", mdFilename.c_str(), end-start);

//     LOGF_D("W: %d, H: %d\n", w,h);

//     auto pixels = psram_vector<uint16_t>(w*h);
//     start = millis();
//     file = fileSys.open(binFilename.c_str());
//     if(!file){
//         LOGF_I("failed to open %s", binFilename.c_str());
//     }
//     file.read(reinterpret_cast<uint8_t*>(pixels.data()), w*h*sizeof(uint16_t));
//     file.close();
//     end = millis();
//     float speed = pixels.size()*sizeof(uint16_t)/(static_cast<float>(end-start)/1000);
//     LOGF_D("Read %s in %dms (%.1f B/s)\n", binFilename.c_str(), end-start, speed);

//     return std::make_unique<PixelsHolder>(w,h, std::move(pixels));
// }

