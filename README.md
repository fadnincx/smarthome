# Smarthome

## About

In this repo is the code for the ESP8266 / Wemos D1 mini to run the smarthome based light / shutter system. All the communication is MQTT based and thus easy to integrate in other applications.

Each device provides a slim webapplication to debug/controll the basic functions, as well as OTA.

The software is desined that the ESP is always on and thus battery powered operation is not adviced.


## Compile / Deployment infos:


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

Configuration should be provided using the following schema. Each of the 4 arrays is optional and can contain as many children as needed (limit not tested). All the other values are required.
```
{
   "name":"Device name",
   "availableTopic":"deviceTopic/available",
   "switch":[
      {
         "pin":1,
         "statusTopic":"lightX",
         "setTopic":"lightX/set",
         "msgOff":"{\"state\":\"OFF\"}",
         "msgOn":"{\"state\":\"ON\", \"brightness\": 255}",
         "msgDark":"{\"state\":\"ON\", \"brightness\": 20}"
      }
   ],
   "shutter":[
      {
         "pinUp":1,
         "pinDown":2,
         "statusTopic":"shutterY",
         "setTopic":"shutterY/set",
         "msgUp":"{\"move\":\"UP\"}",
         "msgStop":"{\"move\":\"STOP\"}",
         "msgDown":"{\"move\":\"DOWN\"}"
      }
   ],
   "shutterRelais":[
      {
         "pinUp":1,
         "pinDown":2,
         "statusTopic":"shutterY",
         "setTopic":"shutterY/set",
         "upMsg":"up",
         "stopMsg":"stop",
         "downMsg":"down",
         "msgOpen":"{\"state\":\"OPEN\"}",
         "msgClose":"{\"state\":\"CLOSE\"}"
      }
   ],
   "lightRelais":[
      {
         "pin":3,
         "statusTopic":"lightX",
         "setTopic":"lightY/set",
         "onMsg":"on",
         "offMsg":"off",
         "msgOff":"{\"state\":\"OFF\"}",
         "msgOn":"{\"state\":\"ON\"}"
      }
   ]
}
```
