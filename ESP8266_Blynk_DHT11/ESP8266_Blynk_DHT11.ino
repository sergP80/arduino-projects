/*
 Link to boards https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
 Dependencies:
  Blynk,
  Dht sensor library
  Adafruit Common Sens Lab
  NOTE! Don't forget to choose ESPxx board
*/
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiManager.h>
#include <DHT.h>

#define DHTTYPE DHT11

#define dht_dpin D4

DHT dht(dht_dpin, DHTTYPE); 

#define MQ3_PIN A0
#define LED_RED_PIN D5

float baseLine = 0.0f;

int prevButtonPressTimeStamp = 0;
const int buttonPressInterval = 100;

BlynkTimer timer;
const char api_key[] = "vRKgqMy5E1FOrSZ0HeM0ORCZ8d71ya7W";
const char* wifi_ap_name = "TempHumMonitorAP";
const char* wifi_ap_key = "tpcs2020";

void setup()
{
    Serial.begin(115200);
    
    if (WiFi.SSID() == "")
    {
      Serial.println("No saved connections");
      initConnection(); 
    }
    else
    {
      Serial.println("Try to connect to last network...");
      WiFi.begin(WiFi.SSID().c_str(), WiFi.psk().c_str());
      
      delay(3000);
      
      if (WiFi.waitForConnectResult() != WL_CONNECTED)
      {
        initConnection();
      }    
    }

    Blynk.begin(api_key, WiFi.SSID().c_str(), WiFi.psk().c_str());
    
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());
    
    dht.begin();
    
    pinMode(MQ3_PIN, INPUT);    
    calibrateMQ3(100);

    pinMode(LED_RED_PIN, OUTPUT);    
    digitalWrite(LED_RED_PIN, LOW);
    timer.setInterval(1000L, sendSensorData);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("=======================");
    Serial.println("Not connected. Try to reconnect...");
    Serial.println("=======================");
    ESP.reset();
  } else 
  {
    Blynk.run();
    timer.run();
  }
}

void initConnection()
{
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  wifiManager.autoConnect(wifi_ap_name, wifi_ap_key);
}

void sendSensorData()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  } 
  Serial.println("Humidity and temperature\n\n");
  Serial.print("Current humidity = ");
  Serial.print(h);
  Serial.print("%  ");
  Serial.print("temperature = ");
  Serial.println(t); 
  Blynk.virtualWrite(V5, t);
  Blynk.virtualWrite(V6, h);

  float BAC = getMQ3Data();
  if (isnan(BAC))
  {
    Serial.println("Failed to read from MQ3 sensor!");
    return;
  }
  Serial.print("Current C2H5OH level: ");
  Serial.print(BAC);
  Serial.println("%%");
  Blynk.virtualWrite(V7, BAC);
}

float getMQ3Data()
{
  int count = 10;
  float sensorValue = 0.0f;
  for (int i = 0; i < count; ++i)
  {
    sensorValue += analogRead(MQ3_PIN);
    delay(10);
  }
  
  sensorValue /= count;
  sensorValue -= baseLine;
  
  float outputValue = map(sensorValue, 0, 1023.0, 0, 255.0);
  float outputVoltage = outputValue*(5.0/255.0);
  float BAC = 0.0;
  if (outputVoltage > 0.10)
  {
    BAC = -0.6694f*((outputVoltage)*(outputVoltage)) + (0.5634f*outputVoltage) + 0.0451f;
    if (BAC>=0.0f)
    {
      BAC=BAC;
    }
    else
    {
      BAC=0.00f; 
    }
  }
  else 
    if (-0.06f <= outputVoltage &&  outputVoltage < 0.04f)
    {
      BAC = 0.02f;
    }
    else
      if (0.04f <= outputVoltage && outputVoltage <= 0.10f)
      {
        BAC = 0.08f;
      }
  return BAC;    
}

void calibrateMQ3(int samples)
{
  float result;
  for (int i = 0; i < samples; ++i)
  {
    float value = analogRead(MQ3_PIN);
    result = i ? (result + value)/2 : value;
    delay(100);
  }

   baseLine = result + 20;
}

BLYNK_WRITE(V8) {
  if (param.asInt())
  {
    prevButtonPressTimeStamp = millis();
  }
  else
  {
    if (millis() - prevButtonPressTimeStamp > buttonPressInterval)
    {
      Serial.println("Reset device");
      ESP.reset();
    }
  }
}

BLYNK_WRITE(V9) {
  Serial.println("Signal led");
  digitalWrite(LED_RED_PIN, param.asInt());
}
