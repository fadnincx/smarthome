# smarthome


Need to add `secret.h` with the following content:
```
#ifndef SECRETS_H
#define SECRETS_H

// Define Wifi Credentials
#define WIFI_SSID "yourWiFissid"
#define WIFI_PASSWORD "yourWiFipassword"

// Define Configuration URL
#define CONFIG_URL "http://yourConfigUrl"

// Define MQTT Credentials
#define MQTT_HOST "192.168.1.1"
#define MQTT_PORT 1883
#define MQTT_USER "yourMqttUser"
#define MQTT_PASSWORD "yourMqttPassword"

// Define Update Credentials
#define WEB_UPDATE_USERNAME "yourUpdateUser"
#define WEB_UPDATE_PASSWORD "yourUpdatePassword"
```