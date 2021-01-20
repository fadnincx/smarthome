#ifndef SMART_ESP_WEBSERVICE_H
#define SMART_ESP_WEBSERVICE_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

#include "config.h"
#include "name.h"
#include "secrets.h"
#include "version.h"

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;


void webInfo(){
  String html = "<h1>";
  html += Name::getInstance()->getName();
  html += " @ ";
  html += WiFi.localIP().toString();
  html += "</h1>";

  html += "<h3>Current Status:</h3>";
  html += Config::getInstance()->getHtmlInfo();
  
  html += "<footer>Version ";
  html += VERSION;
  html += " from ";
  html += __FILE__;
  html += ", compiled ";
  html += __DATE__;
  html += " ";
  html += __TIME__;
  html += ", compiled with ";
  html += __VERSION__ ;
  html += " </footer>";
  
  httpServer.send(200, "text/html", html);
}


void webVersion(){
    httpServer.send(200, "text/plain", VERSION);
}

void webName(){
    httpServer.send(200, "text/plain", Name::getInstance()->getName());
}
void webReboot() {
    
    // Redirect to root page
    httpServer.sendHeader("Location", "/",true);
    httpServer.send(302, "text/plane","");

    // Print Debug info
    Serial.println("Web Reboot");

    // Wait a second
    delay(1000);

    // Reboot
    ESP.restart();
    
}

void webHandleNotFound(){

    String msg = "<h1>ERROR 404 - Page not Found</h1><p>by ";
    msg += Name::getInstance()->getName();
    msg += " @ ";
    msg += WiFi.localIP().toString();

    httpServer.send(404, "text/html", msg);
}
void processWeb(){
    // Handle HttpServer / Update
    httpServer.handleClient();
}


void initializeWebservice(){

  // Set info page on root
  httpServer.on("/", webInfo);

  // Set Reboot Path
  httpServer.on("/reboot", webReboot);

  // Initialize Web OTA Update
  httpUpdater.setup(&httpServer, "/update", WEB_UPDATE_USERNAME, WEB_UPDATE_PASSWORD);

  // Return Version
  httpServer.on("/version", webVersion);

  // Return Name
  httpServer.on("/name", webName);

  // Initialize Web OTA Update

  httpServer.onNotFound(webHandleNotFound);

  // Start Webserver
  httpServer.begin();

  Serial.println("Initialized Webserver");
  
}

#endif