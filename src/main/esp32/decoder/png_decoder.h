// #ifndef __ESP_GALLERY_PNG_DECODER_H__
// #define __ESP_GALLERY_PNG_DECODER_H__

// #include "decoder.h"
// #include <PNGdec.h>
// #include <LittleFS.h>


// class PNGDecoder : public Decoder {
//     public:
//         PNGDecoder();
//         ~PNGDecoder();

//         void init() override;
//         void decode(char* filepath, std::function<int(void*)> drawCallback, void* returned) override;
//     private:
//         void * pngOpen(const char *filename, int32_t *size);
//         void pngClose(void *handle);
//         int32_t pngRead(PNGFILE *page, uint8_t *buffer, int32_t length);
//         int32_t pngSeek(PNGFILE *page, int32_t position);
//         int32_t pngDraw(PNGDRAW *pDraw);
//         PNG getPNG();

//         File file;
//         std::function<void(void*)> drawCallback;
// };
// #endif