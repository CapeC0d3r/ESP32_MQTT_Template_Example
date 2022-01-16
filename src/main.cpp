#include <Arduino.h>
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif

//https://github.com/knolleary/pubsubclient
#include "../lib/pubsubclient-master/src/PubSubClient.h"

//UPDATE THESE! 
const char *ssid =                       "YOUR WIFI NETWORK NAME";
const char *password =                   "YOUR WIFI NETWORK PASSWORD";
const char *mqtt_server_ip =             "YOUR.MQTT.SERVER.IP"; //Don't include the port here.
uint16_t   mqtt_port =                   1883;
const char *mqtt_user_name =             "THE MQTT USER YOU CREATED IN HOME ASSISTANT";
const char *mqtt_user_password =         "THE MQTT USER PASSWORD";
const char *sensor_id =                  "ESP32_MQTT_Sensor_1";
const char *mqtt_sensor_status_topic =   "ESP32_MQTT_Sensor_1/Status";
const char *mqtt_sensor_data_topic =     "ESP32_MQTT_Sensor_1/Data";
const char *mqtt_sensor_incoming_topic = "ESP32_MQTT_Sensor_1/Incoming";

WiFiClient espClient;
PubSubClient client(espClient);

uint32_t last_time_data_was_published = 0;

void setup_wifi()
{
    delay(10);

    //Connect to the wifi network.
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.printf("\r\nINFO: Message arrived on [%s]", mqtt_sensor_incoming_topic);
   
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println(); 
}

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("INFO: Attempting MQTT connection...");

        if (client.connect("ESP32_MQTT_Sensor_1", "mqtt_user", "mqtt:)"))
        {
            Serial.println("\r\nINFO: connected");
            //Send a status message.
            client.publish(mqtt_sensor_status_topic, "connected");
            //Subscribe to the incoming communication topic.
            client.subscribe(mqtt_sensor_incoming_topic);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void publishData(void)
{
    char data[200];
    //Fake data. Replace with actual sensor data. 
    float temperature_sensor_reading = 75.0f; 
    float humidity_sensor_reading = 65.0f; 

    //Stupid way to get some changes in my fake data. 
    uint32_t fake_variance = millis() % 99; 
    while(fake_variance > 3)
    {
        fake_variance /= 2; 
    }
    
    snprintf(data, sizeof(data), "{ \"Temperature\": %.1f, \"Humidity\": %.1f }", temperature_sensor_reading + (float)fake_variance, humidity_sensor_reading - (float)fake_variance );
    Serial.printf("\r\nINFO: Publishing [%s] to [%s]", data, mqtt_sensor_data_topic);
            
    client.publish(mqtt_sensor_data_topic, data);

    yield();
}

void setup()
{
    Serial.begin(115200);
    setup_wifi();
    //Connect to the the MQTT broker running on Home Assistant.
    //Use the IP address of the HA server and the port that you configured while setting up the MQTT broker. 
    client.setServer(mqtt_server_ip, mqtt_port);
    client.setCallback(callback);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();
    //Publish every 10 seconds. 
    if (millis() - last_time_data_was_published > 10000)
    {
        last_time_data_was_published = millis(); 
        publishData();
    }
}
