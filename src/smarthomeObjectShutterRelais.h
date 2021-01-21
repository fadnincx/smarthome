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
        virtual void subscribeToMqtt() override{
            MyMQTTClient::getInstance()->subscribe((char *) setTopic, qtAT_LEAST_ONCE);
        }
        virtual void msgArrived(char *topic, char *data, bool retain, bool duplicate) override{
           if(strcmp (topic, setTopic) == 0){ // handle only if subscribed topic
                for (char *iter = data; *iter != '\0'; ++iter) { // to lower
                    *iter = std::tolower(*iter);
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
        virtual String getFixedHtmlTile(int n){
            String html = "<div class=\'shutterRelais\' style='border: 1px solid black; padding: 10px; margin: 10px; width: 300px;'>";
            html+="<h3>Shutter-Relais</h3>";
            html+="<p>Pin ";
            html+=pinUp;
            html+=" and Pin ";
            html+=pinDown;
            html+="</p>";
            html+="<p>Up <span id='shutterRelais-";
            html+=n;
            html+="-up'>N/A</span></p>";
            html+="<p>Down <span id='shutterRelais-";
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
            html+="            document.getElementById('shutterRelais-";
            html+=n;
            html+="-up').innerHTML = d.up;";
            html+="            document.getElementById('shutterRelais-";
            html+=n;
            html+="-down').innerHTML = d.down;";
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
            String json = "{\"up\":\"";
            json += digitalRead(pinUp)==LOW?"OFF":"ON";
            json += "\",\"down\":\"";
            json += digitalRead(pinDown)==LOW?"OFF":"ON";
            json += "\"}";
            return json;
        }
};


#endif