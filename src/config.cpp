
#include "config.h"

#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <list>

#include "name.h"
#include "secrets.h"
#include "smarthomeObjects.h"
#include "smarthomeObjectSwitch.h"
#include "smarthomeObjectShutter.h"
#include "smarthomeObjectLightRelais.h"
#include "smarthomeObjectShutterRelais.h"

Config* Config::getInstance(){
    return (!instance) ? instance = new Config() : instance;
}
Config* Config::instance = 0;
Config::Config(){}

void Config::loadConfigFromServer(){
    HTTPClient http;
    Serial.println("[HTTP] begin...\n");
    http.begin(CONFIG_URL);

    // start connection and send HTTP header
    int httpCode = http.GET();
    if(httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if(httpCode == HTTP_CODE_OK) {

            // get lenght of document (is -1 when Server sends no Content-Length header)
            int len = http.getSize();

            // get tcp stream
            WiFiClient * stream = http.getStreamPtr();

            // read all data from server
            while(http.connected() && (len > 0 || len == -1)) {

                if(stream->available()){
                    configString += stream->read();
                }
                
            }

            Serial.println();
            Serial.println("[HTTP] connection closed or file end.\n");
            
        }
        
    }
    http.end();
}
void Config::parseConfig(){
    StaticJsonDocument<2048> doc;    
    DeserializationError error = deserializeJson(doc, configString);
    
    // Test if parsing succeeds.
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }

    // Set Name
    if(!doc.containsKey("name")){
        Serial.println("No name in JSON!");
        return;
    }
    Name::getInstance()->setName(doc["name"]);

    // Set Available Topic
    if(doc.containsKey("availableTopic")){
        availableTopic = doc["availableTopic"];
    }

    // Has Switch
    if(doc.containsKey("switch")){
        for(JsonObject elem: doc["switch"].as<JsonArray>() ) {
            if(elem.containsKey("pin") &&
            elem.containsKey("statusTopic") &&
            elem.containsKey("setTopic") &&
            elem.containsKey("msgOff") &&
            elem.containsKey("msgOn") ){
                if(elem.containsKey("msgDark")){
                    Switch s = Switch(
                        elem["pin"],
                        elem["statusTopic"],
                        elem["setTopic"],
                        elem["msgOff"],
                        elem["msgOn"],
                        elem["msgDark"]
                    );
                    list.push_back(s);
                } else {
                    Switch s = Switch(
                        elem["pin"],
                        elem["statusTopic"],
                        elem["setTopic"],
                        elem["msgOff"],
                        elem["msgOn"]
                    );
                    list.push_back(s);
                }
            }
        }
    }
    // Has Shutter
    if(doc.containsKey("shutter")){
        for(JsonObject elem: doc["shutter"].as<JsonArray>() ) {
            if(elem.containsKey("pinUp") &&
            elem.containsKey("pinDown") &&
            elem.containsKey("statusTopic") &&
            elem.containsKey("setTopic") &&
            elem.containsKey("msgUp") &&
            elem.containsKey("msgStop") &&
            elem.containsKey("msgDown") ){     
                Shutter s = Shutter(
                    elem["pinUp"],
                    elem["pinDown"],
                    elem["statusTopic"],
                    elem["setTopic"],
                    elem["msgUp"],
                    elem["msgStop"],
                    elem["msgDown"]
                );
                list.push_back(s); 
            }
        }
    }
    // Has Light Relais
    if(doc.containsKey("lightRelais")){
        for(JsonObject elem: doc["lightRelais"].as<JsonArray>() ) {
            if(elem.containsKey("pin") &&
            elem.containsKey("statusTopic") &&
            elem.containsKey("setTopic") &&
            elem.containsKey("offMsg") &&
            elem.containsKey("onMsg") &&
            elem.containsKey("msgOff") &&
            elem.containsKey("msgOn") ){
                LightRelais r = LightRelais(
                    elem["pin"],
                    elem["statusTopic"],
                    elem["setTopic"],
                    elem["offMsg"],
                    elem["onMsg"],
                    elem["msgOff"],
                    elem["msgOn"]
                );
                list.push_back(r);
            }
        }
    }
    // Has Shutter Relais
    if(doc.containsKey("shutterRelais")){
        for(JsonObject elem: doc["shutterRelais"].as<JsonArray>() ) {
            if(elem.containsKey("pinUp") &&
            elem.containsKey("pinDown") &&
            elem.containsKey("statusTopic") &&
            elem.containsKey("setTopic") &&
            elem.containsKey("upMsg") &&
            elem.containsKey("stopMsg") &&
            elem.containsKey("downMsg") &&
            elem.containsKey("msgOpen") &&
            elem.containsKey("msgClose") ){     
                ShutterRelais r = ShutterRelais(
                    elem["pinUp"],
                    elem["pinDown"],
                    elem["statusTopic"],
                    elem["setTopic"],
                    elem["upMsg"],
                    elem["stopMsg"],
                    elem["downMsg"],
                    elem["msgOpen"],
                    elem["msgClose"]
                );
                list.push_back(r); 
            }
        }
    }

}
    
void Config::loadConfig(){
    loadConfigFromServer();
    parseConfig();
}

void Config::subscribeAllObjects(){
    std::list<BaseObject>::iterator it;
    for(it = list.begin(); it != list.end(); it++){
        it->subscribeToMqtt();
    }
}
void Config::distributePacketToAllObjects(char *topic, char *data, bool retain, bool duplicate){
    std::list<BaseObject>::iterator it;
    for(it = list.begin(); it != list.end(); it++){
        it->msgArrived(topic, data,  retain, duplicate);
    }
}
void Config::checkAllForAction(){
    std::list<BaseObject>::iterator it;
    for(it = list.begin(); it != list.end(); it++){
        it->checkForAction();
    }
}
String Config::getHtmlInfo(){
    std::list<BaseObject>::iterator it;
    String html = "";
    for(it = list.begin(); it != list.end(); it++){
        html += it->getHtmlInfo();
        html += "<br>";
    }
    return html;
}

