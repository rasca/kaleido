#ifndef GYRO_H
#define GYRO_H

////// Accelerometer //////
#include <Arduino.h>
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <helper_3dmath.h>

#define INTERRUPT_PIN 19


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

// GyroData gyroData;  // ugly hack so that static methods needed for EspNow can access it
VectorFloat gyroData;

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

class Gyro {
private:
    MPU6050 mpu;

    // MPU control/status vars
    bool dmpReady = false;  // set true if DMP init was successful
    uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
    uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
    uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
    uint16_t fifoCount;     // count of all bytes currently in FIFO
    uint8_t fifoBuffer[64]; // FIFO storage buffer

public:
    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    void initialize(){

        Serial.println("Gyro init");
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
        mpu.initialize();
        pinMode(INTERRUPT_PIN, INPUT);

        Serial.println("Testing device connections...");

        // Try to initialize!
        if (!mpu.testConnection()) {
            Serial.println("Failed to find MPU6050 chip");
            Serial.println("Restarting...");
            ESP.restart();
        }
        Serial.println("MPU6050 Found!");

        // wait for ready
        // Serial.println(F("\nSend any character to begin DMP programming and demo: "));
        // while (Serial.available() && Serial.read()); // empty buffer
        // while (!Serial.available()); // wait for data
        // while (Serial.available() && Serial.read()); // empty buffer again

        // load and configure the DMP
        Serial.println(F("Initializing DMP..."));
        devStatus = mpu.dmpInitialize();

        // make sure it worked (returns 0 if so)
        if (devStatus == 0)
        {
            // Calibration Time: generate offsets and calibrate our MPU6050
            mpu.CalibrateAccel(6);
            mpu.CalibrateGyro(6);
            mpu.PrintActiveOffsets();
            // turn on the DMP, now that it's ready
            Serial.println(F("Enabling DMP..."));
            mpu.setDMPEnabled(true);

            // enable Arduino interrupt detection
            Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
            Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
            Serial.println(F(")..."));
            attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
            mpuIntStatus = mpu.getIntStatus();

            // set our DMP Ready flag so the main loop() function knows it's okay to use it
            Serial.println(F("DMP ready! Waiting for first interrupt..."));
            dmpReady = true;

            // get expected DMP packet size for later comparison
            packetSize = mpu.dmpGetFIFOPacketSize();
        }
        else
        {
            // ERROR!
            // 1 = initial memory load failed
            // 2 = DMP configuration updates failed
            // (if it's going to break, usually the code will be 1)
            Serial.print(F("DMP Initialization failed (code "));
            Serial.print(devStatus);
            Serial.println(F(")"));
        }
    }

    VectorFloat accumulatedRotation;

    Quaternion q;

    void getQuaternion(Quaternion& q) {
        mpu.dmpGetQuaternion(&q, fifoBuffer);
    }

    VectorFloat ypr;

    VectorFloat getYawPitchRoll()
    {
        // Code from http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
        // referenced by https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

        mpu.dmpGetQuaternion(&q, fifoBuffer);

        float test = q.x * q.y + q.z * q.w;
        if (test > 0.499)
        { // singularity at north pole
            ypr.x = 2 * atan2(q.x, q.w);
            ypr.y = M_PI / 2;
            ypr.z = 0;
        }
        else if (test < -0.499)
        { // singularity at south pole
            ypr.x = -2 * atan2(q.x, q.w);
            ypr.y = -M_PI / 2;
            ypr.z = 0;
        }
        else
        {
            float sqx = q.x * q.x;
            float sqy = q.y * q.y;
            float sqz = q.z * q.z;
            ypr.x = atan2(2 * q.y * q.w - 2 * q.x * q.z, 1 - 2 * sqy - 2 * sqz);
            ypr.y = asin(2 * test);
            ypr.z = atan2(2 * q.x * q.w - 2 * q.y * q.z, 1 - 2 * sqx - 2 * sqz);
        }

        ypr.x = ypr.x * 180.0f / M_PI;
        ypr.y = ypr.y * 180.0f / M_PI;
        ypr.z = ypr.z * 180.0f / M_PI;
        return ypr;
    }

    Quaternion lastQuaternion;
    Quaternion qAcc;

    Quaternion getAccQuaternionDifference()
    {
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        qAcc.w += q.w - lastQuaternion.w;
        qAcc.x += q.x - lastQuaternion.x;
        qAcc.y += q.y - lastQuaternion.y;
        qAcc.z += q.z - lastQuaternion.z;
        lastQuaternion = q;
        return qAcc;
    }

    VectorInt16 accel;
    VectorInt16 gyro;

    void getRawAccelGyro()
    {
        mpu.dmpGetAccel(&accel, fifoBuffer);
        mpu.dmpGetGyro(&gyro, fifoBuffer);
    }

    static void print(VectorFloat& v) {
        Serial.print("vector\t");
        Serial.print(v.x);
        Serial.print("\t");
        Serial.print(v.y);
        Serial.print("\t");
        Serial.print(v.z);
        Serial.print("\t");
    }

    static void print(VectorInt16& v) {
        Serial.print("vector int\t");
        Serial.print(v.x);
        Serial.print("\t");
        Serial.print(v.y);
        Serial.print("\t");
        Serial.print(v.z);
        Serial.print("\t");
    }

    static void print(Quaternion& q) {
        Serial.print("quaternion\t");
        Serial.print(q.w);
        Serial.print("\t");
        Serial.print(q.x);
        Serial.print("\t");
        Serial.print(q.y);
        Serial.print("\t");
        Serial.print(q.z);
        Serial.print("\t");
    }

    void tick() {
        if (!dmpReady)
            return;
        // read a packet from FIFO
        if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer))
        { // Get the Latest packet
            ypr = getYawPitchRoll();
            // Serial.print("ypr\t");
            // print(ypr);
            // Serial.println();

            // Quaternion q = getAccQuaternionDifference();
            // Serial.print("q\t");
            // print(q);
            // Serial.println();

            // getRawAccelGyro();
            // Serial.print("accel\t");
            // print(accel);
            // Serial.print("gyro\t");
            // print(gyro);
            // Serial.println();
        }
    }

    void print() {
        Serial.print("AccelX:");
        Serial.print(ax);
        Serial.print(",\t");
        Serial.print("AccelY:");
        Serial.print(ay);
        Serial.print(",\t");
        Serial.print("AccelZ:");
        Serial.print(az);
        Serial.print(",\t");
        Serial.print("GyroX:");
        Serial.print(gx);
        Serial.print(",\t");
        Serial.print("GyroY:");
        Serial.print(gy);
        Serial.print(",\t");
        Serial.print("GyroZ:");
        Serial.print(gz);
        Serial.println("");
    }
};

#endif // GYRO_H