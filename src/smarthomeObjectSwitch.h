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
        virtual void subscribeToMqtt() override{
            MyMQTTClient::getInstance()->subscribe((char *) statusTopic, qtAT_LEAST_ONCE);
        }
        virtual void msgArrived(char *topic, char *data, bool retain, bool duplicate) override{
            if(strcmp (topic, statusTopic) == 0){ // handle only if subscribed topic
                for (char *iter = data; *iter != '\0'; ++iter) { // to lower
                    *iter = std::tolower(*iter);
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
        virtual void checkForAction() override{
            //Serial.print("Check switch on pin ");Serial.print(pin);Serial.println("!");
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
                        Serial.println("MQTT Send Off failed");
                        }
                        Led::getInstance()->blinkLed();
                    }
                
                }
                
            }
        }
        virtual String getFixedHtmlTile(int n){
            String html = "<div class=\'switch\' style='border: 1px solid black; padding: 10px; margin: 10px; width: 300px;'>";
            html+="<h3>Switch</h3>";
            html+="<p>Pin ";
            html+=pin;
            html+="</p>";
            html+="<p>Physically <span id='switch-";
            html+=n;
            html+="-physical'>N/A</span></p>";
            html+="<p>Online <span id='switch-";
            html+=n;
            html+="-online'>N/A</span></p>";
            html+="<button type='button' onclick='toggle";
            html+=n;
            html+="()'>Toggle Switch</button>";
            html+="<script>";
            html+="setInterval(function() {";
            html+="    getData";
            html+=n;
            html+="();";
            html+="}, 2000);";
            html+="function getData";
            html+=n;
            html+="(){";
            html+="    var xhttp = new XMLHttpRequest();";
            html+="    xhttp.onreadystatechange = function() {";
            html+="        if (this.readyState == 4 && this.status == 200) {";
            html+="            var d = JSON.parse(this.responseText);";
            html+="            document.getElementById('switch-";
            html+=n;
            html+="-physical').innerHTML = d.physical;";
            html+="            document.getElementById('switch-";
            html+=n;
            html+="-online').innerHTML = d.online;";
            html+="        }";
            html+="    };";
            html+="    xhttp.open('GET', 'status/";
            html+=n;
            html+="', true);";
            html+="    xhttp.send();";
            html+="}";
            html+="function toggle";
            html+=n;
            html+="(){";
            html+="    var xhttp = new XMLHttpRequest();";
            html+="    xhttp.onreadystatechange = function() {";
            html+="        if (this.readyState == 4 && this.status == 200) {";
            html+="            var d = JSON.parse(this.responseText);";
            html+="            document.getElementById('switch-";
            html+=n;
            html+="-physical').innerHTML = d.physical;";
            html+="            document.getElementById('switch-";
            html+=n;
            html+="-online').innerHTML = d.online;";
            html+="        }";
            html+="    };";
            html+="    xhttp.open('GET', 'toggle/";
            html+=n;
            html+="', true);";
            html+="    xhttp.send();";
            html+="}";
            html+="</script>";
            html+="</div>";
            
            return html;
        }
        virtual String getStatus() override{
            String json = "{\"physical\":\"";
            json += physicalState==HIGH?"not pressed":"pressed";
            json += "\", \"online\":\"";
            json += onlineState==1?"ON":"OFF";
            json += "\"}";
            return json;
        }
        virtual void toggle(){
            if (onlineState == 0) {    
                if (!MyMQTTClient::getInstance()->publish((char *) setTopic,(char *) msgOn,qtAT_LEAST_ONCE,false,false)) {
                    Serial.println("MQTT Send ON failed");
                }
            } else {
                if (!MyMQTTClient::getInstance()->publish((char *) setTopic,(char *) msgOff,qtAT_LEAST_ONCE,false,false)) {
                    Serial.println("MQTT Send Off failed");
                }
            }
        }

};

#endif