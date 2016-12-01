// Example testing sketch for various DHT11, DHT21, DHT22 humidity/temperature sensors
// Written by Lin Wei Ting in National Taipei University of Technology Applied Network Laboratory. Taipei, Taiwan.
#include <Wire.h>
#include "LDHT.h"
#include "Arduino.h"
#include "SI114X.h"
#include <Timer.h>
#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiClient.h>

#define DHTPIN 4          // what pin we're connected to
#define DHTTYPE DHT11     // using DHT11 sensor
#define WIFI_AP "CHT3961"
#define WIFI_PASSWORD "0975281521"

#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.
//#define per 50
//#define per1 3
#define DEVICEID "DskkFj40" // Input your deviceId
#define DEVICEKEY "q0EOWS9W1GD3aoMT" // Input your deviceKey
#define SITE_URL "api.mediatek.com"
LDHT dht(DHTPIN,DHTTYPE);
LWiFiClient c2;
SI114X SI1145 = SI114X();
 
float tempC=0.0,Humi=0.0;
float HeatIndex=0.0;
float uv=0.0;
Timer updateTime;
void setup()
{
    Serial.begin(115200);
    dht.begin();
    LTask.begin();
    LWiFi.begin();	
    Serial.print(DHTTYPE);
    Serial.println(" test!");
    Serial.println("Connecting to AP");
    while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
    {
      Serial.print("GGGGG");
      delay(1000);
    }
    while (!SI1145.Begin()) {
    Serial.println("Si1145 is not ready!");
    delay(1000);
   }
    Serial.println("calling connection");
     updateTime.every(15000, uploadData);
}

void loop()
{
    updateTime.update();
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    if(dht.read())
    {
        tempC = dht.readTemperature();
        Humi = dht.readHumidity();


        Serial.println("------------------------------");
        Serial.print("Temperature Celcius = ");
        Serial.print(dht.readTemperature());
        Serial.println("C");

        Serial.print("Temperature Fahrenheit = ");
        Serial.print(dht.readTemperature(false));
        Serial.println("F");

        Serial.print("Humidity = ");
        Serial.print(dht.readHumidity());
        Serial.println("%");

        Serial.print("HeatIndex = ");
        Serial.print(dht.readHeatIndex(tempC,Humi));
        HeatIndex=dht.readHeatIndex(tempC,Humi);
        Serial.println(HeatIndex);
        Serial.println("C");

        Serial.print("DewPoint = ");
        Serial.print(dht.readDewPoint(tempC,Humi));
        Serial.println("C");
    }
    Serial.print("Vis: "); Serial.println(SI1145.ReadVisible());
  Serial.print("IR: "); Serial.println(SI1145.ReadIR());
  //the real UV value must be div 100 from the reg value , datasheet for more information.
  Serial.print("UV: ");  Serial.println((float)SI1145.ReadUV()/100);
  uv=(float)SI1145.ReadUV()/100;

    delay(500);
}
void uploadSteps()
{
  char upload_steps[] = "tem,,";
  char stepsC[20];
  sprintf(stepsC, "%.2f",  HeatIndex);
  Serial.println("calling connectionn");
  LWiFiClient c2;
  while (!c2.connect(SITE_URL, 80))
  {
    Serial.println("Re-Connecting to WebSite");
    delay(1000);
  }

  int thislength = strlen(upload_steps) + strlen(stepsC);
  Serial.println(stepsC);
  c2.print("POST /mcs/v2/devices/");
  c2.print(DEVICEID);
  c2.println("/datapoints.csv HTTP/1.1");
  c2.print("Host: ");
  c2.println(SITE_URL);
  c2.print("deviceKey: ");
  c2.println(DEVICEKEY);
  c2.print("Content-Length: ");
  c2.println(thislength);
  c2.println("Content-Type: text/csv");
  c2.println("Connection: close");
  c2.println();
  c2.print(upload_steps);
  c2.println(HeatIndex);
  //delay(500);

  int errorcount = 0;
  while (!c2.available())
  {
    Serial.print("waiting HTTP response: ");
    Serial.println(errorcount);
    errorcount += 1;
    if (errorcount > 10) {
      c2.stop();
      return;
    }
    delay(100);
  }
  
  while (c2)
  {
    int v = c2.read();
    if (v != -1)
    {
      Serial.print(char(v));
    }
    else
    {
      Serial.println("no more content, disconnect");
      c2.stop();

    }
  }
}
void uploadHumidity(){
  char upload_hum[] = "hum,,";
  char humC[20];
  sprintf(humC, "%.2f",  Humi);
  Serial.println("calling connectionn");
  LWiFiClient c2;
  while (!c2.connect(SITE_URL, 80))
  {
    Serial.println("Re-Connecting to WebSite");
    delay(1000);
  }

  int thislength = strlen(upload_hum) + strlen(humC);
  Serial.println(humC);
  c2.print("POST /mcs/v2/devices/");
  c2.print(DEVICEID);
  c2.println("/datapoints.csv HTTP/1.1");
  c2.print("Host: ");
  c2.println(SITE_URL);
  c2.print("deviceKey: ");
  c2.println(DEVICEKEY);
  c2.print("Content-Length: ");
  c2.println(thislength);
  c2.println("Content-Type: text/csv");
  c2.println("Connection: close");
  c2.println();
  c2.print(upload_hum);
  c2.println(Humi);
  //delay(500);

  int errorcount = 0;
  while (!c2.available())
  {
    Serial.print("waiting HTTP response: ");
    Serial.println(errorcount);
    errorcount += 1;
    if (errorcount > 10) {
      c2.stop();
      return;
    }
    delay(100);
  }
  
  while (c2)
  {
    int v = c2.read();
    if (v != -1)
    {
      Serial.print(char(v));
    }
    else
    {
      Serial.println("no more content, disconnect");
      c2.stop();

    }
  }

}
void uploadUV(){
  char upload_uv[] = "uvlight,,";
  char uvC[20];
  sprintf(uvC, "%.2f",  uv);
  Serial.println("calling connectionn");
  LWiFiClient c2;
  while (!c2.connect(SITE_URL, 80))
  {
    Serial.println("Re-Connecting to WebSite");
    delay(1000);
  }

  int thislength = strlen(upload_uv) + strlen(uvC);
  Serial.println(uvC);
  c2.print("POST /mcs/v2/devices/");
  c2.print(DEVICEID);
  c2.println("/datapoints.csv HTTP/1.1");
  c2.print("Host: ");
  c2.println(SITE_URL);
  c2.print("deviceKey: ");
  c2.println(DEVICEKEY);
  c2.print("Content-Length: ");
  c2.println(thislength);
  c2.println("Content-Type: text/csv");
  c2.println("Connection: close");
  c2.println();
  c2.print(upload_uv);
  c2.println(uv);
  //delay(500);

  int errorcount = 0;
  while (!c2.available())
  {
    Serial.print("waiting HTTP response: ");
    Serial.println(errorcount);
    errorcount += 1;
    if (errorcount > 10) {
      c2.stop();
      return;
    }
    delay(100);
  }
  
  while (c2)
  {
    int v = c2.read();
    if (v != -1)
    {
      Serial.print(char(v));
    }
    else
    {
      Serial.println("no more content, disconnect");
      c2.stop();

    }
  }

}
void uploadData()
{
   
  Serial.println("ZZZZZZZZ");
  
  uploadSteps();
  uploadHumidity();
  uploadUV();
}
