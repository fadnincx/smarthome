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



// TODO

// Define mqtt topic whether it is currently day or night.
// Used since i have troubles with NTP
#define SUN_STATE_TOPIC "sun"

// Define NTP
#define NTP_HOST "192.168.1.2"

// Define LED Pin
#define LED_PIN 2

// Define Arduino Pins on ESP available
#define ALLOWED_PINS {0, 1, 2, 3, 4, 5, 12, 13, 14, 15, 16}
#define NR_ALLOWED_PINS 11


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

}