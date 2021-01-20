#include "myMqttClient.h"

#include <ESP8266WiFi.h>

// MQTT Client by https://github.com/bkeevil/arduino-mqtt/ Stand: 1.9.2019
#include "mqtt.h"

#include "config.h"
#include "wifiConnection.h"
#include "secrets.h"
#include "name.h"

MyMQTTClient* MyMQTTClient::getInstance(){
    return (!instance) ? instance = new MyMQTTClient() : instance;
}
MyMQTTClient* MyMQTTClient::instance = 0;
MyMQTTClient::MyMQTTClient(){}

void MyMQTTClient::connectToMQTT() {

    // Set Stream
    this->stream = &wifiClient;

    // Configure Last Will Message -> offline information
    strcpy(this->willMessage.topic, Config::getInstance()->getAvailableTopic());
    strcpy(this->willMessage.data, Config::getInstance()->getAvailableOfflineMessage());
    this->willMessage.enabled = true;
    this->willMessage.retain = true;
    this->willMessage.qos = qtAT_LEAST_ONCE;

    //Connect

    this->connect((char*) Name::getInstance()->getName(),(char*) MQTT_USER,(char*) MQTT_PASSWORD, true);

    sendOnline = false;
}

// Events
void MyMQTTClient::connected() {
    Serial.println("Connected to MQTT");
    MQTTClient::connected();
};
void MyMQTTClient::initSession() {
    Serial.println("Initializing Subscriptions");
    Config::getInstance()->subscribeAllObjects();
    
    MQTTClient::initSession();
};
bool MyMQTTClient::subscribe(char *filter, byte qos){
    usedSubscriptionPacketId++;
    return MQTTClient::subscribe(usedSubscriptionPacketId, filter, qos);
}
void MyMQTTClient::subscribed(word packetID, byte resultCode) {
    Serial.print("Subscribed "); 
    Serial.print(packetID); 
    Serial.print(" "); 
    Serial.println(resultCode);
    MQTTClient::subscribed(packetID,resultCode);
};
void MyMQTTClient::unsubscribed(word packetID) {
    Serial.print("Unsubscribed "); 
    Serial.println(packetID);
    MQTTClient::unsubscribed(packetID);
};
void MyMQTTClient::receiveMessage(char *topic, char *data, bool retain, bool duplicate) {

    Config::getInstance()->distributePacketToAllObjects(topic, data, retain, duplicate);

};

void MyMQTTClient::mqttConnectToServer() {

    // Print debug information
    Serial.println("Connecting to MQTT ...");

    Serial.println("Establishing TCP Connection");
    if (!wifiClient.connect(MQTT_HOST, MQTT_PORT)){
        Serial.println("TCP Connection Failed");
        int counter = 0;
        while(!wifiClient.connect(MQTT_HOST, MQTT_PORT) && counter < 5){
        counter++;
        delay(250);
        }
        if(counter >= 5){
        Serial.println("Unable to connect TCP");
        ESP.restart();
        }
    
    }
    Serial.println("TCP Connected");    
    connectToMQTT();
}
void MyMQTTClient::mqttMaintain(){
    byte errorCode;

    if(!wifiClient.connected()){
        if(WiFi.status() != WL_CONNECTED){
            WifiConnection::connectIfNotConnected();
        }
        mqttConnectToServer();
    }

    if (wifiClient.available() > 1) {
        Serial.print(wifiClient.available()); Serial.println(" bytes available");
        
        errorCode = this->dataAvailable();
        
        if (errorCode != MQTT_ERROR_NONE) {
        Serial.print("Error code "); Serial.println(errorCode);
        }
        
    } else {
        
        if (this->isConnected) {
            c++;
            d++;
            
            if (c==10) {
                errorCode = this->intervalTimer();
                if (errorCode != 0) {
                    Serial.print("intervalTimer Error "); Serial.println(errorCode);
                    ESP.restart();
                }
                c = 0;
            }

            if(!sendOnline){
                if (!this->publish((char *) Config::getInstance()->getAvailableTopic(),(char *) Config::getInstance()->getAvailableOnlineMessage(),qtAT_LEAST_ONCE,true,false)) {
                    Serial.print("MQTT Send Available status failed");
                } else {
                    Serial.print("MQTT Send Available status successfull");
                    sendOnline = true;
                }
            }
        
        } else {
            mqttConnectToServer();
        }
        
        wifiClient.flush();  
        delay(50);
    }
}

