#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiManager.h>
#include <DHT.h>

#define DHTTYPE DHT11

#define dht_dpin D4

DHT dht(dht_dpin, DHTTYPE); 

BlynkTimer timer;
const char api_key[] = "vRKgqMy5E1FOrSZ0HeM0ORCZ8d71ya7W";
const char* wifi_ap_name = "TempHumMonitor";
const char* wifi_ap_key = "43dDdckl2342";

void setup()
{
    Serial.begin(115200);
    if (WiFi.SSID() == "")
    {
      initConnection(); 
    } else
    {
      WiFi.begin(WiFi.SSID().c_str(), WiFi.psk().c_str());
      if (WiFi.waitForConnectResult() != WL_CONNECTED)
      {
        initConnection();
      }    
    }

    Blynk.begin(api_key, WiFi.SSID().c_str(), WiFi.psk().c_str());
    
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());
    
    dht.begin();
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
}
