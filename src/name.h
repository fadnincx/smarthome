#ifndef SMART_ESP_NAME_H
#define SMART_ESP_NAME_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

class Name
{
    public:
        static Name* getInstance();
    private:
        static Name * instance;
        bool hasRTC = false;
        const char* name;

        struct {
            uint32_t crc32;
            char data[508];
        } rtcData;

        void printMemory();
        uint32_t calculateCRC32(const uint8_t *data, size_t length);
        Name();

    public:
        const char * getName();
        void setName(const char* newName);
    
};





#endif