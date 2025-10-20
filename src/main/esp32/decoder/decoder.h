#ifndef __ESP_GALLERY_DECODER_H__
#define __ESP_GALLERY_DECODER_H__
#include <functional>
#include <cstdint>
#include <memory>

#include "i_draw_target.h"
#include "delegating_draw_target.h"

class Decoder {
    public:
        Decoder(): firstDelegate(nullptr), lastDelegate(nullptr){}
        Decoder(DelegatingDrawTarget *firstDelegate, DelegatingDrawTarget *lastDelegate)
            :firstDelegate(firstDelegate),lastDelegate(lastDelegate){}
        virtual ~Decoder();

        virtual void decode(uint8_t *fileBytes, size_t bytesCount, IDrawTarget &iDrawTarget) = 0;
        IDrawTarget* wrapWithDelegates(IDrawTarget *target); 
        void destroyWrappingDelegates();      
    protected:
        DelegatingDrawTarget *firstDelegate, *lastDelegate;
};

#endif