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

const int16_t TICK_MOVEMENT_THRESHOLD = 80;
const unsigned long STILL_TIME_TO_SLEEP_NO_MOVEMENT_MS = 1000;
const unsigned long STILL_TIME_TO_SLEEP_MOVEMENT_MS = 60000;
const uint64_t SLEEP_TIME_US = 20000000;
const unsigned long INITIAL_TICKS = 800;

class Wand : public Project<0>
{

public:
    EspNow<GyroData> espNow;
    int16_t lastAccelX, lastAccelY, lastAccelZ;
    Gyro gyro;
    unsigned long lastMovementTime;
    unsigned long ticks;
    bool hasBeenMoved;

    void initialize(Framework<0> &framework)
    {
        espNow.setup(OnDataRecv, OnDataSent);
        espNow.setupBroadcast();
        gyro.initialize();
        lastMovementTime = millis();
        ticks = 0;
        hasBeenMoved = false;
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

        if (ticks > INITIAL_TICKS)
        {
            if (accelDelta > TICK_MOVEMENT_THRESHOLD)
            {
                espNow.send();
                lastMovementTime = millis();
                hasBeenMoved = true;
                // Serial.println("Moved");
            }
            else
            {
                const unsigned long stillTime = millis() - lastMovementTime;
                // Serial.print("Still time: ");
                // Serial.println(stillTime);
                if ((!hasBeenMoved && stillTime >= STILL_TIME_TO_SLEEP_NO_MOVEMENT_MS) || stillTime >= STILL_TIME_TO_SLEEP_MOVEMENT_MS)
                {
                    // Serial.println("Sleeping");
                    ESP.deepSleep(SLEEP_TIME_US);
                }
            }
        } else {
            lastMovementTime = millis();
        }
        lastAccelX = gyroData.accel_x;
        lastAccelY = gyroData.accel_y;
        lastAccelZ = gyroData.accel_z;
        // Serial.print(lastAccelX);
        // Serial.print("\t");
        // Serial.print(lastAccelY);
        // Serial.print("\t");
        // Serial.println(lastAccelZ);
        ticks++;
    }
};

#endif // WAND_H