#include "TemperaturePreferences.h"

Preferences preferences;

TemperaturePreferences::TemperaturePreferences(const char* folder){
    this->folder = folder;
    this->hasConfigurationStatus = false;
}

/**
 * @brief Write the Wifi Configuration to the Preferences
 *
 * @param ssid the SSID of the Wifi Network
 * @param passwd the Password of the Wifi Network
 */
void TemperaturePreferences::writeWiFiConfiguration(String ssid, String passwd)
{
    preferences.begin(folder, false);
    if(ssid != "") preferences.putString(PREF_KEY_WIFI_SSID, ssid);
    if(passwd != "") preferences.putString(PREF_KEY_WIFI_PASSWORD, passwd);
    preferences.end();
}

/**
 * @brief Write the InfluxDB Configuration to the Preferences
 *
 * @param influxUrl the URL of the InfluxDB
 * @param influxToken the Token of the InfluxDB
 * @param influxOrganisation the Organisation of the InfluxDB
 * @param influxBucket the Bucket of the InfluxDB
 */
void TemperaturePreferences::writeInfluxDBConfiguration(String influxUrl, String influxToken, String influxOrganisation, String influxBucket)
{
    preferences.begin(folder, false);
    if(influxUrl != "") preferences.putString(PERF_KEY_INFLUX_URL, influxUrl);
    if(influxToken != "") preferences.putString(PERF_KEY_INFLUX_TOKEN, influxToken);
    if(influxOrganisation != "") preferences.putString(PERF_KEY_INFLUX_ORGANISATION, influxOrganisation);
    if(influxBucket != "") preferences.putString(PERF_KEY_INFLUX_BUCKET, influxBucket);
    preferences.end();
}

/**
 * @brief Set the Configuration Mode
 *
 * @param hasConfiguration the Configuration Mode
 */
void TemperaturePreferences::setConfiguration(bool hasConfiguration)
{
    preferences.begin(folder, false);
    preferences.putBool(PERF_KEY_HAS_CONFIGURATION, hasConfiguration);
    preferences.end();
    updateConfigurationStatus();
}

/**
 * @brief Set the Fail Code
 *
 * @param failCode the Fail Code
 */
void TemperaturePreferences::setErrorCode(int failCode)
{
    preferences.begin(folder, false);
    preferences.putInt(PERF_KEY_FAIL, failCode);
    preferences.end();
}

/**
 * @brief Get the Influx Parameter object
 *
 * @param url the URL of the InfluxDB
 * @param token the Token of the InfluxDB
 * @param organisation the Organisation of the InfluxDB
 * @param bucket the Bucket of the InfluxDB
 */
void TemperaturePreferences::getInfluxParameter(String *url, String *token, String *organisation, String *bucket)
{
    preferences.begin(folder, false);
    *url = preferences.getString(PERF_KEY_INFLUX_URL, "No URL");
    *token = preferences.getString(PERF_KEY_INFLUX_TOKEN, "No Token");
    *organisation = preferences.getString(PERF_KEY_INFLUX_ORGANISATION, "No Organisation");
    *bucket = preferences.getString(PERF_KEY_INFLUX_BUCKET, "No Bucket");
    preferences.end();
}

/**
 * @brief Get the Wi Fi Parameter object
 *
 * @param ssid the SSID of the Wifi Network
 * @param passwd the Password of the Wifi Network
 */
void TemperaturePreferences::getWiFiParameter(String *ssid, String *passwd)
{
    preferences.begin(folder, false);
    *ssid = preferences.getString(PREF_KEY_WIFI_SSID, "No SSID");
    *passwd = preferences.getString(PREF_KEY_WIFI_PASSWORD, "No Password");
    preferences.end();
}

/**
 * @brief Get the Last Error Code
 *
 * @return the Last Error Code
 */
int TemperaturePreferences::getLastErrorCode(){
    preferences.begin(folder, false);
    int errorCode = preferences.getInt(PERF_KEY_FAIL, -1);
    preferences.end();
    return errorCode;
}

/**
 * @brief Check if the Configuration is set
 *
 * @return true if the Configuration is set
 */
bool TemperaturePreferences::hasConfiguration(){
    return hasConfigurationStatus;
}

/**
 * @brief Update the Configuration Status
 */
void TemperaturePreferences::clear(){
    preferences.begin(folder, false);
    preferences.clear();
    preferences.end();
}

/**
 * @brief Update the Configuration Status
 */
void TemperaturePreferences::updateConfigurationStatus(){
    preferences.begin(folder, false);
    hasConfigurationStatus = preferences.getBool(PERF_KEY_HAS_CONFIGURATION, false);
    preferences.end();
}

/**
 * @brief Write the Node Name
 * 
 * @param nodeName the name
 */
void TemperaturePreferences::writeNodeName(String nodeName){
    preferences.begin(folder, false);
    preferences.putString(PERF_KEY_NODE_NAME, nodeName);
    preferences.end();
}

/**
 * @brief get the Node Name
 * 
 * @return String the Name
 */
void TemperaturePreferences::getNodeName(String *name){
    preferences.begin(folder, false);
    *name = preferences.getString(PERF_KEY_NODE_NAME, "No Node Name");
    preferences.end();
}