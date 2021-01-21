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
        bool initialSet = false;
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
            //digitalWrite(pin, LOW); // TODO: not always low, but what is currently in mqtt...
        }
        virtual void subscribeToMqtt() override{
            MyMQTTClient::getInstance()->subscribe((char *) statusTopic, qtAT_LEAST_ONCE);
            MyMQTTClient::getInstance()->subscribe((char *) setTopic, qtAT_LEAST_ONCE);
        }
        virtual void msgArrived(char *topic, char *data, bool retain, bool duplicate) override{
            if(strcmp (topic, setTopic) == 0){ // handle only if subscribed topic
                for (char *iter = data; *iter != '\0'; ++iter) { // to lower
                    *iter = std::tolower(*iter);
                }

                // Turn ON
                if ( strstr(data, onMsg)) {

                    // Set Pin
                    digitalWrite(pin, HIGH);

                    // Update MQTT Status
                    if (!MyMQTTClient::getInstance()->publish((char *) statusTopic, (char *) msgOn, qtAT_LEAST_ONCE,true,false)) {
                        Serial.println("MQTT Send Turn ON Light failed");
                    }

                // Turn OFF  
                }else if ( strstr(data, offMsg)) {

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

            }else if(!initialSet && strcmp (topic, statusTopic) == 0){ // handle only if subscribed topic
                for (char *iter = data; *iter != '\0'; ++iter) { // to lower
                    *iter = std::tolower(*iter);
                }

                // Turn ON
                if ( strstr(data, onMsg)) {

                    // Set Pin
                    digitalWrite(pin, HIGH);

                // Turn OFF  
                }else if ( strstr(data, offMsg)) {

                    // Set Pin
                    digitalWrite(pin, LOW);
                
                }else{
                    Serial.println("MQTT Recieved unknown Message in topic");   
                    Serial.println(topic);
                    Serial.println(data);
                    Serial.println();
                }
                initialSet = true;
            }
        }
         virtual String getFixedHtmlTile(int n){
            String html = "<div class=\'lightRelais\' style='border: 1px solid black; padding: 10px; margin: 10px; width: 300px;'>";
            html+="<h3>Light Relais</h3>";
            html+="<p>Pin ";
            html+=pin;
            html+="</p>";
            html+="<p>Status <span id='relais-";
            html+=n;
            html+="-state'>N/A</span></p>";
            html+="<button type='button' onclick='toggle";
            html+=n;
            html+="()'>Toggle Relais</button>";
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
            html+="            document.getElementById('relais-";
            html+=n;
            html+="-state').innerHTML = d.state;";
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
            html+="            document.getElementById('relais-";
            html+=n;
            html+="-physical').innerHTML = d.state;";
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
            String json = "{\"state\":\"";
            json += digitalRead(pin)==HIGH?"ON":"OFF";
            json += "\"}";
            return json;
        }
        virtual void toggle(){
            if (digitalRead(pin) == LOW) {    
                digitalWrite(pin, HIGH);
                if (!MyMQTTClient::getInstance()->publish((char *) statusTopic, (char *) msgOn, qtAT_LEAST_ONCE,true,false)) {
                    Serial.println("MQTT Send Turn ON Light failed");
                }
            } else {
                digitalWrite(pin, LOW);
                if (!MyMQTTClient::getInstance()->publish((char *) statusTopic, (char *) msgOff, qtAT_LEAST_ONCE,true,false)) {
                    Serial.println("MQTT Send Turn OFF Light failed");
                }
            }
        }
        
};



#endif