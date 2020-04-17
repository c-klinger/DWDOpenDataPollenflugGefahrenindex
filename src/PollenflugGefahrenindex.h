// DWDOpenDataPollenflugGefahrenindex
// Copyright (c) 2020 Chris Klinger
// MIT License

#ifndef PollenflugGefahrenindex_h
#define PollenflugGefahrenindex_h

#include "Arduino.h"

typedef struct  {
  int8_t esche;
  int8_t birke;
  int8_t ambrosia;
  int8_t roggen;
  int8_t erle;
  int8_t hasel;
  int8_t beifuss;
  int8_t graeser;
} PollenflugDetailData;

typedef struct  {
  // Forecast Data for today
  PollenflugDetailData today;
  // Forecast Data for tomorrow
  PollenflugDetailData tomorrow;
  // Forecast Data for day after tommorrow (only available on fridays)
  PollenflugDetailData dayafter_to;

  // "region_id": 30
  int region_id;
  // "partregion_id": 31
  int partregion_id; 
  
  // "regionName": "Niedersachsen und Bremen"
  String regionName;
  // "partregion_name": "Westl. Niedersachsen/Bremen"
  String partregionName;

  // "last_update": "2020-04-15 11:00 Uhr"
  String lastUpdate; 
} PollenflugData;

class PollenflugGefahrenindex {

  private:
    PollenflugData *data;
    uint8_t convertLoad(const char* loadLevel);

  public:
    // Default Constructor
    PollenflugGefahrenindex();

    // Default Destructor
    ~PollenflugGefahrenindex();

    /**
     * Updates the data from web server
     * 
     * @param *data - the data object where values are stored
     * @param region_id - region id to get data for, use 0 for all regions, see https://opendata.dwd.de/climate_environment/health/alerts/s31fg.json for regions
     */
    void updateData(PollenflugData *data, uint8_t region_id);

    /**
     * Converts internal values into readable descriptions.
     * 
     * 1: "keine Belastung",
     * 2: "keine bis geringe Belastung",
     * 3: "geringe Belastung",
     * 4: "geringe bis mittlere Belastung",
     * 5: "mittlere Belastung",
     * 6: "mittlere bis hohe Belastung"
     * 7: "hohe Belastung",
     * all other values: unknown
     * 
     * descriptions copied from https://opendata.dwd.de/climate_environment/health/alerts/s31fg.json in april 2020
     * 
     * @param loadLevel - internal load level from PollenflugDetailData to translate
     */
    String getDescription(uint8_t loadLevel);
    
    /**
     * Returns the max load index (highest value) of all pollen types for a data set
     * 
     * @param *data - the data set to investigate
     */
    uint8_t getMaxLoadIndex(PollenflugDetailData *data); 

    /**
     * Returns the max load index (highest value) for a data set.
     * The data set can be filtered for specific pollen types. If false is passed for a specific type this type will be skipped during calculation.
     * 
     * @param *data - the data set to investigate
     * @param esche - use pollen type esche
     * @param birke - use pollen type birke
     * @param ambrosia - use pollen type ambrosia
     * @param roggen - use pollen type roggen
     * @param erle - use pollen type erle
     * @param hasel - use pollen type hasel
     * @param beifuss - use pollen type beifuss
     * @param graeser - use pollen type graeser
     */
    uint8_t getMaxLoadIndex(PollenflugDetailData *data, bool esche, bool birke, bool ambrosia, bool roggen, bool erle, bool hasel, bool beifuss, bool graeser); 
};
#endif