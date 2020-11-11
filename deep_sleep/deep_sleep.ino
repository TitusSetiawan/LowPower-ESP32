#include <ModbusMaster.h>
#include <AntaresESP32MQTT.h>

///////// PIN /////////
#define MAX485_DE      4
#define MAX485_RE_NEG  0
#define RX2 16
#define TX2 17
#define greenLed 26

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10        /* Time ESP32 will go to sleep (in seconds) */
#define WIFISSID "......"
#define PASSWORD "haha1112"
#define ACCESSKEY ""
#define projectName ""
#define deviceName ""

RTC_DATA_ATTR int bootCount = 0;

uint8_t result;
float temp, hum;


ModbusMaster node;
AntaresESP32MQTT antares(ACCESSKEY);
void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

void read_sensor()
{
  antares.setMqttServer();
  antares.checkMqttConnection();
  result = node.readInputRegisters(0x0001, 2);
  if (result == node.ku8MBSuccess)            // add !(isnan(humi) || isnan(temp)) when use DHT sensor
  {
    temp = node.getResponseBuffer(0) / 10.0f;
    hum = node.getResponseBuffer(1) / 10.0f;
    Serial.print("Temp = ");
    Serial.print(temp);
    Serial.print(" Hum = ");
    Serial.print(hum);
    antares.add("Temp", temp);
    antares.add("Hum", hum);
    antares.publish(projectName, deviceName);
    //    Serial.println(publishCek);
    //    Serial.println();
    delay(1500);
  }
}

void deep_sleep()
{
  digitalWrite(greenLed, LOW);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON); // all RTC Peripherals are powered
  esp_deep_sleep_start();
}

void setup() {
  pinMode(greenLed, OUTPUT);
  digitalWrite(greenLed, HIGH);
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RX2, TX2);
  node.begin(1, Serial2);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  //  antares.setDebug(true);
  antares.wifiConnection(WIFISSID, PASSWORD);
  read_sensor();
  deep_sleep();
}

void loop() {
}
