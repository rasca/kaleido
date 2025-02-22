// Ovni.h
#ifndef BASE_OVNI_H
#define BASE_OVNI_H

#include <Arduino.h>
#include <FastLED.h>
#include "Framework.h"
#include "Project.h"
#include "PhysicalSegments.h"
#include "VirtualSegments.h"
#include "Utils.h"
// Add DMX library
#include <esp_dmx.h>

template<size_t NUM_LEDS>
class Ovni : public Project<NUM_LEDS> {

public:

    VirtualSegments<NUM_LEDS> vu_meter;
    VirtualSegments<NUM_LEDS> ambient;

    const dmx_port_t dmx_num = DMX_NUM_1;
    const dmx_config_t config = DMX_CONFIG_DEFAULT;

    const int personality_count = 1;

    dmx_personality_t personalities[1] = {
        {1, "Ovni"}
    };

    // Movement Sensor
    const int movement_sensor_pin = 21;

    // DMX
    const int tx_pin = 17;
    const int rx_pin = 16;
    const int rts_pin = 21;

    uint8_t data[DMX_PACKET_SIZE] = {0, 0, 0};

    
    Ovni() : Project<NUM_LEDS>(),
                 vu_meter(this->physicalSegments.leds),
                 ambient(this->physicalSegments.leds)
    {
        // // Constructor initializes FillEffect with lines_all and CRGB::Red
        auto segment = this->physicalSegments.addSegment(0, 5 * 60);  // dots 6 x 50led strings
        // FastLED.addLeds<WS2812, 26>(&(segment.getLEDs()[0]), segment.getSize());

        // facets
        segment = this->physicalSegments.addSegment(0, 5 * 60);  // 1 x 50led string
        FastLED.addLeds<WS2812, 14, GRB>(&(segment.getLEDs()[0]), segment.getSize());

        vu_meter.addSegment(0, 5 * 60); // all the dots
        // ambient.addSegment(300, 1 * 50); // all the facets
    }

    void initialize(Framework<NUM_LEDS>& framework) {
        FastLED.clear();

        // DMX setup
        dmx_driver_install(dmx_num, &config, personalities, personality_count);
        dmx_set_pin(dmx_num, tx_pin, tx_pin, rts_pin);

        pinMode(movement_sensor_pin, INPUT); // PIR motion sensor is determined is an input here.  
        pinMode(2,OUTPUT);

    }

    long long lastPaint = 0;
    int paintInterval = 2000; // millis
    bool on = false;


    void tick() override {

        if (millis() - lastPaint > paintInterval) {
            lastPaint = millis();

            if (on) {
                Serial.println("ON");
                data[0] = 20;
                data[1] = 70;
                data[2] = 0;
                data[3] = 0;
                data[4] = 0;
                data[5] = 0;
                data[6] = 0;
                on = false;
            } else {
                Serial.println("OFF");
                data[0] = 0;
                on = true;
            }

            if (digitalRead(movement_sensor_pin) == HIGH) {
                Serial.println("Movement detected");
                digitalWrite(2, HIGH);
            } else {
                Serial.println("No movement detected");
                digitalWrite(2, LOW);
            }
            dmx_write(dmx_num, data, DMX_PACKET_SIZE);
            dmx_send(dmx_num);

            // Clear previous frame
            FastLED.clear();

            FastLED.show();
        }
    }

};

#endif // OVNI_H
