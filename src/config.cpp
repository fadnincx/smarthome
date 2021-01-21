
#include "config.h"

#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <list>
#include <algorithm>

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

            configString = http.getString();

            Serial.println(configString);
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
                    list.push_back( new Switch(
                        elem["pin"],
                        elem["statusTopic"],
                        elem["setTopic"],
                        elem["msgOff"],
                        elem["msgOn"],
                        elem["msgDark"]
                    ));
                } else {
                    list.push_back( new Switch(
                        elem["pin"],
                        elem["statusTopic"],
                        elem["setTopic"],
                        elem["msgOff"],
                        elem["msgOn"]
                    ));
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
                list.push_back( new Shutter(
                    elem["pinUp"],
                    elem["pinDown"],
                    elem["statusTopic"],
                    elem["setTopic"],
                    elem["msgUp"],
                    elem["msgStop"],
                    elem["msgDown"]
                )); 
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
                list.push_back( new LightRelais(
                    elem["pin"],
                    elem["statusTopic"],
                    elem["setTopic"],
                    elem["offMsg"],
                    elem["onMsg"],
                    elem["msgOff"],
                    elem["msgOn"]
                ));
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
                list.push_back( new ShutterRelais(
                    elem["pinUp"],
                    elem["pinDown"],
                    elem["statusTopic"],
                    elem["setTopic"],
                    elem["upMsg"],
                    elem["stopMsg"],
                    elem["downMsg"],
                    elem["msgOpen"],
                    elem["msgClose"]
                ));
            }
        }
    }

}
    
void Config::loadConfig(){
    loadConfigFromServer();
    parseConfig();
    Serial.println("Finished loading Configuration");
}

void Config::subscribeAllObjects(){
    for_each(list.begin(), list.end(), [](decltype(*begin(list)) act){act->subscribeToMqtt();});
}
void Config::distributePacketToAllObjects(char *topic, char *data, bool retain, bool duplicate){
    for_each(list.begin(), list.end(), [&](decltype(*begin(list)) act){act->msgArrived(topic, data, retain, duplicate);});
}
void Config::checkAllForAction(){
    for_each(list.begin(), list.end(), [](decltype(*begin(list)) act){act->checkForAction();});
}
String Config::getNthStatus(size_t n){
    if(list.size() > n){ 
        std::list<BaseObject*>::iterator it = list.begin();
        advance(it, n);
        return (*it)->getStatus();
    }
    return "";
}
String Config::getFixedHtmlTiles(){
    String html = "";
    int index = 0;
    for_each(list.begin(), list.end(), [&](decltype(*begin(list)) act){
        html += act->getFixedHtmlTile(index);
        index++;
    });
    return html;
}
String Config::toggleNth(size_t n){
    if(list.size() > n){ 
        std::list<BaseObject*>::iterator it = list.begin();
        advance(it, n);
        (*it)->toggle();
        return (*it)->getStatus();
    }
    return "";
}
String Config::getStringLoadedConfig(){
    return configString;
}

