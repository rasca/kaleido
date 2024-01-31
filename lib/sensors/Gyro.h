#ifndef GYRO_H
#define GYRO_H

////// Accelerometer //////
#include <Arduino.h>
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <helper_3dmath.h>
#include <Utils.h>

#define INTERRUPT_PIN 19


// struct for esp now sending of gyro data
typedef struct GyroData
{
    float yaw, pitch, roll;
    float q_w, q_x, q_y, q_z;
    float qAcc_w, qAcc_x, qAcc_y, qAcc_z;
    int16_t accel_x, accel_y, accel_z;
    int16_t gyro_x, gyro_y, gyro_z;
} GyroData;

GyroData gyroData;  // ugly hack so that static methods needed for EspNow can access it

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


    Quaternion getQuaternion() {
        Quaternion q;
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        gyroData.q_w = q.w;
        gyroData.q_x = q.x;
        gyroData.q_y = q.y;
        gyroData.q_z = q.z;
        return q;
    }


    void getYawPitchRoll()
    {
        // Code from http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
        // referenced by https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

        Quaternion q = getQuaternion();

        float test = q.x * q.y + q.z * q.w;
        if (test > 0.499)
        { // singularity at north pole
            gyroData.yaw = 2 * atan2(q.x, q.w);
            gyroData.pitch = M_PI / 2;
            gyroData.roll = 0;
        }
        else if (test < -0.499)
        { // singularity at south pole
            gyroData.yaw = -2 * atan2(q.x, q.w);
            gyroData.pitch = -M_PI / 2;
            gyroData.roll = 0;
        }
        else
        {
            float sqx = q.x * q.x;
            float sqy = q.y * q.y;
            float sqz = q.z * q.z;
            gyroData.yaw = atan2(2 * q.y * q.w - 2 * q.x * q.z, 1 - 2 * sqy - 2 * sqz);
            gyroData.pitch = asin(2 * test);
            gyroData.roll = atan2(2 * q.x * q.w - 2 * q.y * q.z, 1 - 2 * sqx - 2 * sqz);
        }

        gyroData.yaw = gyroData.yaw * 180.0f / M_PI;
        gyroData.pitch = gyroData.pitch * 180.0f / M_PI;
        gyroData.roll = gyroData.roll * 180.0f / M_PI;
    }

    void getYawPitchRoll2()
    {
        Quaternion q = getQuaternion();
        calculateEulerAngles(q, 2, 3, 1, gyroData.yaw, gyroData.pitch, gyroData.roll);
    }


    Quaternion lastQuaternion;

    Quaternion getAccQuaternionDifference()
    {
        Quaternion q = getQuaternion();
        gyroData.qAcc_w += q.w - lastQuaternion.w;
        gyroData.qAcc_x += q.x - lastQuaternion.x;
        gyroData.qAcc_y += q.y - lastQuaternion.y;
        gyroData.qAcc_z += q.z - lastQuaternion.z;
        lastQuaternion = q;
        return q;
    }

    void getRawAccelGyro()
    {
        VectorInt16 accel, gyro;
        mpu.dmpGetAccel(&accel, fifoBuffer);
        mpu.dmpGetGyro(&gyro, fifoBuffer);
        gyroData.accel_x = accel.x;
        gyroData.accel_y = accel.y;
        gyroData.accel_z = accel.z;
        gyroData.gyro_x = gyro.x;
        gyroData.gyro_y = gyro.y;
        gyroData.gyro_z = gyro.z;
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

    static void print(GyroData& data) {
        Serial.print("yaw:\t");
        Serial.print(data.yaw);
        Serial.print("\tpitch:\t");
        Serial.print(data.pitch);
        Serial.print("\troll:\t");
        Serial.print(data.roll);
        Serial.print("\t");

        Serial.print("q:\t");
        Serial.print(data.q_w);
        Serial.print("\t");
        Serial.print(data.q_x);
        Serial.print("\t");
        Serial.print(data.q_y);
        Serial.print("\t");
        Serial.print(data.q_z);
        Serial.print("\t");

        Serial.print("q diff acc:\t");
        Serial.print(data.qAcc_w);
        Serial.print("\t");
        Serial.print(data.qAcc_x);
        Serial.print("\t");
        Serial.print(data.qAcc_y);
        Serial.print("\t");
        Serial.print(data.qAcc_z);
        Serial.print("\t");

        Serial.print("accel:\t");
        Serial.print(data.accel_x);
        Serial.print("\t");
        Serial.print(data.accel_y);
        Serial.print("\t");
        Serial.print(data.accel_z);
        Serial.print("\t");

        Serial.print("gyro:\t");
        Serial.print(data.gyro_x);
        Serial.print("\t");
        Serial.print(data.gyro_y);
        Serial.print("\t");
        Serial.print(data.gyro_z);
        Serial.print("\t");

        Serial.println("");
    }

    void tick() {
        if (!dmpReady)
            return;
        // read a packet from FIFO
        if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer))
        { // Get the Latest packet
            getYawPitchRoll2();
            // Serial.print("ypr\t");
            // Serial.print(gyroData.yaw);
            // Serial.print("\t");
            // Serial.print(gyroData.pitch);
            // Serial.print("\t");
            // Serial.print(gyroData.roll);
            // Serial.println();

            getQuaternion();
            // Serial.print("q\t");
            // print(q);
            // Serial.println();

            getAccQuaternionDifference();
            // Serial.print("q\t");
            // print(q);
            // Serial.println();

            // gyroData.accel & gyroData.gyro
            getRawAccelGyro();
            // Serial.print("accel\t");
            // print(accel);
            // Serial.print("gyro\t");
            // print(gyro);
            // Serial.println();

            // print(gyroData);
        }
    }
};

#endif // GYRO_H