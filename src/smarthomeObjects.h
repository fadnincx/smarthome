#ifndef SMART_ESP_SMARTHOMEOBJECTS
#define SMART_ESP_SMARTHOMEOBJECTS

class BaseObject{
    public:
        virtual void subscribeToMqtt(){}
        virtual void msgArrived(char *topic, char *data, bool retain, bool duplicate){}
        virtual void checkForAction(){}
        virtual void toggle(){}
        virtual String getStatus(){ return "";}
        virtual String getFixedHtmlTile(int n){return "";}
};


#endif