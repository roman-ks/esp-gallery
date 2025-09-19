#pragma once

#include "pixels_holder.h"
#include <unordered_map>
#include "FS.h"
#include <span>
#include <memory>
#include <unordered_set>

class RendererCache {
    private:
        std::unordered_map<std::string, std::unique_ptr<PixelsHolder>> cache;
        std::unordered_set<std::string> written;
        fs::FS fileSys;
    public:
        RendererCache(fs::FS fileSys);
        bool exists(const std::string &key);
        PixelsHolder& get(const std::string &key);
        void put(const std::string &key, std::unique_ptr<PixelsHolder> value);
        void write(const std::string &key, const PixelsHolder &value);
        void handleInvalidWrite(fs::File &file, std::string &filename);
        std::unique_ptr<PixelsHolder> load(const std::string &key);
        bool loadAndCache(const std::string &key);

        void createDir(const std::string &filepath);

};