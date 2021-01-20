#ifndef SMART_ESP_WIFI_CONNECTION
#define SMART_ESP_WIFI_CONNECTION


/**
 * Class to Connect and keep connected to Wifi and MQTT
 **/
class WifiConnection
{       
    public:
        static void connectToWifi();
        static void connectIfNotConnected();

};

#endif