#include "timeUtils.h"
#include <Arduino.h>
#include <time.h>

Time* Time::getInstance(){
    return (!instance) ? instance = new Time() : instance;
}
Time* Time::instance = 0;
struct tm* Time::getTime(){
    time_t now;
    struct tm * timeinfo;
    time(&now);
    timeinfo = localtime(&now);
    Serial.printf("%i.%i.%i - %i:%i:%i\n",timeinfo->tm_mday,(timeinfo->tm_mon+1),(timeinfo->tm_year+1900), timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);

    return timeinfo;
}
bool Time::isDay(){
    return true;
}
Time::Time(){
    // Set NTP Server
    configTime(0,0,"192.168.10.3");

    // Set Timezone
    setenv("TZ", "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", 0);

    // call data once...
    time_t now;
    time(&now);
    localtime(&now);

    Serial.println("Initialized Time");
}


