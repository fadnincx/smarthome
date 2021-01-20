#ifndef SMART_ESP_SMARTHOMEOBJECTS_LIGHT_RELAIS
#define SMART_ESP_SMARTHOMEOBJECTS_LIGHT_RELAIS

#include "smarthomeObjects.h"

#include "MyMqttClient.h"
#include <Arduino.h>

#include "timeUtils.h"
#include "led.h"



class LightRelais: public BaseObject{
    private:
        int pin;
        const char* statusTopic;
        const char* setTopic;
        const char* offMsg;
        const char* onMsg;
        const char* msgOff;
        const char* msgOn;
    public:
        LightRelais(int pin, const char* statusTopic, const char* setTopic,  const char* offMsg, const char* onMsg, const char* msgOff, const char* msgOn){
            this->pin = pin;
            this->statusTopic = statusTopic;
            this->setTopic = setTopic;
            this->offMsg = offMsg;
            this->onMsg = onMsg;
            this->msgOff = msgOff;
            this->msgOn = msgOn;
            pinMode(pin, OUTPUT);
            digitalWrite(pin, LOW); // TODO: not always low, but what is currently in mqtt...
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

                // Turn ON
                if ( strstr(data, "on")) {

                    // Set Pin
                    digitalWrite(pin, HIGH);

                    // Update MQTT Status
                    if (!MyMQTTClient::getInstance()->publish((char *) statusTopic, (char *) msgOn, qtAT_LEAST_ONCE,true,false)) {
                        Serial.println("MQTT Send Turn ON Light failed");
                    }

                // Turn OFF  
                }else if ( strstr(data, "off")) {

                    // Set Pin
                    digitalWrite(pin, LOW);

                    // Update MQTT Status
                    if (!MyMQTTClient::getInstance()->publish((char *) statusTopic, (char *) msgOff, qtAT_LEAST_ONCE,true,false)) {
                        Serial.println("MQTT Send Turn OFF Light failed");
                    }
                
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
            String html = "Relais on pin <b>";
            html += pin;
            html += "</b>";
            return html;
        }
        
};



#endif