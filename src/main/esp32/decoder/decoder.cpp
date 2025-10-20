#include "decoder.h"
#include <stdlib.h>
#include "../log.h"

Decoder::~Decoder(){
    destroyWrappingDelegates();
}

IDrawTarget* Decoder::wrapWithDelegates(IDrawTarget *target){
    if(lastDelegate == nullptr)
        return target;
    lastDelegate->setDelegate(target);
    return firstDelegate;
}   

void Decoder::destroyWrappingDelegates(){
    if(lastDelegate != nullptr)
        lastDelegate->setDelegate(nullptr);
}    

