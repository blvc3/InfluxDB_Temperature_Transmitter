/**
 * @brief InfluxDB Transmitter
 * @details This Programm is used to transmit temperatur data to an InfluxDB
 * @author Christoph Schwarz
 * @version 0.1
 * @date 2022-05-27
 */

//------ INCLUDE ------
#include "Header.h"
#include "Identifier.h"

//------ VARIABLES ------
// Wifi Web Server
IPAddress ip(192, 168, 178, 110);
IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(WEB_SERVER_PORT);
const char *ssidLocal = WIFI_LOCAL_SSID;

// Temperature Sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

// Configuration
bool hasConfiguration = false;

// Wifi Connect
const char *ssidHomeNetwork;
const char *passwdHomeNetwork;
String *wifiScanSSIDs;
WiFiMulti wifiMulti;

// InfluxDB
const char *influxdbUrl;
const char *influxdbToken;
const char *influxdbOrganisation;
const char *influxdbBucket;

//InfluxDB Client
InfluxDBClient client(influxdbUrl, influxdbOrganisation, influxdbBucket, influxdbToken, InfluxDbCloud2CACert);

//Preferences
TemperaturePreferences settings("pref");

// Datapoints
Point sensor(WIFI_LOCAL_SSID);

// ------ FUNCTIONS ------
/**
 * @brief Scan for Wifi Networks
 */
void scanWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.println("Scan start");
    int n = WiFi.scanNetworks();
    Serial.println("Scan done");
    wifiScanSSIDs = new String[n];
    if (n == 0)
    {
        Serial.println("No networks found");
    }
    else
    {
        Serial.print("Found " + String(n) + " networks");
        for (int i = 0; i < n; ++i)
        {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            wifiScanSSIDs[i] = WiFi.SSID(i);
            delay(10);
        }
    }
    Serial.println("Finished Scan");
}

/**
 * @brief get the Website String
 *
 * @return String the HTML Website
 */
String getHTMLString(bool addWifi, bool addInflux)
{
    String html = "<!DOCTYPE HTML><html><head><title>Temperature Sensor</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><body>";
    if (addWifi)
    {
        scanWiFi();
        Serial.println(sizeof(wifiScanSSIDs));
        for (int i = 0; i < sizeof(wifiScanSSIDs); i++)
        {
            html += String(i) + ") " + wifiScanSSIDs[i] + "<br>";
        }
        html += "<form action=\"/input\">SSID <input type=\"number\" name=\"ssid\" min = 0 max = ";
        html += String(sizeof(wifiScanSSIDs));
        html += "><br>Password <input type=\"text\" name=\"passwd\"><br>";
    }
    if (addInflux)
    {
        html += "InfluxDB URL <input type=\"text\" name=\"influxUrl\"><br>";
        html += "InfluxDB Token <input type=\"text\" name=\"influxToken\"><br>";
        html += "InfluxDB Organisation <input type=\"text\" name=\"influxOrganisation\"><br>";
        html += "InfluxDB Bucket <input type=\"text\" name=\"influxBucket\"><br>";
    }
    html += "<input type=\"submit\" value=\"Submit\"></form><form actiom=\"/refresh\"><input type=\"submit\" value=\"Refresh\"></form></body></html>";
    return html;
}

/**
 * @brief Get the Temperature object
 *
 * @param temp the integer in witch the temperature should be stored
 * @param cycels how many cycles should be done to measure the temperature
 */
float getTemperature(int cycels)
{
    float result = 1;
    for (int i = 0; i < cycels; i++)
    {
        // Measuring
        tempSensor.requestTemperatures();
        result += tempSensor.getTempCByIndex(0);

        Serial.println("Current Temp: " + String(result));

        if ((int)result == -127)
        {
            result = 0;
            i = 0;
            Serial.println("No Sensor Connected");
        }
        delay(1000);
    }
    result = result / cycels;
    return result;
}

/**
 * @brief send the Temperature to the InfluxDB
 *
 * @param temp the Temperature
 */
void sendTemp(int *temp)
{
    sensor.clearFields();
    sensor.addField("rssi", WiFi.RSSI());
    sensor.addField("temperature", temp);
    Serial.print("Writing: ");

    Serial.println(sensor.toLineProtocol());
    if (wifiMulti.run() != WL_CONNECTED)
    {
        Serial.println("Wifi connection lost");
    }
    if (!client.writePoint(sensor))
    {
        Serial.print("InfluxDB write failed: ");
        Serial.println(client.getLastErrorMessage());
    }
}

bool hasWifi()
{
    int checkCounter = 0;
    bool result = false;
    while (WiFi.status() != WL_CONNECTED && checkCounter < 10)
    {
        delay(500);
        checkCounter++;
    }

    if (checkCounter > 9)
    {
        result = false;
    }
    else
    {
        result = true;
    }
    return result;
}

void configureTemperatureSensor()
{
    Serial.println("No configuration found");
    Serial.println("Starting configuration mode");

    // Scan Wifi
    String webseiteStringHTML;
    int failLastTime = settings.getLastErrorCode();

    switch (failLastTime)
    {
    case FAIL_MESSAGE_WIFI_CONNECT:
        Serial.println("Failed last Time: WIFI");
        webseiteStringHTML = getHTMLString(true, false);
        break;
    case INFLUX_PARAMETER_ERROR:
        Serial.println("Failed last Time: INFLUX");
        webseiteStringHTML = getHTMLString(false, true);
        break;
    default:
        Serial.println("Failed last Time: No Error given");
        webseiteStringHTML = getHTMLString(true, true);
        break;
    }

    Serial.println("Please connect to " + String(WIFI_LOCAL_SSID));
    WiFi.mode(WIFI_MODE_AP);
    WiFi.config(ip, gateway, subnet);
    WiFi.softAP(ssidLocal);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // Web Server
    //  Send web page with input fields to client
    server.on("/", HTTP_GET, [webseiteStringHTML]()
              { server.send(200, "text/html", webseiteStringHTML); });
    server.on("/input", HTTP_GET, []()
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

            settings.writeWiFiConfiguration(ssid, passwd);
            settings.writeInfluxDBConfiguration(influxUrl, influxToken, influxOrganisation, influxBucket);

            settings.preferencesSetConfiguration(true);

            server.send(200, "text/html", "Configuration done");
            ESP.restart(); });
    server.begin();
}

void startTemperatureSensor()
{
    Serial.println("Configuration found");

    // Get Configuration from Preferences
    String prefSSID;
    String prefPasswd;
    String perfInfluxUrl;
    String perfInfluxPort;
    String perfInfluxToken;
    String perfInfluxOrganisation;
    String perfInfluxBucket;

    settings.getWiFiParameter(&prefSSID, &prefPasswd);
    settings.getInfluxParameter(&perfInfluxUrl, &perfInfluxToken, &perfInfluxOrganisation, &perfInfluxBucket);

    Serial.println("SSID: " + prefSSID);
    Serial.println("Password: " + prefPasswd);
    Serial.println("InfluxDB URL: " + perfInfluxUrl);
    Serial.println("InfluxDB Port: " + perfInfluxPort);
    Serial.println("InfluxDB Token: " + perfInfluxToken);
    Serial.println("InfluxDB Organisation: " + perfInfluxOrganisation);
    Serial.println("InfluxDB Bucket: " + perfInfluxBucket);

    if (prefSSID == "No SSID" || prefPasswd == "No Password")
    {
        Serial.println("No SSID found");
        settings.preferencesSetFail(FAIL_MESSAGE_WIFI_CONNECT);
        settings.preferencesSetConfiguration(false);
        ESP.restart();
    }
    else
    {
        ssidHomeNetwork = prefSSID.c_str();
        passwdHomeNetwork = prefPasswd.c_str();
    }

    if (perfInfluxUrl == "No URL" || perfInfluxPort == "No Port" || perfInfluxToken == "No Token" || perfInfluxOrganisation == "No Organisation" || perfInfluxBucket == "No Bucket")
    {
        Serial.println("No InfluxDB found");
        settings.preferencesSetFail(INFLUX_PARAMETER_ERROR);
        settings.preferencesSetConfiguration(false);
        ESP.restart();
    }
    else
    {
        influxdbUrl = perfInfluxUrl.c_str();
        influxdbToken = perfInfluxToken.c_str();
        influxdbOrganisation = perfInfluxOrganisation.c_str();
        influxdbBucket = perfInfluxBucket.c_str();
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssidHomeNetwork, passwdHomeNetwork);
    Serial.println("\nConnecting");
    if(!hasWifi()){
        Serial.println("Wifi connection lost");
        settings.preferencesSetFail(FAIL_MESSAGE_WIFI_CONNECT);
        settings.preferencesSetConfiguration(false);
        ESP.restart();
    }
    else
    {
        Serial.println("Wifi connected");
    }

    // InfluxDB Configuration Sernsor
    sensor.addTag("device", DEVICE);
    sensor.addTag("SSID", WiFi.SSID());
    sensor.addTag("temperature", "0");

    timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

    if (client.validateConnection())
    {
        Serial.print("Connected to InfluxDB: ");
        Serial.println(client.getServerUrl());
    }
    else
    {
        Serial.print("InfluxDB connection failed: ");
        String cause = client.getLastErrorMessage();
        Serial.println(cause);

        if (cause.equals("Invalid parameters"))
        {
            settings.preferencesSetFail(INFLUX_PARAMETER_ERROR);
            settings.preferencesSetConfiguration(false);
            ESP.restart();
        }
        else
            Serial.println("No known error");
    }
}

// ------ MAIN ------
/**
 * @brief Set up the ESP32
 */
void setup()
{
    // Serial
    Serial.begin(115200);
    delay(1);
    Serial.println("Starting");

    bool hasConfiguration = settings.hasConfiguration();

    // LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // Wifi Web Server no configuration
    if (!hasConfiguration)
    {
        configureTemperatureSensor();
    }
    else
    {
        startTemperatureSensor();
    }
}

/**
 * @brief Main Loop
 */
void loop()
{
    if (!hasConfiguration)
        server.handleClient();
    else
    {
        int temp = getTemperature(10);
        sendTemp(&temp);
    }
}