#ifndef __ESP_GALLERY_DECODER_H__
#define __ESP_GALLERY_DECODER_H__
#include <functional>
#include <cstdint>
#include <memory>

using DrawCallbackFunc = std::function<int(uint16_t, uint16_t, uint16_t, uint16_t*)>;

class Decoder {
    public:
        ~Decoder() = default;

        virtual void init();
        virtual void decode(char* filepath, DrawCallbackFunc drawCallback) = 0;
    private:
};

#endif