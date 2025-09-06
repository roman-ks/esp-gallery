#ifndef __ESP_GALLERY_PNG_DECODER_H__
#define __ESP_GALLERY_PNG_DECODER_H__

#include "decoder.h"
#include <PNGdec.h>
#include <FS.h>
#include <memory>
#include "../../core/configs.h"


class PNGDecoder : public Decoder {
    public:
        PNGDecoder();
        ~PNGDecoder();

        void init() override;
        void decode(char* filepath, DrawCallbackFunc drawCallback) override;
    private:
        static void * pngOpen(const char *filename, int32_t *size);
        static void pngClose(void *handle);
        static int32_t pngRead(PNGFILE *page, uint8_t *buffer, int32_t length);
        static int32_t pngSeek(PNGFILE *page, int32_t position);
        static int pngDraw(PNGDRAW *pDraw);

        inline static DrawCallbackFunc s_drawCallback = nullptr;
        inline static std::unique_ptr<File> file = nullptr;
        inline static PNG png = PNG();
        inline static uint16_t lineBuffer[MAX_IMAGE_WIDTH];
};
#endif