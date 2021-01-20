#ifndef SMART_ESP_SMARTHOMEOBJECTS_SWITCH
#define SMART_ESP_SMARTHOMEOBJECTS_SWITCH

#include "smarthomeObjects.h"
#include "MyMqttClient.h"
#include <Arduino.h>

#include "timeUtils.h"
#include "led.h"

class Switch: public BaseObject{
    private:
        int pin;
        const char* statusTopic;
        const char* setTopic;
        const char* msgOff;
        const char* msgOn;
        const char* msgDark;
        bool hasDark = false;
        int physicalState;
        int onlineState;
        unsigned long timeLastPressed = 0;
        unsigned long countdown = 0;
        bool lastPressHandled = true;
    public:
        Switch(int pin, const char* statusTopic, const char* setTopic, const char* msgOff, const char* msgOn){
            this->pin = pin;
            this->statusTopic = statusTopic;
            this->setTopic = setTopic;
            this->msgOff = msgOff;
            this->msgOn = msgOn;
            pinMode(pin, INPUT_PULLUP);
            physicalState = digitalRead( pin );
        }
        Switch(int pin, const char* statusTopic, const char* setTopic, const char* msgOff, const char* msgOn, const char* msgDark){
            this->pin = pin;
            this->statusTopic = statusTopic;
            this->setTopic = setTopic;
            this->msgOff = msgOff;
            this->msgOn = msgOn;
            this->msgDark = msgDark;
            this->hasDark = true;
            pinMode(pin, INPUT_PULLUP);
            physicalState = digitalRead( pin );
        }
        void subscribeToMqtt() override{
            MyMQTTClient::getInstance()->subscribe((char *) statusTopic, qtAT_LEAST_ONCE);
        }
        void msgArrived(char *topic, char *data, bool retain, bool duplicate) override{
            if(strcmp (topic, statusTopic) == 0){ // handle only if subscribed topic
                for (char *iter = data; *iter != '\0'; ++iter) { // to lower
                    *iter = std::tolower(*iter);
                    ++iter;
                }
                if ( strstr(data, "on")) {
                    onlineState = 1;
                    Serial.println("MQTT Recieved Status: ON");
                } else if ( strstr( data, "off")) {
                    onlineState = 0;
                    Serial.println("MQTT Recieved Status: OFF");
                } else {
                    Serial.println("MQTT Recieved unknown Message in topic");   
                    Serial.println(topic);
                    Serial.println(data);
                    Serial.println();
                }
            }
        }
        void checkForAction() override{
            if( digitalRead( pin ) != physicalState && lastPressHandled) {
                lastPressHandled = false;
                timeLastPressed = millis();
            }
            // Hande if 50ms
            if( digitalRead( pin ) != physicalState && !lastPressHandled && millis() - timeLastPressed > 50) {
                lastPressHandled = true;
                physicalState = digitalRead(pin);

                // If off -> turn on
                if (onlineState == 0) {    
                    if (hasDark && !Time::getInstance()->isDay()) {
                        if (!MyMQTTClient::getInstance()->publish((char *) setTopic,(char *) msgDark,qtAT_LEAST_ONCE,false,false)) {
                        Serial.println("MQTT Send ON Dark failed");
                        }
                        countdown = millis();
                        Led::getInstance()->blinkLed();
                    } else {
                        if (!MyMQTTClient::getInstance()->publish((char *) setTopic,(char *) msgOn,qtAT_LEAST_ONCE,false,false)) {
                        Serial.println("MQTT Send ON failed");
                        }
                        Led::getInstance()->blinkLed();
                    }
                } else {
                    if(millis()-countdown <= 3000){
                        if (!MyMQTTClient::getInstance()->publish((char *) setTopic,(char *) msgOn,qtAT_LEAST_ONCE,false,false)) {
                        Serial.println("MQTT Send ON Bright failed");
                        }
                        countdown = 0;
                        Led::getInstance()->blinkLed();
                    }else{
                        if (!MyMQTTClient::getInstance()->publish((char *) setTopic,(char *) msgOff,qtAT_LEAST_ONCE,false,false)) {
                        Serial.println("MQTT Send ON Off failed");
                        }
                        Led::getInstance()->blinkLed();
                    }
                
                }
                
            }
        }
        String getHtmlInfo() override{
            String html = "Switch on pin <b>";
            html += pin;
            html += "</b> is ";
            html += (physicalState==HIGH?"<b>not</b> ":"");
            html += " pressed and online status is ";
            html += (onlineState==1?"<b>ON</b>":"<b>OFF</b>");
            return html;
        }

};

#endif