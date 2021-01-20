#ifndef SMART_ESP_MY_MQTT_CLIENT
#define SMART_ESP_MY_MQTT_CLIENT

#include <ESP8266WiFi.h>

// MQTT Client by https://github.com/bkeevil/arduino-mqtt/ Stand: 1.9.2019
#include "mqtt.h"

#include "config.h"
#include "wifiConnection.h"


class MyMQTTClient: public MQTTClient {
    public:
        static MyMQTTClient* getInstance();
    private:
        static MyMQTTClient* instance;
        bool sendOnline = false;
        WiFiClient wifiClient;

        byte c=0,d=0;
        long a0=0,a1=100,a2=1000;
        char buffer[6];

        word usedSubscriptionPacketId = 0;

        void connectToMQTT();
        MyMQTTClient();
    public:
        // Events
        void connected() override;
        void initSession() override;
        bool subscribe(char *filter, byte qos);
        void subscribed(word packetID, byte resultCode) override;
        void unsubscribed(word packetID) override;
        void receiveMessage(char *topic, char *data, bool retain, bool duplicate) override;
        void mqttConnectToServer();
        void mqttMaintain();

};

#endif