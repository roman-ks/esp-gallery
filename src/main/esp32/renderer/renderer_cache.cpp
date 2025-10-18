#include "renderer_cache.h"
#define LOG_LEVEL 2
#include "../log.h"
#include <cstdLib>
#include "../../core/configs.h"


RendererCache::RendererCache(fs::FS &fileSys):fileSys(fileSys), cache(), written(){
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

    return loadAndCache(key);
}

bool RendererCache::loadAndCache(const std::string &key){
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

    if(fileSys.exists(binFilename.c_str()) /*&& fileSys.exists(mdFilename.c_str())*/){
        written.insert(std::string(key));
        return;        
    }
    std::string tmpFilename = std::string("/c/tmp.b");


    LOGF_D("Starting to write %s to write %d pixels\n", binFilename.c_str(), pixelsHolder.pixels.size());

    uint32_t start = millis();

    fs::File file = fileSys.open(binFilename.c_str(), FILE_WRITE, true);
    byte mdBuf[2];
    mdBuf[0]=pixelsHolder.width & 0xff;
    mdBuf[1]= pixelsHolder.width >> 8;
    LOGF_D("Writing 0x%x 0x%x\n", mdBuf[0], mdBuf[1]);
    size_t res = file.write(mdBuf, sizeof(mdBuf));
    if(res != 2){
        LOGF_D("Invalid write size %d, expected: %d\n", res, 2);
        handleInvalidWrite(file, binFilename);
        return;
    }
    mdBuf[0]=pixelsHolder.height & 0xff;
    mdBuf[1]= pixelsHolder.height >> 8;
    LOGF_D("Writing 0x%x 0x%x\n", mdBuf[0], mdBuf[1]);
    res = file.write(mdBuf, sizeof(mdBuf));
    if(res != 2){
        LOGF_D("Invalid write size %d, expected: %d\n", res, 2);
        handleInvalidWrite(file, binFilename);
        return;
    }
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(pixelsHolder.pixels.data());
    res = file.write(bytes, pixelsHolder.pixels.size()*sizeof(uint16_t));
    if(res != pixelsHolder.pixels.size()*sizeof(uint16_t)){
        LOGF_D("Invalid write size %d, expected: %d\n", res, pixelsHolder.pixels.size()*sizeof(uint16_t));
        handleInvalidWrite(file, binFilename);
        return;
    }
    file.close();

    fileSys.rename(tmpFilename.c_str(), binFilename.c_str());
    uint32_t end = millis();
    float speed = pixelsHolder.pixels.size()*sizeof(uint16_t)/(static_cast<float>(end-start)/1000);
    LOGF_D("Written %s in %dms (%.1f B/s)\n", binFilename.c_str(), end-start, speed);

    written.insert(std::string(key));
}

void RendererCache::handleInvalidWrite(fs::File &file, std::string &filename){
    file.close();
    fileSys.remove(filename.c_str());
    LOG_I("Invalid write. Deleted file");
}


std::unique_ptr<PixelsHolder> RendererCache::load(const std::string &key){
    LOGF_D("Loading for key %s\n", key.c_str());
    std::string binFilename = std::string("/c")+key+".b";

    if(!fileSys.exists(binFilename.c_str())){
        LOGF_I("Cache file %s is missing\n", binFilename.c_str());
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

    if(w > MAX_IMAGE_WIDTH || h > MAX_IMAGE_HEIGHT){
        // invalid cached item delete to be retried
        file.close();
        bool removed = fileSys.remove(binFilename.c_str());
        LOGF_I("Removed invalid file with w: %d, h: %d. Result %d\n", w, h, removed);
        return nullptr;
    }
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

void RendererCache::unload(const std::string &key){
    cache.erase(key);
}
