#include "TemperatureWifiHelper.h"

/**
 * @brief Destroy the Temperature Wifi Helper:: Temperature Wifi Helper object
 */
TemperatureWifiHelper::~TemperatureWifiHelper(){
    if(wifiNetworksList != nullptr){
        delete wifiNetworksList;
    }
}

/**
 * @brief Set an ssid to connect to
 * 
 * @param ssid the ssid
 */
void TemperatureWifiHelper::setSSID(String ssid)
{
    this->ssid = ssid;
}

/**
 * @brief Set a password to the WiFi
 * 
 * @param password the password
 */
void TemperatureWifiHelper::setPassword(String password)
{
    this->password = password;
}

/**
 * @brief connect to the wifi
 * 
 * @return true if the connections was successful
 * @return false if the connection was not successful
 */
bool TemperatureWifiHelper::connect()
{
    if (this->ssid != "")
    {
        this->password == ""? WiFi.begin(this->ssid.c_str()) : WiFi.begin(this->ssid.c_str(), this->password.c_str());

        printoutWifi("Connecting...\n");

        while (!WiFi.isConnected()) delay(500);

        printoutWifi("WiFi connected\n");
        printoutWifi("IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    } else return false;
}

/**
 * @brief If the client is connected to the wifi
 * 
 * @return true if the client is connected to the wifi
 * @return false if the client is not connected to the wifi
 */
bool TemperatureWifiHelper::hasWifi()
{
    return WiFi.isConnected();
}

/**
 * @brief Returns a List of all Wifi Networks in the area
 * 
 * @return String* a list of the available networks
 */
String *TemperatureWifiHelper::getWifiNetworksList()
{
    return this->wifiNetworksList;
}

/**
 * @brief Scan for Wifi Networks
 */
void TemperatureWifiHelper::discoverWifi()
{
    WiFi.disconnect();
    printoutWifi("Scanning for Wifi Networks...\n");
    int n = WiFi.scanNetworks();
    if (n == 0)
    {
        printoutWifi("No networks found\n");
    }
    else
    {
        this->wifiNetworksList = new String[n];
        printoutWifi("Found " + String(n) + " networks:\n");
        for (int i = 0; i < n; ++i)
        {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i) + ", ");
            this->wifiNetworksList[i] = WiFi.SSID(i);
            delay(10);
        }
    }
}