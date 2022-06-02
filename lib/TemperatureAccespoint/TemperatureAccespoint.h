/**
 * @brief Temperature Accespoint
 * @details This Programm is used to create a Accespoint for the Temperature Sensor
 * @author Christoph Schwarz
 * @version 1.0
 * @date 2022-06-02
 */

#ifndef TemperatureAccespoint_h
#define TemperatureAccespoint_h

#include <WiFi.h>
#include <WebServer.h>
#include <ESPAsyncWebServer.h>
#include "TemperaturePreferences.h"

#define WEB_SERVER_PORT 80

class TemperatureAccespoint
{
public:
    String inputSsid;
    String inputPasswd;
    String inputInfluxUrl;
    String inputInfluxToken;
    String inputInfluxOrgranisation;
    String inputInfluxBucket;

    TemperatureAccespoint(String ssid);

    void printConnectionInfo();
    void start(String webseiteStringHTML, String *wifiScanSSIDs, TemperaturePreferences *settings);
    void handle();

private:
    String ssid;
};

#endif