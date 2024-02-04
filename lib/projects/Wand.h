#ifndef WAND_H
#define WAND_H

#include <Arduino.h>
#include <FastLED.h>
#include <credentials.h>
#include "Framework.h"
#include "Project.h"
#include "EspNow.h"
#include "Gyro.h"
#include "Utils.h"

class Wand : public Project<0> {

public:

    EspNow<GyroData> espNow;
    int16_t lastAccelX, lastAccelY, lastAccelZ;
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

    void tick() override
    {
        gyro.tick();
        // gyro.print();

        espNow.data = gyroData;

        // Angular distance:
        // Quaternion qr0(1, 0, 0, 0);
        // Quaternion qr1(0, 1, 0, 0);
        // Quaternion qr2(0, 0, 1, 0);
        // Quaternion qr3(0, 0, 0, 1);
        // Quaternion q(gyroData.q_w, gyroData.q_x, gyroData.q_y, gyroData.q_z);

        // Serial.print(getAngularDistance(qr0, q));
        // Serial.print("\t");
        // Serial.print(getAngularDistance(qr1, q));
        // Serial.print("\t");
        // Serial.print(getAngularDistance(qr2, q));
        // Serial.print("\t");
        // Serial.print(getAngularDistance(qr3, q));
        // Serial.print("\t");
        // Serial.println("");


        int16_t accelDelta = abs(gyroData.accel_x - lastAccelX) + abs(gyroData.accel_y - lastAccelY) + abs(gyroData.accel_z - lastAccelZ);
        if (accelDelta > 30)
        {
            espNow.send();
        }
        lastAccelX = gyroData.accel_x;
        lastAccelY = gyroData.accel_y;
        lastAccelZ = gyroData.accel_z;
    }
};

#endif // WAND_H