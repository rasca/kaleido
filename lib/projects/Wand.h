#ifndef WAND_H
#define WAND_H

#include <Arduino.h>
#include <FastLED.h>
#include <credentials.h>
#include "Framework.h"
#include "Project.h"
#include "EspNow.h"
#include "Gyro.h"

class Wand : public Project<0> {

public:

    EspNow<GyroData> espNow;
    Gyro gyro;

    void initialize(Framework<0>& framework) {
        espNow.setup(OnDataRecv, OnDataSent);
        espNow.setupBroadcast();
        gyro.initialize();
    }

    static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
    {
        // Serial.print("\r\nLast Packet Send Status:\t");
        // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    }
    static void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {}

    void tick() override {
        gyro.tick();
        // gyro.print();

        espNow.data.a_x = gyro.a.acceleration.x;
        espNow.data.a_y = gyro.a.acceleration.y;
        espNow.data.a_z = gyro.a.acceleration.z;
        espNow.data.g_x = gyro.g.gyro.x;
        espNow.data.g_y = gyro.g.gyro.y;
        espNow.data.g_z = gyro.g.gyro.z;
        espNow.data.t = gyro.temp.temperature;

        espNow.send();
    }
};

#endif // WAND_H