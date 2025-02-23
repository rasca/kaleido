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
#include "../dmx/dmx.h"

template <size_t NUM_LEDS>
class Ovni : public Project<NUM_LEDS>
{

public:
    VirtualSegments<NUM_LEDS> upper;
    VirtualSegments<NUM_LEDS> lower;

    // PIR motion sensor input
    const int movement_sensor_pin = 21;

    // dmx
    int readcycle = 0;
    uint8_t send_value = 0;

    Ovni() : Project<NUM_LEDS>(),
             upper(this->physicalSegments.leds),
             lower(this->physicalSegments.leds)
    {
        // lower circle 5m x 28 pixel (84 leds, 3 per IC) x meter
        auto lower_pixels = 5 * 28;
        auto segment = this->physicalSegments.addSegment(0, lower_pixels);
        FastLED.addLeds<WS2811, 26, GRB>(&(segment.getLEDs()[0]), segment.getSize());

        // upper circle 5m x 28 pixel (84 leds, 3 per IC) x meter + 2m x 18 pixel (108 leds, 6 per IC) x meter
        auto upper_pixels = 5 * 28 + 2 * 18;
        segment = this->physicalSegments.addSegment(lower_pixels, upper_pixels);
        FastLED.addLeds<WS2811, 25, GRB>(&(segment.getLEDs()[0]), segment.getSize());

        lower.addSegment(0, lower_pixels);            // all the dots
        upper.addSegment(lower_pixels, upper_pixels); // all the dots
    }

    void initialize(Framework<NUM_LEDS> &framework)
    {
        FastLED.clear();

        pinMode(movement_sensor_pin, INPUT); // PIR motion sensor is determined is an input here.
        pinMode(2, OUTPUT);

        DMX::Initialize(DMXDirection::output);
    }

    long long lastPaint = 0;
    int paintInterval = 40; // millis
    bool movementDetected = false;

    void tick() override
    {

        if (digitalRead(movement_sensor_pin) == HIGH)
        {
            movementDetected = true;
        }
        if (millis() - lastPaint > paintInterval)
        {
            lastPaint = millis();

            // Clear previous frame
            FastLED.clear();

            auto upper_segment = upper.getSegments()[0];
            auto upper_leds = upper_segment.getLEDs();

            auto lower_segment = lower.getSegments()[0];
            auto lower_leds = lower_segment.getLEDs();

            if (movementDetected)
            {
                Serial.println("Movement detected");
                digitalWrite(2, HIGH);

                DMX::Write(1, 125);
                DMX::Write(2, 125);
                DMX::Write(3, 125);
                DMX::Write(4, 125);
                DMX::Write(5, 125);
                DMX::Write(6, 125);
                DMX::Write(7, 125);
                DMX::Write(8, 125);

                for (int i = 0; i < upper_segment.getSize(); ++i)
                {
                    upper_leds[i] = CRGB::Red; // Set each LED to white
                }

                for (int i = 0; i < lower_segment.getSize(); ++i)
                {
                    lower_leds[i] = CRGB::Red; // Set each LED to white
                }
            }
            else
            {
                Serial.println("No movement detected");
                digitalWrite(2, LOW);

                DMX::Write(1, 0);
                DMX::Write(2, 0);
                DMX::Write(3, 0);
                DMX::Write(4, 0);
                DMX::Write(5, 0);
                DMX::Write(6, 0);
                DMX::Write(7, 0);
                DMX::Write(8, 0);
                for (int i = 0; i < upper_segment.getSize(); ++i)
                {
                    upper_leds[i] = CRGB::Black; // Set each LED to white
                }

                for (int i = 0; i < lower_segment.getSize(); ++i)
                {
                    lower_leds[i] = CRGB::Black; // Set each LED to white
                }
            }

            FastLED.show();
            movementDetected = false;
        }
    }
};

#endif // OVNI_H
