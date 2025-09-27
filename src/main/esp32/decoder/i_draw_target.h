#pragma once

#include <cstdint>

class IDrawTarget {
    public:
        virtual void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0;
        virtual void pushPixels(const void *pixels, uint32_t count) = 0;
};