#pragma once

class IController {
    public:
        virtual void loop()=0;  
        virtual ~IController(){};
        
        virtual void onPause() = 0;
        virtual void onResume() = 0;

        virtual void handleRightButtonPress() = 0;
        virtual void handleRightButtonLongPress() = 0;

        virtual void handleEnterButtonPress() = 0;

        virtual void handleLeftButtonPress() = 0;
        virtual void handleLeftButtonLongPress() = 0;
};