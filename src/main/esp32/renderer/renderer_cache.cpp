#include "renderer_cache.h"

bool RendererCache::exists(const std::string &key){
    return cache.find(key)!=cache.end();
}

PixelsHolder &RendererCache::get(const std::string &key){
    return *cache[key];
}

void RendererCache::put(const std::string &key, std::unique_ptr<PixelsHolder> value){
    cache[std::string(key)] = std::move(value);
}


void RendererCache::write(){}
void RendererCache::load(std::span<std::string> keys){}