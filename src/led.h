#ifndef SMART_ESP_LED_H
#define SMART_ESP_LED_H

#include <Arduino.h>

class Led {

    public:
        static Led* getInstance();
    private:
        static Led* instance;
        bool isBlink = false;
        unsigned long blinkStart = 0;
        Led();
    public:
        void blinkLed();
        void blinkLoop();

};


#endif