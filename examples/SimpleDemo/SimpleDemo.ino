// DWDOpenDataPollenflugGefahrenindex
// Copyright (c) 2020 Chris Klinger
// MIT License

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#include "PollenflugGefahrenindex.h"

#define WIFI_SSID "change_me"
#define WIFI_PASS "change_me"
#define WIFI_HOSTNAME "DWDOpenData-PollenflugDemo"

void connectWifi()
{
  Serial.begin(115200);

  if (WiFi.status() == WL_CONNECTED)
    return;
  //Manual Wifi
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(WIFI_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    if (i > 80)
      i = 0;
    i += 10;
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected.");
}

void setup()
{
  Serial.println("DWDOpenDataPollenflugGefahrenindex Simple Demo");
  connectWifi();
}

void loop()
{
  PollenflugData pollenflugData;
  PollenflugGefahrenindex *pollenflugClient;

  // see https://opendata.dwd.de/climate_environment/health/alerts/Beschreibung_pollen_s31fg.pdf for region_id
  pollenflugClient->updateData(&pollenflugData, 31);

  Serial.println("updateData() done.");
  Serial.println();
  Serial.print("Region Name: ");
  Serial.println(pollenflugData.regionName);
  Serial.print("Part-Region Name: ");
  Serial.println(pollenflugData.partregionName);
  Serial.print("Last Update: ");
  Serial.println(pollenflugData.lastUpdate);

  Serial.print("Max Load Index for today: ");
  Serial.println(pollenflugClient->getDescription(pollenflugClient->getMaxLoadIndex(&pollenflugData.today)));

  Serial.print("Max Load Index for tommorow: ");
  Serial.println(pollenflugClient->getDescription(pollenflugClient->getMaxLoadIndex(&pollenflugData.tomorrow)));

  Serial.print("Load Index (Graeser) for today: ");
  Serial.println(pollenflugClient->getDescription(pollenflugData.today.graeser));

  Serial.print("Load Index (Graeser) for tomorrow: ");
  Serial.println(pollenflugClient->getDescription(pollenflugData.tomorrow.graeser));

  Serial.print("Load Index (Graeser) for dayafter tomorrow: ");
  Serial.println(pollenflugClient->getDescription(pollenflugData.dayafter_to.graeser));

  delay(60000); // wait one minute for next query
}