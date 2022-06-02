/**
 * @brief Temperature Wifi Helper
 * @details This Programm is used to connect to a Wifi 
 * @author Christoph Schwarz
 * @version 1.0
 * @date 2022-06-02
 */

#ifndef TemperatureWifiHelper_h
#define TemperatureWifiHelper_h

#include <Wifi.h>

#define printoutWifi(x) Serial.print("[WIFI]" + String(x));

class TemperatureWifiHelper
{
    public:
        ~TemperatureWifiHelper();
        void setSSID(String ssid);
        void setPassword(String password);
        bool connect();
        bool hasWifi();
        String *getWifiNetworksList();
        void discoverWifi();
    private:
        String *wifiNetworksList;
        String ssid;
        String password;
};
#endif