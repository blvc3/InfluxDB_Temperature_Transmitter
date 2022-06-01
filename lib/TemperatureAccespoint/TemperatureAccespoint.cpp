#include "TemperatureAccespoint.h"

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
    Serial.println("Connected to " + WiFi.SSID());
    Serial.println("Go To https://" + WiFi.localIP().toString() + "/ to configure the device");
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
    Serial.println("Connecting to WiFi");
    WiFi.begin(ssid.c_str());
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [webseiteStringHTML]()
              { server.send(200, "text/html", webseiteStringHTML); });
    server.on("/input", HTTP_GET, [wifiScanSSIDs, settings]()
              {
            String ssidNumber = server.arg("ssid");
            int ssidInt = ssidNumber.toInt();
            String ssid = wifiScanSSIDs[ssidInt];
            String passwd = server.arg("passwd");
            String influxUrl = server.arg("influxUrl");
            String influxToken = server.arg("influxToken");
            String influxOrganisation = server.arg("influxOrganisation");
            String influxBucket = server.arg("influxBucket");

            if(ssid != "") Serial.println("SSID: " + ssid);
            if(passwd != "") Serial.println("Password: " + passwd);
            if(influxUrl != "") Serial.println("InfluxDB URL: " + influxUrl);
            if(influxToken != "") Serial.println("InfluxDB Token: " + influxToken);
            if(influxOrganisation != "") Serial.println("InfluxDB Organisation: " + influxOrganisation);
            if(influxBucket != "") Serial.println("InfluxDB Bucket: " + influxBucket);

            settings->writeWiFiConfiguration(ssid, passwd);
            settings->writeInfluxDBConfiguration(influxUrl, influxToken, influxOrganisation, influxBucket);
            settings->setConfiguration(true);

            server.send(200, "text/html", "Configuration done");
            Serial.println("Setup done Restarting now");
            delay(1000);
            ESP.restart(); });
    server.begin();
}

/**
 * @brief Check if the WLAN is connected
 */
void TemperatureAccespoint::handle(){
    server.handleClient();
}