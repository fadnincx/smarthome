// Include Arduino
#include <Arduino.h>

// Config
#include "config.h"

// Webserver and OTA
#include "webservice.h" 

// Time
#include "timeUtils.h"

// Secrets
#include "secrets.h"

// Wifi Connection
#include "wifiConnection.h"

// Mqtt
#include "MyMqttClient.h"

// Led
#include "led.h"



/**
 * Arduino Setup Routine
 **/
void setup() {

  // Initialize Serial Connection
  Serial.begin(115200);
  Serial.println();

  // Initialize Wifi Connection
  Serial.println("Init Wifi");
  WifiConnection::connectToWifi();

  // Load Config from Server
  Config::getInstance()->loadConfig();

  // Initialize Time
  Time::getInstance();

  // Initialize Mqtt Connection
  MyMQTTClient::getInstance()->mqttConnectToServer();

  // Initialize Webservice
  initializeWebservice();

  Serial.println("Finished Setup");
  
}

/**
 * Arduino Loop Routine
 **/
void loop() {

  // Check if wifi Online
  WifiConnection::connectIfNotConnected();
  
  // Check all switch, relais, ... for action
  Config::getInstance()->checkAllForAction();

  // MQTT Process
  MyMQTTClient::getInstance()->mqttMaintain();

  // Process Webservice
  processWeb();

  // Process Led
  Led::getInstance()->blinkLoop();

}