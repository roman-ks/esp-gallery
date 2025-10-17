#pragma once

#include "decoder.h"

class AdvancableDecoder: public Decoder {

    public:
        AdvancableDecoder() = default;
        AdvancableDecoder(DelegatingDrawTarget *firstDelegate, DelegatingDrawTarget *lastDelegate):
            Decoder(firstDelegate, lastDelegate){}
        virtual void advance(IDrawTarget &iDrawTarget) = 0;

};