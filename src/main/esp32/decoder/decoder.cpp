#include "decoder.h"
#include <stdlib.h>
#include "../log.h"

Decoder::~Decoder(){
    destroyWrappingDelegates();
}

IDrawTarget* Decoder::wrapWithDelegates(IDrawTarget *target){
    for(DelegatingDrawTargetFactory factory: delegatingFactories){
        DelegatingDrawTarget *wrappingDelegate = factory(target);
        createdDelegates.push_back(wrappingDelegate);
        target = wrappingDelegate;
    }
    return target;
}   

void Decoder::destroyWrappingDelegates(){
    for(void *delegate: createdDelegates){
        free(delegate);
    }
    createdDelegates.clear();
}    

