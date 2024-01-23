#ifndef WAND_H
#define WAND_H

#include <Arduino.h>
#include <FastLED.h>
#include <credentials.h>
#include "Framework.h"
#include "Project.h"
#include "WifiCore.h"
#include "Gyro.h"

class Wand : public Project<0> {

public:

    WifiCore wifi;
    Gyro gyro;

    Wand(): wifi((char*)WIFI_SSID, (char*)WIFI_PASSWORD) {}

    void initialize(Framework<0>& framework) {
        wifi.setupStation();
        gyro.initialize();
    }

    void tick() override {
        wifi.checkConnection();
        gyro.tick();
        gyro.print();
    }
};

#endif // WAND_H