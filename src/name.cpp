#include "name.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
Name* Name::getInstance(){
    return (!instance) ? instance = new Name() : instance;
}
Name* Name::instance = 0;
//prints all rtcData, including the leading crc32
void Name::printMemory() {
    char buf[3];
    uint8_t *ptr = (uint8_t *)&rtcData;
    for (size_t i = 0; i < sizeof(rtcData); i++) {
        sprintf(buf, "%02X", ptr[i]);
        Serial.print(buf);
        if ((i + 1) % 32 == 0) {
        Serial.println();
        } else {
        Serial.print(" ");
        }
    }
    Serial.println();
}
uint32_t Name::calculateCRC32(const uint8_t *data, size_t length) {
    uint32_t crc = 0xffffffff;
    while (length--) {
        uint8_t c = *data++;
        for (uint32_t i = 0x80; i > 0; i >>= 1) {
        bool bit = crc & 0x80000000;
        if (c & i) {
            bit = !bit;
        }
        crc <<= 1;
        if (bit) {
            crc ^= 0x04c11db7;
        }
        }
    }
    return crc;
}

Name::Name() {  // private constructor
    if (ESP.rtcUserMemoryRead(0, (uint32_t*) &rtcData, sizeof(rtcData))) {
        hasRTC = true;
        Serial.println("Read: ");
        printMemory();
        Serial.println();
        uint32_t crcOfData = calculateCRC32((uint8_t*) &rtcData.data[0], sizeof(rtcData.data));
        Serial.print("CRC32 of data: ");
        Serial.println(crcOfData, HEX);
        Serial.print("CRC32 read from RTC: ");
        Serial.println(rtcData.crc32, HEX);
        if (crcOfData != rtcData.crc32) {
            Serial.println("CRC32 in RTC memory doesn't match CRC32 of data. Data is probably invalid!");
        } else {
            Serial.println("CRC32 check ok, data is probably valid.");
            name =  rtcData.data;
            return;
        }
    }
    name = WiFi.macAddress().c_str();
}
const char * Name::getName(){  
    return name;
}

void Name::setName(const char* newName){
    if(strcmp(newName, name) == 0)
        return;
    name = newName;
    if(hasRTC) {
        // Copy Name
        memcpy(rtcData.data, newName, sizeof(newName));
        // Update CRC32
        rtcData.crc32 = calculateCRC32((uint8_t*) &rtcData.data[0], sizeof(rtcData.data));

        // Write to RTC
        if (ESP.rtcUserMemoryWrite(0, (uint32_t*) &rtcData, sizeof(rtcData))) {
            Serial.println("Write: ");
            printMemory();
            Serial.println();
        }
    }
}