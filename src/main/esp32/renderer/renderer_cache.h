#pragma once

#include "pixels_holder.h"
#include <unordered_map>
#include "FS.h"
#include <span>
#include <memory>

class RendererCache {
    private:
        std::unordered_map<std::string, std::unique_ptr<PixelsHolder>> cache;
        // fs::File cacheRoot;
    public:
        RendererCache():cache(){}
        bool exists(const std::string &key);
        PixelsHolder& get(const std::string &key);
        void put(const std::string &key, std::unique_ptr<PixelsHolder> value);
        void write();
        void load(std::span<std::string> keys);

};