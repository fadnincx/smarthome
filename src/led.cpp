#include "led.h"

#include <Arduino.h>

Led* Led::getInstance(){
    return (!instance) ? instance = new Led() : instance;
}
Led* Led::instance = 0;
Led::Led(){
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
}
void Led::blinkLed(){

    // Enable LED
    digitalWrite(LED_BUILTIN, LOW);

    blinkStart = millis();
    isBlink = true;
}
void Led::blinkLoop(){
    if(isBlink && millis() - blinkStart > 100){
        // Disable LED
        digitalWrite(LED_BUILTIN, HIGH);
        isBlink = false;
    }
}    
