#ifndef SMART_ESP_SMARTHOMEOBJECTS_SHUTTER_RELAIS
#define SMART_ESP_SMARTHOMEOBJECTS_SHUTTER_RELAIS

#include "smarthomeObjects.h"

#include "MyMqttClient.h"
#include <Arduino.h>

#include "timeUtils.h"
#include "led.h"



class ShutterRelais: public BaseObject{
    private:
        int pinUp;
        int pinDown;
        const char* statusTopic;
        const char* setTopic;
        const char* upMsg;
        const char* stopMsg;
        const char* downMsg;
        const char* msgOpen;
        const char* msgClose;
    public:
        ShutterRelais(int pinUp, int pinDown, const char* statusTopic, const char* setTopic, const char* upMsg, const char* stopMsg, const char* downMsg, const char* msgOpen, const char* msgClose){
            this->pinUp = pinUp;
            this->pinDown = pinDown;
            this->statusTopic = statusTopic;
            this->setTopic = setTopic;
            this->upMsg = upMsg;
            this->stopMsg = stopMsg;
            this->downMsg = downMsg;
            this->msgOpen = msgOpen;
            this->msgClose = msgClose;

            pinMode(pinUp, OUTPUT);
            pinMode(pinDown, OUTPUT);
            digitalWrite(pinUp, LOW);
            digitalWrite(pinDown, LOW);
        }
        void subscribeToMqtt() override{
            MyMQTTClient::getInstance()->subscribe((char *) setTopic, qtAT_LEAST_ONCE);
        }
        void msgArrived(char *topic, char *data, bool retain, bool duplicate) override{
           if(strcmp (topic, statusTopic) == 0){ // handle only if subscribed topic
                for (char *iter = data; *iter != '\0'; ++iter) { // to lower
                    *iter = std::tolower(*iter);
                    ++iter;
                }

                // Open
                if ( strstr(data, upMsg)) {

                    // Set Pin
                    digitalWrite(pinDown, LOW);
                    delay(100);
                    digitalWrite(pinUp, HIGH);

                    // Update MQTT Status
                    if (!MyMQTTClient::getInstance()->publish((char *) statusTopic, (char *) msgOpen, qtAT_LEAST_ONCE,true,false)) {
                        Serial.println("MQTT Send Status Open Shutter failed");
                    }

                // Close
                }else if ( strstr(data, downMsg)) {

                    // Set Pin
                    digitalWrite(pinUp, LOW);
                    delay(100);
                    digitalWrite(pinDown, HIGH);

                    // Update MQTT Status
                    if (!MyMQTTClient::getInstance()->publish((char *) statusTopic, (char *) msgClose, qtAT_LEAST_ONCE,true,false)) {
                        Serial.println("MQTT Send Status Close Shutter failed");
                    }
                // Stop
                }else if ( strstr(data, stopMsg)) {

                    // Set Pin
                    digitalWrite(pinUp, LOW);
                    digitalWrite(pinDown, LOW);

                }else{
                    Serial.println("MQTT Recieved unknown Message in topic");   
                    Serial.println(topic);
                    Serial.println(data);
                    Serial.println();
                }

            }
        }
        void checkForAction() override{
           return;
        }
        String getHtmlInfo() override{
            String html = "Shutter Relais on pin <b>";
            html += pinUp;
            html += "</b> and <b>";
            html += pinDown;
            html += "</b>";
            return html;
        }
};


#endif