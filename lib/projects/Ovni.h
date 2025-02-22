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



template<size_t NUM_LEDS>
class Ovni : public Project<NUM_LEDS> {

public:

    VirtualSegments<NUM_LEDS> vu_meter;
    VirtualSegments<NUM_LEDS> ambient;

    // Audio processing variables
    const int SAMPLE_WINDOW = 50;  // Sample window in ms
    const int MIC_PIN = 12;        // Analog pin for microphone
    float peakToPeak = 0;          // Peak-to-peak level
    float volumeSmooth = 0;        // Smoothed volume level
    float autoGain = 1.0;          // Auto gain multiplier
    const float GAIN_ADJUST_RATE = 0.01;  // How quickly the gain adjusts
    const float TARGET_VOLUME = 0.6;      // Target volume level (0.0 to 1.0)

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
        analogReadResolution(12);  // Set analog read resolution to 12 bits for better precision
    }

    long long lastPaint = 0;
    int paintInterval = 40; // millis


    void tick() override {
        
        Serial.println(volume);

        if (millis() - lastPaint > paintInterval) {
            lastPaint = millis();

            // Clear previous frame
            FastLED.clear();

            FastLED.show();
        }
    }
};

#endif // OVNI_H
