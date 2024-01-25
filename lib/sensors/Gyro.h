#ifndef GYRO_H
#define GYRO_H

////// Accelerometer //////
#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>


// struct for esp now sending of gyro data
typedef struct GyroData
{
    float a_x;
    float a_y;
    float a_z;
    float g_x;
    float g_y;
    float g_z;
    float t;
} GyroData;

GyroData gyroData;  // ugly hack so that static methods needed for EspNow can access it

class Gyro {
private:
    Adafruit_MPU6050 mpu;

public:
    sensors_event_t a, g, temp;

    void initialize(){

        Serial.println("Gyro init");

        // Try to initialize!
        if (!mpu.begin()) {
            Serial.println("Failed to find MPU6050 chip");
            Serial.println("Restarting...");
            ESP.restart();
        }
        Serial.println("MPU6050 Found!");

        //setupt motion detection
        // mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
        // mpu.setMotionDetectionThreshold(1);
        // mpu.setMotionDetectionDuration(20);
        mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
        mpu.setInterruptPinPolarity(true);
        // mpu.setMotionInterrupt(true);

        Serial.println("");
        delay(100);
    }

    void tick() {
        mpu.getEvent(&a, &g, &temp);
    }

    void print() {
        Serial.print("AccelX:");
        Serial.print(a.acceleration.x);
        Serial.print(",\t");
        Serial.print("AccelY:");
        Serial.print(a.acceleration.y);
        Serial.print(",\t");
        Serial.print("AccelZ:");
        Serial.print(a.acceleration.z);
        Serial.print(",\t");
        Serial.print("GyroX:");
        Serial.print(g.gyro.x);
        Serial.print(",\t");
        Serial.print("GyroY:");
        Serial.print(g.gyro.y);
        Serial.print(",\t");
        Serial.print("GyroZ:");
        Serial.print(g.gyro.z);
        Serial.println("");
    }
};

#endif // GYRO_H