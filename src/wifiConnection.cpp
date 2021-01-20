#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "secrets.h"

#include "name.h"
#include "MyMqttClient.h"


void WifiConnection::connectToWifi() {
            
    // Set Wifi Hostname
    WiFi.hostname(Name::getInstance()->getName());

    // Set Wifi Mode to STA -> Station Mode -> as Wifi Client, not as Access Point
    WiFi.mode(WIFI_STA);

    Serial.print("Wifi Hostname is:");
    Serial.println(WiFi.hostname());

    // Set SSID and Password to connect
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // Try to connect to Wifi
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        // If connection fails

        // Print Debug Information
        Serial.println("Connection Failed! Rebooting...");

        // Wait 5s
        delay(5000);

        // Reboot
        ESP.restart();
        
    }

    // Successfully connected
    Serial.println("Connected to wifi");
}
void WifiConnection::connectIfNotConnected(){
    // Check if still connected to the Wifi, else try to reconnect

    // Set Try counter
    int wifi_retry = 0;

    //Try 5 Times to connect to WiFi
    while( WiFi.status() != WL_CONNECTED && wifi_retry < 5 ) {

        // Increase try counter
        wifi_retry++;

        // Print Debug information
        Serial.println("WiFi not connected. Try to reconnect");

        // Fully Disconnect from Wifi
        WiFi.disconnect();

        // Set Wifi Hostname
        WiFi.hostname(Name::getInstance()->getName());

        // Set Wifi Mode to STA
        WiFi.mode(WIFI_STA);

        // Set SSID and password to connect to
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        // Wait 100ms before check again
        delay(100);
    }

    // If Failed 5 Times, then reboot
    if( wifi_retry >= 5 ) {

        // Print debug info
        Serial.println("\nReboot");

        // Reboot
        ESP.restart();
        
    }
    if(wifi_retry >0){
        MyMQTTClient::getInstance()->mqttConnectToServer();
    }
}
