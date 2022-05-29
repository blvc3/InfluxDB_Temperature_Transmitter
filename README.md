# influxDB_temperature_transmitter

A PlatformIO Programm that transmitts temperature data to an influxDB on a EPS32

## Configuration:

Step 1: Create an InfluxDB Account or set it up on Docker or something else <br>
Step 2: Create a Bucket and an API Token <br>
Step 3: Start the ESP32 and Connect to it´s WiFi <br>
Step 4: got to 192.168.4.1 and enter your Credentials <br>
Step 5: Set it up and measure <br>

## Pinlayout

### Temperaturesensor
| Temp Sensor | ESP32 | 
| --- | --- |
|VCC | VCC|
|GND | GND|
|Data | Pin 15|

**Still in Progress**
