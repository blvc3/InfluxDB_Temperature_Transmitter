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

//Wifi List
String *wifiScanSSIDs;

// Temperature Sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

// InfluxDB
String influxdbUrl;
String influxdbToken;
String influxdbOrganisation;
String influxdbBucket;

// Preferences
TemperaturePreferences settings("pref");
TemperatureWifiHelper wifi;
TemperatureAccespoint accespoint(NODE_NAME);


// Datapoints
Point sensor(NODE_NAME);

// ------ FUNCTIONS ------
/**
 * @brief get the Website String
 *
 * @return String the HTML Website
 */
String getHTMLString(bool addWifi, bool addInflux)
{
    String html = "<!DOCTYPE HTML><html><head><title>Temperature Sensor</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><body><form action=\"/input\">";
    if (addWifi)
    {
        wifi.discoverWifi();
        wifiScanSSIDs = wifi.getWifiNetworksList();
        Serial.println(sizeof(wifiScanSSIDs));
        for (int i = 0; i < sizeof(wifiScanSSIDs); i++)
        {
            html += String(i) + ") " + wifiScanSSIDs[i] + "<br>";
        }
        html += "SSID <input type=\"number\" name=\"ssid\" min = 0 max = ";
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
double getTemperature(int cycels)
{
    double result = 1;
    for (int i = 0; i < cycels; i++)
    {
        // Measuring
        tempSensor.requestTemperatures();
        result += tempSensor.getTempCByIndex(0);

        if (tempSensor.getTempCByIndex(0) == -127)
        {
            result = 0;
            i = 0;
            Serial.println("No Sensor Connected");
        }
        delay(1000);
    }
    result = result / cycels;
    Serial.println("Measured Temperature: " + String(result) + "°C" + " In " + String(cycels) + " Cycles");
    return result;
}

/**
 * @brief send the Temperature to the InfluxDB
 *
 * @param temp the Temperature
 */
void sendTemp(double *temp)
{
    // InfluxDB Client
    InfluxDBClient client(influxdbUrl, influxdbOrganisation, influxdbBucket, influxdbToken, InfluxDbCloud2CACert);
    sensor.clearFields();
    sensor.addField("rssi", WiFi.RSSI());
    sensor.addField("temperature", temp);
    Serial.print("Writing: ");

    Serial.println(sensor.toLineProtocol());
    if(!wifi.hasWifi()){
        wifi.connect();
        if(!wifi.hasWifi()) Serial.println("No Wifi Connection");
    }
    if (!client.writePoint(sensor))
    {
        Serial.print("InfluxDB write failed: ");
        String errormessage = client.getLastErrorMessage();
        Serial.println(errormessage);
    }
}

void configureTemperatureSensor(int errorcode)
{
    Serial.println("No configuration found");

    // Scan Wifi
    String webseiteStringHTML;

    switch (errorcode)
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

    accespoint.start(webseiteStringHTML, wifi.getWifiNetworksList(), &settings);
    accespoint.printConnectionInfo();

}

void startTemperatureSensor()
{
    Serial.println("Configuration found:");

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
        settings.setErrorCode(FAIL_MESSAGE_WIFI_CONNECT);
        settings.setConfiguration(false);
        ESP.restart();
    }
    else
    {
        wifi.setSSID(prefSSID.c_str());
        wifi.setPassword(prefPasswd.c_str());
    }

    if (perfInfluxUrl == "No URL" || perfInfluxToken == "No Token" || perfInfluxOrganisation == "No Organisation" || perfInfluxBucket == "No Bucket")
    {
        Serial.println("Not all Parameter given");
        settings.setErrorCode(INFLUX_PARAMETER_ERROR);
        settings.setConfiguration(false);
        ESP.restart();
    }
    else
    {
        influxdbUrl = perfInfluxUrl;
        influxdbToken = perfInfluxToken;
        influxdbOrganisation = perfInfluxOrganisation;
        influxdbBucket = perfInfluxBucket;
    }

    
    wifi.connect();

    if (!wifi.hasWifi())
    {
        Serial.println("Wifi connection lost");
        settings.setErrorCode(FAIL_MESSAGE_WIFI_CONNECT);
        settings.setConfiguration(false);
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

    // InfluxDB Client
    InfluxDBClient client(influxdbUrl, influxdbOrganisation, influxdbBucket, influxdbToken, InfluxDbCloud2CACert);

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
            settings.setErrorCode(INFLUX_PARAMETER_ERROR);
            settings.setConfiguration(false);
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
    Serial.println("Starting...");

    // Pins
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    pinMode(RESET_BUTTON_PIN, INPUT);

    if (digitalRead(RESET_BUTTON_PIN) == HIGH)
    {
        Serial.println("Reset button pressed");
        settings.setConfiguration(false);
        delay(5000);
        ESP.restart();
    }

    // Wifi Web Server no configuration
    int failLastTimeErrorCode = settings.getLastErrorCode();
    settings.setErrorCode(-1);

    settings.updateConfigurationStatus();

    if (settings.hasConfiguration()) startTemperatureSensor();
    else configureTemperatureSensor(failLastTimeErrorCode);
}

/**
 * @brief Main Loop
 */
void loop()
{
    if (settings.hasConfiguration())
    {
        double temp = getTemperature(10);
        sendTemp(&temp);
    } else accespoint.handle();
}