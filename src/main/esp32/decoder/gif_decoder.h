// #pragma once
//
// #include "decoder.h"
// #include <FS.h>
// #include <AnimatedGIF.h>
//
// class GIFDecoder: public Decoder {
//
//     public:
//         GIFDecoder();
//         ~GIFDecoder()=default;
//
//         void init() override;
//         void decode(const char* filepath, DrawCallbackFunc &drawCallback) override;
//
//     private:
//         inline static DrawCallbackFunc s_drawCallback = nullptr;
//         inline static std::unique_ptr<File> file = nullptr;
//         inline static AnimagedGIF gif = AnimagedGIF();
//         inline static uint16_t lineBuffer[MAX_IMAGE_WIDTH];
//
//
// };