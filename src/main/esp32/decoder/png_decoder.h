#ifndef __ESP_GALLERY_PNG_DECODER_H__
#define __ESP_GALLERY_PNG_DECODER_H__

#include "decoder.h"
#include <PNGdec.h>
#include <FS.h>


class PNGDecoder : public Decoder {
    public:
        PNGDecoder();
        ~PNGDecoder();

        void init() override;
        void decode(char* filepath, std::function<int(void*)> drawCallback, void* context) override;
    private:
        static void * pngOpen(const char *filename, int32_t *size);
        static void pngClose(void *handle);
        static int32_t pngRead(PNGFILE *page, uint8_t *buffer, int32_t length);
        static int32_t pngSeek(PNGFILE *page, int32_t position);
        static int pngDraw(PNGDRAW *pDraw);
        PNG getPNG();

        inline static std::function<int(void*)> s_drawCallback = nullptr;
};
#endif