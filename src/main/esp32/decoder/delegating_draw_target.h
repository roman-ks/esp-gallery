#pragma once

#include "i_draw_target.h"
#include <memory>

class DelegatingDrawTarget: public IDrawTarget {
    public:
        DelegatingDrawTarget(IDrawTarget *delegate): delegate(delegate){}

        void setDelegate(IDrawTarget *newDelegate){
            delegate = newDelegate;
        }
    protected:
        IDrawTarget *delegate;
};