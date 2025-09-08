#ifndef __ESP_GALLERY_DECODER_H__
#define __ESP_GALLERY_DECODER_H__
#include <functional>
#include <cstdint>
#include <memory>

#include "i_draw_target.h"

class Decoder {
    public:
        virtual ~Decoder() = default;

        virtual void init();
        virtual void decode(const char* filepath, IDrawTarget &iDrawTarget) = 0;
    private:
};

#endif