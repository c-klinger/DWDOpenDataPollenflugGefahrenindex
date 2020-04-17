#DWDOpenDataPollenflugGefahrenindex
DWDOpenDataPollenflugGefahrenindex is a C++ library for ESP8266 Microcontrollers that can load and parse OpenData information from Deutscher Wetterdienst (DWD) what different pollen types spreads through the air.

## Quickstart
The library also includes an example Sketch that demonstrate the basic usage.

### Create a Client and load data
For every use case you need to initalize the library and load the data from the DWD OpenData Server. Also check [Configuration](#Configuration) section how to use the `region_id` parameter.

```c
PollenflugData pollenflugData;
PollenflugGefahrenindex *pollenflugClient;

// see https://opendata.dwd.de/climate_environment/health/alerts/Beschreibung_pollen_s31fg.pdf for region_id
pollenflugClient->updateData(&pollenflugData, 31);
```
The returned forecast data is encoded as `uint8_t` values between 1 (no load) and 7 (high load). These are suitable if you want to create graphs or other visual representations. You can always use the `getDescription` method to get a description of the value.

### Get the max. Load Index
The `getMaxLoadIndex` method returns the highest value of all pollen load found for a specific forecast.
```c
Serial.println(pollenflugClient->getDescription(pollenflugClient->getMaxLoadIndex(&pollenflugData.today)));
```

###  Get the max. Load Index with filter for specific pollen types
The `getMaxLoadIndex` method also accepts an filter, to find the highest load value of specific pollen types. Following call f.e. will ignore Rogen and Graeser types.
```c
Serial.println(pollenflugClient->getDescription(pollenflugClient->getMaxLoadIndex(&pollenflugData.today, true, true, true, false, true, true, true, false)));
```

### Get an specific value
short use case description
```c
Serial.println(pollenflugClient->getDescription(pollenflugData.today.graeser));
```

## Configuration
An region_id is required to update the data. The library uses the region_ids from DWD, that are documented in the following [PDF file](https://opendata.dwd.de/climate_environment/health/alerts/Beschreibung_pollen_s31fg.pdf) on page 3ff.
If an greater region (id ending with 0) is used, all sub-regions will be aggregated using an maximum function. The parameter 0 will parse all regions of germany.

## License and credits
Copyright (c) 2018 Chris Klinger. Licensed under MIT license, see  [LICENSE](https://github.com/c-klinger/esp8266-arduino-wifirgb/blob/master/LICENSE.md) for the full license.

DWDOpenDataPollenflugGefahrenindex uses the following libraries and frameworks, which have their own licenses:

- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) [MIT]

## Support the project
Do you like this library? I would appreciate if you star this project on GitHub! 

## Issues and support
See <https://github.com/c-klinger/esp8266-arduino-wifirgb/issues>.