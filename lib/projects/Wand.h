#ifndef WAND_H
#define WAND_H

#include <Arduino.h>
#include <FastLED.h>
#include <credentials.h>
#include "Framework.h"
#include "Project.h"
#include "WifiCore.h"

class Wand : public Project<0> {

public:

    WifiCore wifi;

    Wand(): wifi(WIFI_SSID, WIFI_PASSWORD) {}

    void initialize(Framework<0>& framework) {
        wifi.setupStation();
    }

    void tick() override {
        wifi.checkConnection();
    }
};

#endif // WAND_H