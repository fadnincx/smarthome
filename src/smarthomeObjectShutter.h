#ifndef SMART_ESP_SMARTHOMEOBJECTS_SHUTTER
#define SMART_ESP_SMARTHOMEOBJECTS_SHUTTER

#include "smarthomeObjects.h"

#include "MyMqttClient.h"
#include <Arduino.h>

#include "timeUtils.h"
#include "led.h"




class Shutter: public BaseObject{
    private:
        int pinUp;
        int pinDown;
        const char* statusTopic;
        const char* setTopic;
        const char* msgUp;
        const char* msgStop;
        const char* msgDown;  
        int shutterUpPhysicalState;
        int shutterDownPhysicalState;
        unsigned long shutterUpTimeLastPressed = 0;
        unsigned long shutterDownTimeLastPressed = 0;
        unsigned long shutterStopTimeLastPressed = 0;
        bool shutterUpLastPressHandled = true;
        bool shutterDownLastPressHandled = true;
        bool shutterStopLastPressHandled = true;
    public:
        Shutter(int pinUp, int pinDown, const char* statusTopic, const char* setTopic, const char* msgUp, const char* msgStop, const char* msgDown){
            this->pinUp = pinUp;
            this->pinDown = pinDown;
            this->statusTopic = statusTopic;
            this->setTopic = setTopic;
            this->msgUp = msgUp;
            this->msgStop = msgStop;
            this->msgDown = msgDown;
            pinMode(pinUp, INPUT_PULLUP);
            pinMode(pinDown, INPUT_PULLUP);
            shutterUpPhysicalState = digitalRead( pinUp );
            shutterDownPhysicalState = digitalRead( pinDown );
        }
        virtual void subscribeToMqtt() override{
            MyMQTTClient::getInstance()->subscribe((char *) statusTopic, qtAT_LEAST_ONCE);
        }
        virtual void msgArrived(char *topic, char *data, bool retain, bool duplicate) override{
           return;
        }
        virtual void checkForAction() override{
            // Check Shutter Up Pressed
            if( digitalRead( pinUp ) == LOW && shutterUpPhysicalState == HIGH && shutterUpLastPressHandled) {
                shutterUpLastPressHandled = false;
                shutterUpTimeLastPressed = millis();
            }
            // Check Shutter Down Pressed
            if( digitalRead( pinDown ) == LOW && shutterDownPhysicalState == HIGH && shutterDownLastPressHandled) {
                shutterDownLastPressHandled = false;
                shutterDownTimeLastPressed = millis();
            }
            // Check Shutter Down and Up released
            if( (shutterUpPhysicalState == LOW || shutterDownPhysicalState == LOW) && digitalRead( pinUp ) == HIGH && digitalRead( pinDown ) == HIGH && shutterStopLastPressHandled) {
                shutterStopLastPressHandled = false;
                shutterStopTimeLastPressed = millis();
            }
            // Hande if 50ms
            // Handle Shutter Up Pressed
            if( digitalRead( pinUp ) == LOW && shutterUpPhysicalState == HIGH && !shutterUpLastPressHandled && millis() - shutterUpTimeLastPressed > 50) {
                shutterUpLastPressHandled = true;
                shutterUpPhysicalState = LOW;
                if (!MyMQTTClient::getInstance()->publish((char *) setTopic,(char *) msgUp,qtAT_LEAST_ONCE,false,false)) {
                    Serial.println("MQTT Send Up failed");
                }
                Led::getInstance()->blinkLed();
            }
            // Handle Shutter Down Pressed
            if( digitalRead( pinDown ) == LOW && shutterDownPhysicalState == HIGH && !shutterDownLastPressHandled && millis() - shutterDownTimeLastPressed > 50) {
                shutterDownLastPressHandled = true;
                shutterDownPhysicalState = LOW;
                if (!MyMQTTClient::getInstance()->publish((char *) setTopic,(char *) msgDown,qtAT_LEAST_ONCE,false,false)) {
                    Serial.println("MQTT Send Down failed");
                }
                Led::getInstance()->blinkLed();
            }
            // Handle Shutter Down and Up released
            if( (shutterUpPhysicalState == LOW || shutterDownPhysicalState == LOW) && digitalRead( pinUp ) == HIGH && digitalRead( pinDown ) == HIGH && !shutterStopLastPressHandled && millis() - shutterStopTimeLastPressed > 50) {
                shutterStopLastPressHandled = true;
                shutterUpPhysicalState = HIGH;
                shutterDownPhysicalState = HIGH;
                if (!MyMQTTClient::getInstance()->publish((char *) setTopic,(char *) msgStop,qtAT_LEAST_ONCE,false,false)) {
                    Serial.println("MQTT Send Stop failed");
                }
                Led::getInstance()->blinkLed();
            }
        }
        virtual String getFixedHtmlTile(int n){
            String html = "<div class=\'shutter\' style='border: 1px solid black; padding: 10px; margin: 10px; width: 300px;'>";
            html+="<h3>Shutter</h3>";
            html+="<p>Pin ";
            html+=pinUp;
            html+=" and Pin ";
            html+=pinDown;
            html+="</p>";
            html+="<p>Physically Up <span id='shutter-";
            html+=n;
            html+="-up'>N/A</span></p>";
            html+="<p>Physically Down <span id='shutter-";
            html+=n;
            html+="-down'>N/A</span></p>";
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
            html+="            document.getElementById('shutter-";
            html+=n;
            html+="-up').innerHTML = d.physicalUp;";
            html+="            document.getElementById('shutter-";
            html+=n;
            html+="-down').innerHTML = d.physicalDown;";
            html+="        }";
            html+="    };";
            html+="    xhttp.open('GET', 'status/";
            html+=n;
            html+="', true);";
            html+="    xhttp.send();";
            html+="}";
            html+="</script>";
            html+="</div>";
            
            return html;
        }
        virtual String getStatus() override{
            String json = "{\"physicalUp\":\"";
            json += shutterUpPhysicalState==HIGH?"not pressed":"pressed";
            json += "\",\"physicalDown\":\"";
            json += shutterDownPhysicalState==HIGH?"not pressed":"pressed";
            json += "\"}";
            return json;
        }
};

#endif