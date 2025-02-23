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

    // Timer constants
    const int ON_TIME = 1000;     // Time in seconds to stay on after movement
    const int LASER_TIME = 1000;  // Time in seconds for laser to stay on
    const int LASER_COOLDOWN = 2; // Time in seconds before laser can trigger again
    unsigned long timer_start = 0;
    unsigned long laser_start = 0;
    unsigned long last_laser_end = 0;
    bool is_on = false;
    bool laser_is_on = false;

    Ovni() : Project<NUM_LEDS>(),
             upper(this->physicalSegments.leds),
             lower(this->physicalSegments.leds)
    {
        // lower circle 5m x 28 pixel (84 leds, 3 per IC) x meter
        auto lower_pixels = 5 * 14;
        auto segment = this->physicalSegments.addSegment(0, lower_pixels);
        FastLED.addLeds<WS2811, 26, BRG>(&(segment.getLEDs()[0]), segment.getSize());

        // upper circle 5m x 28 pixel (84 leds, 3 per IC) x meter + 2m x 18 pixel (108 leds, 6 per IC) x meter
        auto upper_pixels = 5 * 14 + 2 * 18;
        segment = this->physicalSegments.addSegment(lower_pixels, upper_pixels);
        FastLED.addLeds<WS2811, 25, BRG>(&(segment.getLEDs()[0]), segment.getSize());

        lower.addSegment(0, lower_pixels);            // all the dots
        upper.addSegment(lower_pixels, upper_pixels); // all the dots
    }

    void initialize(Framework<NUM_LEDS> &framework)
    {
        FastLED.clear();
        pinMode(movement_sensor_pin, INPUT);
        pinMode(2, OUTPUT);
        DMX::Initialize(DMXDirection::output);
    }

    void laserOn()
    {
        Serial.println("Laser ON");
        dmxOn();
        laser_is_on = true;
        last_laser_end = millis();
        laser_start = millis();
    }

    void laserOff()
    {
        Serial.println("Laser OFF");
        dmxOff();
        laser_is_on = false;
        last_laser_end = millis();
    }

    void toOn()
    {
        Serial.println("Turning ON");
        digitalWrite(2, HIGH);
        laserOn();
    }

    void toOff()
    {
        Serial.println("Turning OFF");
        digitalWrite(2, LOW);
        dmxOff();
    }

    void dmxOn()
    {
        DMX::Write(1, 125);
        DMX::Write(2, 90);
        DMX::Write(3, 60);
        DMX::Write(4, 10);
        DMX::Write(5, 20);
        DMX::Write(6, 91); // 91, 141
        DMX::Write(7, 235);
    }

    void dmxOff()
    {
        DMX::Write(1, 0);
        DMX::Write(2, 0);
        DMX::Write(3, 0);
        DMX::Write(4, 0);
        DMX::Write(5, 0);
        DMX::Write(6, 0);
        DMX::Write(7, 0);
    }

    void tick() override
    {
        if (digitalRead(movement_sensor_pin) == HIGH)
        {
            timer_start = millis(); // Reset timer on movement
            if (!is_on)
            {
                is_on = true;
                toOn();
            }
            else
            {

                // Start laser if cooldown period has passed
                if (!laser_is_on && millis() - last_laser_end > LASER_COOLDOWN * 1000)
                {
                    laserOn();
                }
            }
        }

        // Check if laser needs to be turned off (2 second timer)
        if (laser_is_on && (millis() - laser_start > LASER_TIME * 1000))
        {
            laserOff();
        }

        // Check if we need to turn off main lights (timer expired)
        if (is_on && (millis() - timer_start > ON_TIME * 1000))
        {
            is_on = false;
            toOff();
        }

        auto upper_segment = upper.getSegments()[0];
        auto upper_leds = upper_segment.getLEDs();
        auto lower_segment = lower.getSegments()[0];
        auto lower_leds = lower_segment.getLEDs();
        if (is_on)
        {
            for (int i = 0; i < upper_segment.getSize(); ++i)
            {
                upper_leds[i] = CRGB::Red;
            }
            for (int i = 0; i < lower_segment.getSize(); ++i)
            {
                lower_leds[i] = CRGB::Red;
            }
        }
        else
        {

            for (int i = 0; i < upper_segment.getSize(); ++i)
            {
                upper_leds[i] = CRGB::Black;
            }
            for (int i = 0; i < lower_segment.getSize(); ++i)
            {
                lower_leds[i] = CRGB::Black;
            }
        }

        upper_leds[upper_segment.getSize() - 1] = CRGB::Green;
        lower_leds[lower_segment.getSize() - 1] = CRGB::Green;

        // Swap red and green colors from 5 * 14 to the end
        int start_idx = 5 * 14;
        for (int i = start_idx; i < upper_segment.getSize(); ++i)
        {
            uint8_t temp = upper_leds[i].r;
            upper_leds[i].r = upper_leds[i].b;
            upper_leds[i].b = temp;

            temp = upper_leds[i].r;
            upper_leds[i].r = upper_leds[i].g;
            upper_leds[i].g = temp;
        }

        FastLED.show();
    }
};

#endif // OVNI_H
