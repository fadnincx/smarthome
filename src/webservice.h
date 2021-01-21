#ifndef SMART_ESP_WEBSERVICE_H
#define SMART_ESP_WEBSERVICE_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <uri/UriBraces.h>
#include "config.h"
#include "name.h"
#include "secrets.h"
#include "version.h"
#include "timeUtils.h"

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

void processWeb(){
    // Handle HttpServer / Update
    httpServer.handleClient();
}


void initializeWebservice(){

    // Set info page on root
    httpServer.on("/", [](){

        // Titel
        String html = "";
        html += "<div style='position: relative; min-height: 100vh;'><div style='padding-bottom: 2.5em'>";
        html += "<h1>";
        html += Name::getInstance()->getName();
        html += " @ ";
        html += WiFi.localIP().toString();
        html += "</h1>";

        // Tiles with informations
        html += "<div class='tiles' style='display:flex; flow-direction: row; flex-wrap: wrap;'>";
        html += Config::getInstance()->getFixedHtmlTiles();
        html += "</div>";

        // Reboot button
        html += "<button onClick='reboot()'>Reboot ESP</button>";
        html += "<script>function reboot(){const http = new XMLHttpRequest(); http.open('GET','/reboot'); http.send();}</script>";

        // Button with link to update
        html += "<button onClick='window.location.href=\"/update\";'>Upload Update</button>";

        // Button with link to loaded config
        html += "<button onClick='window.location.href=\"/loadedConfig\";'>Loaded Config</button>";

        html += "<br>";
        html += "</div>";
        // Footer
        html += "<footer style='position: absolute; bottom: 0; width: 100%; height: 2.5em;'>Version ";
        html += VERSION;
        html += " from ";
        html += __FILE__;
        html += ", compiled ";
        html += __DATE__;
        html += " ";
        html += __TIME__;
        html += Time::getInstance()->isDay()?"[is Day]":"[is Night]";
        html += " (";
        html += Time::getInstance()->uptime();
        html += ")";
        html += ", compiled with ";
        html += __VERSION__ ;
        html += " </footer>";
        html += "</div>";

        httpServer.send(200, "text/html", html);
    });

    // Set Reboot Path
    httpServer.on("/reboot", [](){
            
        // Redirect to root page
        httpServer.sendHeader("Location", "/",true);
        httpServer.send(302, "text/plane","");

        // Print Debug info
        Serial.println("Web Reboot");

        // Wait a second
        delay(1000);

        // Reboot
        ESP.restart();
    });

    // Initialize Web OTA Update
    httpUpdater.setup(&httpServer, "/update", WEB_UPDATE_USERNAME, WEB_UPDATE_PASSWORD);

    // Return Version
    httpServer.on("/version", [](){
        httpServer.send(200, "text/plain", VERSION);
    });

    // Return Name
    httpServer.on("/name", [](){
        httpServer.send(200, "text/plain", Name::getInstance()->getName());
    });

    // Return Loaded Config
    httpServer.on("/loadedConfig", [](){
        httpServer.send(200, "application/json", Config::getInstance()->getStringLoadedConfig());
    });

    // Ajax return status of object
    httpServer.on(UriBraces("/status/{}"), [](){
        int nr = atoi(httpServer.pathArg(0).c_str());
        String status = Config::getInstance()->getNthStatus(nr);
        httpServer.send(200, "application/json", status);
    });

    // Ajax apply toogle to object
    httpServer.on(UriBraces("/toggle/{}"), [](){
        int nr = atoi(httpServer.pathArg(0).c_str());
        String status = Config::getInstance()->toggleNth(nr);
        httpServer.send(200, "application/json", status);
    });

    // If url not defined, return error page    
    httpServer.onNotFound([](){
        String msg = "<h1>ERROR 404 - Page not Found</h1><p>by ";
        msg += Name::getInstance()->getName();
        msg += " @ ";
        msg += WiFi.localIP().toString();

        httpServer.send(404, "text/html", msg);
    });

  // Start Webserver
  httpServer.begin();

  Serial.println("Initialized Webserver");
  
}

#endif