/**
 * @brief Temperature Preference
 * @details This Programm is used to make Preferences for the Temperature Sensor
 * @author Christoph Schwarz
 * @version 1.0
 * @date 2022-06-02
 */

#ifndef TemperaturePreferences_h
#define TemperaturePreferences_h

#include <Preferences.h>

#define PERF_KEY_HAS_CONFIGURATION "hconf"
#define PREF_KEY_WIFI_SSID "ssid"
#define PREF_KEY_WIFI_PASSWORD "passwd"
#define PREF_KEY_INFLUX_PORT "port"
#define PERF_KEY_INFLUX_TOKEN "token"
#define PERF_KEY_INFLUX_URL "url"
#define PERF_KEY_INFLUX_ORGANISATION "org"
#define PERF_KEY_INFLUX_BUCKET "buck"
#define PERF_KEY_FAIL "fail"
#define PERF_KEY_NODE_NAME "name"

class TemperaturePreferences
{
public:
    TemperaturePreferences(const char* folder);
    void writeWiFiConfiguration(String ssid, String passwd);
    void writeInfluxDBConfiguration(String influxUrl, String influxToken, String influxOrganisation, String influxBucket);
    void writeNodeName(String nodeName);
    void setConfiguration(bool hasConfiguration);
    void getInfluxParameter(String *url, String *token, String *organisation, String *bucket);
    void getWiFiParameter(String *ssid, String *passwd);
    int getLastErrorCode();
    void getNodeName(String *name);
    void setErrorCode(int errorcode);
    bool hasConfiguration();
    void clear();
    void updateConfigurationStatus();
private:
    const char* folder;
    bool hasConfigurationStatus;
    String nodeName;
};

#endif