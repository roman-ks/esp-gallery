#pragma once

#include "decoder.h"

class AdvancableDecoder: public Decoder {

    public:
        AdvancableDecoder(){}
        AdvancableDecoder(std::vector<DelegatingDrawTargetFactory> &delegatingFactories):Decoder(delegatingFactories){}
        virtual void advance(IDrawTarget &iDrawTarget) = 0;

};