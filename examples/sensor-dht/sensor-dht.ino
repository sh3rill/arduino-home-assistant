#include <Ethernet.h>
#include <ArduinoHA.h>
#include <DHT.h>

#define BROKER_ADDR IPAddress(192,168,0,146)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
unsigned long lastSentAt = millis();

// DHT Config
#define DHTPIN 3     // What pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

// DHT Sensor variables
float hum;  //Stores humidity value
float temp; //Stores temperature value

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

HASensor temp1("temp1");
HASensor hum1("hum1");

void onBeforeSwitchStateChanged(bool state, HASwitch* s)
{
    // this callback will be called before publishing new state to HA
    // in some cases there may be delay before onStateChanged is called due to network latency
}

void setup() {

    // DHT begin
    dht.begin();

    // you don't need to verify return status
    Ethernet.begin(mac);

    // set device's details (optional)
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");
    
    // configure sensor (optional)
    temp1.setUnitOfMeasurement("°C");
    temp1.setDeviceClass("temperature");
    temp1.setIcon("mdi:thermometer");
    temp1.setName("Home temperature");

    hum1.setUnitOfMeasurement("%");
    hum1.setDeviceClass("humidity");
    hum1.setIcon("mdi:water-percent");
    hum1.setName("Home humidity");

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();

    if ((millis() - lastSentAt) >= 2000) {
        lastSentAt = millis();
        
        hum = dht.readHumidity();
        temp = dht.readTemperature();

        temp1.setValue(temp);
        hum1.setValue(hum);

        // Supported data types:
        // uint32_t (uint16_t, uint8_t)
        // int32_t (int16_t, int8_t)
        // double
        // float
        // const char*
    }
}
