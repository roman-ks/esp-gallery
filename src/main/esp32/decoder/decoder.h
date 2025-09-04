#ifndef __ESP_GALLERY_DECODER_H__
#define __ESP_GALLERY_DECODER_H__
#include <functional>



class Decoder {
    public:
        ~Decoder() = default;

        virtual void init();
        virtual void decode(char* filepath, std::function<int(void*)> drawCallback, void* returned) = 0;
    private:
};

#endif