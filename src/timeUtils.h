#ifndef SMART_ESP_TIME_H
#define SMART_ESP_TIME_H

#include <time.h>

class Time{

    public:
        static Time* getInstance();
    private:
        static Time* instance;
        Time();
    public:
        struct tm* getTime();
        bool isDay();
};

#endif