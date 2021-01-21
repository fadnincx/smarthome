#ifndef SMART_ESP_CONFIG_H
#define SMART_ESP_CONFIG_H

#include <Arduino.h>
#include <list>
#include <smarthomeObjects.h>

class Config {
    public:
        static Config* getInstance();
    private:
        static Config* instance;
        String configString;
        std::list<BaseObject*> list;

        const char* availableTopic = "";
        const char* availableOnlineMessage = "online";
        const char* availableOfflineMessage = "offline";

        void loadConfigFromServer();
        void parseConfig();
        Config();
    public:
        void loadConfig();

        void subscribeAllObjects();
        void distributePacketToAllObjects(char *topic, char *data, bool retain, bool duplicate);
        void checkAllForAction();
        String getNthStatus(size_t n);
        String toggleNth(size_t n);
        String getFixedHtmlTiles();
        String getStringLoadedConfig();

        const char* getAvailableTopic(){
            return availableTopic;
        }
        const char* getAvailableOnlineMessage(){
            return availableOnlineMessage;
        }
        const char* getAvailableOfflineMessage(){
            return availableOfflineMessage;
        }
};

#endif