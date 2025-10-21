#pragma once

#include "../decoder/advancable_decoder.h"
#include <FS.h>
#include <TJpg_Decoder.h>
#define BUFFER_SIZE 256            // Optimum is >= GIF width or integral division of width
#include "../../core/configs.h"
#include <string>


class JPGDecoder: public Decoder {

    public:
        JPGDecoder(): JPGDecoder(nullptr, nullptr) {}
        JPGDecoder(DelegatingDrawTarget *firstDelegate, DelegatingDrawTarget *lastDelegate);
        ~JPGDecoder()=default;

        void decode(uint8_t *fileBytes, size_t bytesCount, IDrawTarget &iDrawTarget) override;
        
        static bool tftOutput(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap);

        static void setIDrawTarget(IDrawTarget *target){
            iDrawTarget = target;
        }

    private:
        inline static IDrawTarget *iDrawTarget = nullptr;
};