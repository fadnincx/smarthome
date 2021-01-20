#ifndef SMART_ESP_SMARTHOMEOBJECTS
#define SMART_ESP_SMARTHOMEOBJECTS

class BaseObject{
    public:
        virtual void subscribeToMqtt(){}
        virtual void msgArrived(char *topic, char *data, bool retain, bool duplicate){}
        virtual void checkForAction(){}
        virtual String getHtmlInfo(){
            return "";
        }
};


#endif