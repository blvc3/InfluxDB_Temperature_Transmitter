#include "TemperatureAccespoint.h"

WebServer server(WEB_SERVER_PORT);

IPAddress ip(192, 168, 178, 110);
IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);

/**
 * @brief Construct a new Temperature Accespoint
 *
 * @param ssid the name of the WiFi (wifi)
 */
TemperatureAccespoint::TemperatureAccespoint(String ssid)
{
    this->ssid = ssid;
}

/**
 * @brief Prints the connection details
 */
void TemperatureAccespoint::printConnectionInfo()
{
    printoutWifiAP("Connected to " + WiFi.SSID() + "\n");
    printoutWifiAP("Go To https://" + WiFi.localIP().toString() + "/ to configure the device\n");
}

/**
 * @brief Create a new WLAN with a web server
 *
 * @param webseiteStringHTML the HTML code of the web page
 * @param wifiScanSSIDs the list of available WLANs
 * @param settings the settings from TemperaturePreferences
 */
void TemperatureAccespoint::start(String webseiteStringHTML, String *wifiScanSSIDs, TemperaturePreferences *settings)
{
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid.c_str());
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [webseiteStringHTML]()
              { server.send(200, "text/html", webseiteStringHTML); });
    server.on("/input", HTTP_GET, [wifiScanSSIDs, settings]()
              {
            String name = server.arg("name");
            String ssidNumber = server.arg("ssid");
            int ssidInt = ssidNumber.toInt();
            String ssid = wifiScanSSIDs[ssidInt];
            String passwd = server.arg("passwd");
            String influxUrl = server.arg("influxUrl");
            String influxToken = server.arg("influxToken");
            String influxOrganisation = server.arg("influxOrganisation");
            String influxBucket = server.arg("influxBucket");

            if(name != "") Serial.println("Name: " + name);
            if(ssid != "") Serial.println("SSID: " + ssid);
            if(passwd != "") Serial.println("Password: " + passwd);
            if(influxUrl != "") Serial.println("InfluxDB URL: " + influxUrl);
            if(influxToken != "") Serial.println("InfluxDB Token: " + influxToken);
            if(influxOrganisation != "") Serial.println("InfluxDB Organisation: " + influxOrganisation);
            if(influxBucket != "") Serial.println("InfluxDB Bucket: " + influxBucket);

            settings->writeNodeName(name);
            settings->writeWiFiConfiguration(ssid, passwd);
            settings->writeInfluxDBConfiguration(influxUrl, influxToken, influxOrganisation, influxBucket);
            settings->setConfiguration(true);

            server.send(200, "text/html", "Configuration done");
            printoutWifiAP("Setup done Restarting now\n");
            delay(1000);
            ESP.restart(); });
    server.begin();
}

/**
 * @brief Check if the WLAN is connected
 */
void TemperatureAccespoint::handle()
{
    server.handleClient();
}