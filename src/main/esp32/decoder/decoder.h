#ifndef __ESP_GALLERY_DECODER_H__
#define __ESP_GALLERY_DECODER_H__
#include <functional>
#include <cstdint>
#include <memory>

#include "i_draw_target.h"
#include "delegating_draw_target.h"

using DelegatingDrawTargetFactory = std::function<DelegatingDrawTarget*(IDrawTarget*)>;

class Decoder {
    public:
        Decoder():delegatingFactories(), createdDelegates(){}
        Decoder(std::vector<DelegatingDrawTargetFactory> &delegatingFactories)
            :delegatingFactories(delegatingFactories), createdDelegates(){}
        virtual ~Decoder();

        virtual void init()=0;
        virtual void decode(const char* filepath, IDrawTarget &iDrawTarget) = 0;
        IDrawTarget* wrapWithDelegates(IDrawTarget *target); 
        void destroyWrappingDelegates();      
    protected:
        std::vector<DelegatingDrawTargetFactory> delegatingFactories;
        std::vector<void *> createdDelegates;
};

#endif