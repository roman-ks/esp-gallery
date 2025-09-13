#pragma once

#include <cstdint>
#include <vector>
#include "../utils/psram_vector.h"

class PixelsHolder {
    public:
        const uint16_t width;
        const uint16_t height;
        psram_vector<uint16_t> pixels;

        PixelsHolder(uint16_t width, uint16_t height, uint16_t *pixels): 
            width(width), height(height){
              this->pixels = psram_vector<uint16_t>(pixels, pixels+size_t(width)*height);
        }

};