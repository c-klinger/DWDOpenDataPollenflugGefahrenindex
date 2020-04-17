// DWDOpenDataPollenflugGefahrenindex
// Copyright (c) 2020 Chris Klinger
// MIT License

#include "Arduino.h"
#include "PollenflugGefahrenindex.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#include <ArduinoJson.h>

//#define DEBUG // remove comment to get debug output on Serial

#ifdef DEBUG
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINTF(x, y) Serial.printf(x, y)
    #define DEBUG_PRINT(x) Serial.print(x)
#else
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTF(x, y)
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINT_DATA(x)
#endif

#ifdef DEBUG
void DEBUG_PRINT_DATA(PollenflugData *data)
{
    DEBUG_PRINTLN("roggen ");
    DEBUG_PRINT("  today ");
    DEBUG_PRINTLN(data->today.roggen);
    DEBUG_PRINT("  tomorrow ");
    DEBUG_PRINTLN(data->tomorrow.roggen);
    DEBUG_PRINT("  dayafter_to ");
    DEBUG_PRINTLN(data->dayafter_to.roggen);
    
    DEBUG_PRINT("hasel ");
    DEBUG_PRINT("  today ");
    DEBUG_PRINTLN(data->today.hasel);
    DEBUG_PRINT("  tomorrow ");
    DEBUG_PRINTLN(data->tomorrow.hasel);
    DEBUG_PRINT("  dayafter_to ");
    DEBUG_PRINTLN(data->dayafter_to.hasel);

    DEBUG_PRINT("erle ");
    DEBUG_PRINT("  today ");
    DEBUG_PRINTLN(data->today.erle);
    DEBUG_PRINT("  tomorrow ");
    DEBUG_PRINTLN(data->tomorrow.erle);
    DEBUG_PRINT("  dayafter_to ");
    DEBUG_PRINTLN(data->dayafter_to.erle);

    DEBUG_PRINT("esche ");
    DEBUG_PRINT("  today ");
    DEBUG_PRINTLN(data->today.esche);
    DEBUG_PRINT("  tomorrow ");
    DEBUG_PRINTLN(data->tomorrow.esche);
    DEBUG_PRINT("  dayafter_to ");
    DEBUG_PRINTLN(data->dayafter_to.esche);

    DEBUG_PRINT("birke ");
    DEBUG_PRINT("  today ");
    DEBUG_PRINTLN(data->today.birke);
    DEBUG_PRINT("  tomorrow ");
    DEBUG_PRINTLN(data->tomorrow.birke);
    DEBUG_PRINT("  dayafter_to ");
    DEBUG_PRINTLN(data->dayafter_to.birke);

    DEBUG_PRINT("ambrosia ");
    DEBUG_PRINT("  today ");
    DEBUG_PRINTLN(data->today.ambrosia);
    DEBUG_PRINT("  tomorrow ");
    DEBUG_PRINTLN(data->tomorrow.ambrosia);
    DEBUG_PRINT("  dayafter_to ");
    DEBUG_PRINTLN(data->dayafter_to.ambrosia);

    DEBUG_PRINT("graeser ");
    DEBUG_PRINT("  today ");
    DEBUG_PRINTLN(data->today.graeser);
    DEBUG_PRINT("  tomorrow ");
    DEBUG_PRINTLN(data->tomorrow.graeser);
    DEBUG_PRINT("  dayafter_to ");
    DEBUG_PRINTLN(data->dayafter_to.graeser);

    DEBUG_PRINT("beifuss ");
    DEBUG_PRINT("  today ");
    DEBUG_PRINTLN(data->today.beifuss);
    DEBUG_PRINT("  tomorrow ");
    DEBUG_PRINTLN(data->tomorrow.beifuss);
    DEBUG_PRINT("  dayafter_to ");
    DEBUG_PRINTLN(data->dayafter_to.beifuss);
}
#endif

// Fingerprint for api URL, expires on May 8, 2022, needs to be updated well before this date
const uint8_t fingerprint[20] = {0x63, 0x0c, 0xff, 0x84, 0x25, 0x32, 0x5e, 0xa8, 0x67, 0x7d, 0x7b, 0x09, 0xd5, 0x39, 0xd8, 0x76, 0x6a, 0x02, 0x78, 0x6a};

PollenflugGefahrenindex::PollenflugGefahrenindex()
{
}

void PollenflugGefahrenindex::updateData(PollenflugData *data, uint8_t region_id)
{
    DEBUG_PRINT("PollenflugGefahrenindex::updateData_begin, free heap:");
    DEBUG_PRINTLN(ESP.getFreeHeap());
    #ifdef DEBUG
    uint32_t maxHeapUsed = 0; // for debug purpose
    #endif
    DEBUG_PRINT("Region: ");
    DEBUG_PRINTLN(region_id);

    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setFingerprint(fingerprint);

    HTTPClient https;
    https.useHTTP10(true);

    if (https.begin(*client, "https://opendata.dwd.de/climate_environment/health/alerts/s31fg.json"))
    {


        DEBUG_PRINTLN("[HTTPS] GET...");
        // start connection and send HTTP header
        int httpCode = https.GET();
        DEBUG_PRINTF("[HTTPS] GET... code: %d\n", httpCode);
        DEBUG_PRINTLN();

        if (httpCode > 0)
        {
            DynamicJsonDocument doc(1220);

            WiFiClient &input = https.getStream();
            uint8_t parsedRegions = 0;

            data->today = {-1, -1, -1, -1, -1, -1, -1, -1};
            data->tomorrow = {-1, -1, -1, -1, -1, -1, -1, -1};
            data->dayafter_to = {-1, -1, -1, -1, -1, -1, -1, -1};

            input.find("\"content\":[");
            do
            {
                DeserializationError error = deserializeJson(doc, input);

                if (error)
                {
                    Serial.print(F("deserializeJson() failed: "));
                    Serial.println(error.c_str());
                    return;
                }
              
                if (doc["region_id"].as<int>() == region_id || doc["partregion_id"].as<int>() == region_id || region_id == 0)
                    {
                        data->lastUpdate = doc["last_update"].as<char *>();
                        if (region_id == 0)
                        {
                            DEBUG_PRINTLN("aggregate all regions");
                            data->regionName = F("GERMANY");
                            data->partregionName = F("ALL");

                        } else if (doc["region_id"].as<int>() % 10 == 0) {
                            DEBUG_PRINTLN("aggregate greater regions");
                            data->regionName = doc["region_name"].as<char *>();
                            data->partregionName = F("ALL");

                        } else {
                            data->regionName = doc["region_name"].as<char *>();
                            data->partregionName = doc["partregion_name"].as<char *>();
                        }
                        #ifdef DEBUG
                        parsedRegions++;
                        DEBUG_PRINT("parsing part_region ");
                        DEBUG_PRINTLN(doc["partregion_name"].as<char *>());
                        DEBUG_PRINTLN("values before assignment");
                        DEBUG_PRINT_DATA(data);
                        #endif

                        data->today.roggen = _max(data->today.roggen, convertLoad(doc["Pollen"]["Roggen"]["today"].as<char *>()));
                        data->tomorrow.roggen = _max(data->tomorrow.roggen, convertLoad(doc["Pollen"]["Roggen"]["tomorrow"].as<char *>()));
                        data->dayafter_to.roggen = _max(data->dayafter_to.roggen, convertLoad(doc["Pollen"]["Roggen"]["dayafter_to"].as<char *>()));

                        data->today.hasel = _max(data->today.hasel, convertLoad(doc["Pollen"]["Hasel"]["today"].as<char *>()));
                        data->tomorrow.hasel = _max(data->tomorrow.hasel, convertLoad(doc["Pollen"]["Hasel"]["tomorrow"].as<char *>()));
                        data->dayafter_to.hasel = _max(data->dayafter_to.hasel, convertLoad(doc["Pollen"]["Hasel"]["dayafter_to"].as<char *>()));

                        data->today.erle = _max(data->today.erle, convertLoad(doc["Pollen"]["Erle"]["today"].as<char *>()));
                        data->tomorrow.erle = _max(data->tomorrow.erle, convertLoad(doc["Pollen"]["Erle"]["tomorrow"].as<char *>()));
                        data->dayafter_to.erle = _max(data->dayafter_to.erle, convertLoad(doc["Pollen"]["Erle"]["dayafter_to"].as<char *>()));

                        data->today.esche = _max(data->today.esche, convertLoad(doc["Pollen"]["Esche"]["today"].as<char *>()));
                        data->tomorrow.esche = _max(data->tomorrow.esche, convertLoad(doc["Pollen"]["Esche"]["tomorrow"].as<char *>()));
                        data->dayafter_to.esche = _max(data->dayafter_to.esche, convertLoad(doc["Pollen"]["Esche"]["dayafter_to"].as<char *>()));

                        data->today.birke = _max(data->today.birke, convertLoad(doc["Pollen"]["Birke"]["today"].as<char *>()));
                        data->tomorrow.birke = _max(data->tomorrow.birke, convertLoad(doc["Pollen"]["Birke"]["tomorrow"].as<char *>()));
                        data->dayafter_to.birke = _max(data->dayafter_to.birke, convertLoad(doc["Pollen"]["Birke"]["dayafter_to"].as<char *>()));

                        data->today.ambrosia = _max(data->today.ambrosia, convertLoad(doc["Pollen"]["Ambrosia"]["today"].as<char *>()));
                        data->tomorrow.ambrosia = _max(data->tomorrow.ambrosia, convertLoad(doc["Pollen"]["Ambrosia"]["tomorrow"].as<char *>()));
                        data->dayafter_to.ambrosia = _max(data->dayafter_to.ambrosia, convertLoad(doc["Pollen"]["Ambrosia"]["dayafter_to"].as<char *>()));

                        data->today.graeser = _max(data->today.graeser, convertLoad(doc["Pollen"]["Graeser"]["today"].as<char *>()));
                        data->tomorrow.graeser = _max(data->tomorrow.graeser, convertLoad(doc["Pollen"]["Graeser"]["tomorrow"].as<char *>()));
                        data->dayafter_to.graeser = _max(data->dayafter_to.graeser, convertLoad(doc["Pollen"]["Graeser"]["dayafter_to"].as<char *>()));

                        data->today.beifuss = _max(data->today.beifuss, convertLoad(doc["Pollen"]["Beifuss"]["today"].as<char *>()));
                        data->tomorrow.beifuss = _max(data->tomorrow.beifuss, convertLoad(doc["Pollen"]["Beifuss"]["tomorrow"].as<char *>()));
                        data->dayafter_to.beifuss = _max(data->dayafter_to.beifuss, convertLoad(doc["Pollen"]["Beifuss"]["dayafter_to"].as<char *>()));

                        DEBUG_PRINTLN("values after assignment");
                        DEBUG_PRINT_DATA(data);
                        
                    }
                #ifdef DEBUG
                uint32_t heapCurrent = ESP.getFreeHeap();
                if (heapCurrent > maxHeapUsed)
                {
                    maxHeapUsed = heapCurrent;
                }
                #endif
                yield();
            } while (input.findUntil(",", "]"));

            DEBUG_PRINT("foundRegions ");
            DEBUG_PRINTLN(parsedRegions);

            #ifdef DEBUG
            uint32_t heapCurrent = ESP.getFreeHeap();
            if (heapCurrent > maxHeapUsed)
            {
                maxHeapUsed = heapCurrent;
            }
            #endif
        }
        else
        {
            Serial.printf("[HTTPS] GET... failed, error: %s\n\r", https.errorToString(httpCode).c_str());
        }

        https.end();
    }
    else
    {
        Serial.printf("[HTTPS] Unable to connect\n\r");
    }
    DEBUG_PRINT("PollenflugGefahrenindex::updateData_end, free heap:");
    DEBUG_PRINTLN(ESP.getFreeHeap());

    DEBUG_PRINT("PollenflugGefahrenindex::updateData_end, heap used (max):");
    DEBUG_PRINTLN(maxHeapUsed);
}

String PollenflugGefahrenindex::getDescription(uint8_t loadLevel)
{
    DEBUG_PRINTLN("getDescription(" + (String) int(loadLevel) + ")");

    switch (loadLevel)
    {
    case 1:
        return F("keine Belastung");
    case 2:
        return F("keine bis geringe Belastung");
    case 3:
        return F("geringe Belastung");
    case 4:
        return F("geringe bis mittlere Belastung");
    case 5:
        return F("mittlere Belastung");
    case 6:
        return F("mittlere bis hohe Belastung");
    case 7:
        return F("hohe Belastung");

    default:
        return F("error: unknown state");
    }
}

uint8_t PollenflugGefahrenindex::convertLoad(const char *loadLevel)
{
    DEBUG_PRINTLN("convertLoad(" + (String)loadLevel + ")");
    if (strcmp("0", loadLevel) == 0)
    {
        DEBUG_PRINTLN("  returns 1");
        return 1;
    }
    else if (strcmp("0-1", loadLevel) == 0)
    {
        DEBUG_PRINTLN("  returns 2");
        return 2;
    }
    else if (strcmp("1", loadLevel) == 0)
    {
        DEBUG_PRINTLN("  returns 3");
        return 3;
    }
    else if (strcmp("1-2", loadLevel) == 0)
    {
        DEBUG_PRINTLN("  returns 4");
        return 4;
    }
    else if (strcmp("2", loadLevel) == 0)
    {
        DEBUG_PRINTLN("  returns 5");
        return 5;
    }
    else if (strcmp("2-3", loadLevel) == 0)
    {
        DEBUG_PRINTLN("  returns 6");
        return 6;
    }
    else if (strcmp("3", loadLevel) == 0)
    {
        DEBUG_PRINTLN("  returns 7");
        return 7;
    }
    else
    {
        return -1;
    }
}

uint8_t PollenflugGefahrenindex::getMaxLoadIndex(PollenflugDetailData *data)
{
    return getMaxLoadIndex(data, true, true, true, true, true, true, true, true);
}

uint8_t PollenflugGefahrenindex::getMaxLoadIndex(PollenflugDetailData *data, bool esche, bool birke, bool ambrosia, bool roggen, bool erle, bool hasel, bool beifuss, bool graeser)
{
    uint8_t maxLoadIndex = 0;
    if (esche && data->esche >= maxLoadIndex)
    {
        maxLoadIndex = data->esche;
    }
    if (birke && data->birke >= maxLoadIndex)
    {
        maxLoadIndex = data->birke;
    }
    if (ambrosia && data->ambrosia >= maxLoadIndex)
    {
        maxLoadIndex = data->ambrosia;
    }
    if (roggen && data->roggen >= maxLoadIndex)
    {
        maxLoadIndex = data->roggen;
    }
    if (erle && data->erle >= maxLoadIndex)
    {
        maxLoadIndex = data->erle;
    }
    if (hasel && data->hasel >= maxLoadIndex)
    {
        maxLoadIndex = data->hasel;
    }
    if (beifuss && data->beifuss >= maxLoadIndex)
    {
        maxLoadIndex = data->beifuss;
    }
    if (graeser && data->graeser >= maxLoadIndex)
    {
        maxLoadIndex = data->graeser;
    }
    return maxLoadIndex;
}