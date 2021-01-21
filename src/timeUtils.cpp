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

    // Get current time
    struct tm* currentTime = getTime();

    // initialize Dusk2Dawn library for location at home (+2h if summer, +1h if winter)
    Dusk2Dawn home(46.8570158,8.6637819, currentTime->tm_isdst?2:1);

    // Calculate runrise and sunset in minutes after midnight for today
    int sunrise = home.sunrise(currentTime->tm_year+1900, currentTime->tm_mon+1, currentTime->tm_mday, currentTime->tm_isdst);
    int sunset = home.sunrise(currentTime->tm_year+1900, currentTime->tm_mon+1, currentTime->tm_mday, currentTime->tm_isdst);

    // Get minutes since midnight
    int minutestimeSinceMidnight = ((currentTime->tm_hour)*60) + (currentTime->tm_min);

    // return if between sunrise and sunset
    return sunrise < minutestimeSinceMidnight && sunset > minutestimeSinceMidnight;
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
char* Time::uptime(){
    unsigned long milli = millis();
    static char _return[32];
    unsigned long secs=milli/1000, mins=secs/60;
    unsigned int hours=mins/60, days=hours/24;
    milli-=secs*1000;
    secs-=mins*60;
    mins-=hours*60;
    hours-=days*24;
    sprintf(_return,"Uptime %d days %2.2d:%2.2d:%2.2d.%3.3d (counter reset after ~49d17h)", (byte)days, (byte)hours, (byte)mins, (byte)secs, (int)milli);
    return _return;
}

