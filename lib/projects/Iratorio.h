// Iratorio.h
#ifndef BASE_IRATORIO_H
#define BASE_IRATORIO_H

#include <Arduino.h>
#include <FastLED.h>
#include "Framework.h"
#include "Project.h"
#include "PhysicalSegments.h"
#include "VirtualSegments.h"
#include "Utils.h"

template <size_t NUM_LEDS>
class Iratorio : public Project<NUM_LEDS>
{

public:
    VirtualSegments<NUM_LEDS> vu_meter;
    VirtualSegments<NUM_LEDS> ambient;

    // Audio processing variables
    const int SAMPLE_WINDOW = 50;            // Sample window in ms
    const int MIC_PIN = 15;                  // Analog pin for microphone
    float peakToPeak = 0;                    // Peak-to-peak level
    float volumeSmooth = 0;                  // Smoothed volume level
    float autoGain = 1.0;                    // Auto gain multiplier
    const float GAIN_ADJUST_RATE = 0.01;     // How quickly the gain adjusts
    const float TARGET_VOLUME = 0.6;         // Target volume level (0.0 to 1.0)
    const int VU_METER_LENGTH = 60 * 5 - 31; // 5m x 60led/m strip minus 20 leds cut

    Iratorio() : Project<NUM_LEDS>(),
                 vu_meter(this->physicalSegments.leds),
                 ambient(this->physicalSegments.leds)
    {
        // // Constructor initializes FillEffect with lines_all and CRGB::Red
        // ambient
        auto segment = this->physicalSegments.addSegment(0, 50 * 3); // 3 x 50led strings
        FastLED.addLeds<WS2812, 26>(&(segment.getLEDs()[0]), segment.getSize());
        segment = this->physicalSegments.addSegment(150, 50 * 3); // 3 x 50led strings
        FastLED.addLeds<WS2812, 27>(&(segment.getLEDs()[0]), segment.getSize());

        // vu meter
        segment = this->physicalSegments.addSegment(300, VU_METER_LENGTH); // 1 5m x 60led/m strip
        FastLED.addLeds<WS2812, 25, GRB>(&(segment.getLEDs()[0]), segment.getSize());

        ambient.addSegment(0, 2 * 50 * 3);         // all strings
        vu_meter.addSegment(300, VU_METER_LENGTH); // all the dots
    }

    void initialize(Framework<NUM_LEDS> &framework)
    {
        FastLED.clear();
        analogReadResolution(12); // Set analog read resolution to 12 bits for better precision
    }

    long long lastPaint = 0;
    int paintInterval = 40; // millis

    float readVolume()
    {
        unsigned long startMillis = millis();
        float signalMax = 0;
        float signalMin = 4096; // 12-bit ADC maximum

        // Collect samples for SAMPLE_WINDOW ms
        while (millis() - startMillis < SAMPLE_WINDOW)
        {
            float sample = analogRead(MIC_PIN);
            if (sample > signalMax)
                signalMax = sample;
            if (sample < signalMin)
                signalMin = sample;
        }

        peakToPeak = signalMax - signalMin;

        // Apply auto gain
        float volume = (peakToPeak * autoGain) / 4096.0; // Normalize to 0.0-1.0
        volume = constrain(volume, 0.0f, 1.0f);

        // Smooth the volume reading
        volumeSmooth = volumeSmooth * 0.7f + volume * 0.3f;

        // Adjust auto gain
        if (volumeSmooth < TARGET_VOLUME)
        {
            autoGain += GAIN_ADJUST_RATE;
        }
        else
        {
            autoGain -= GAIN_ADJUST_RATE;
        }
        autoGain = constrain(autoGain, 0.1f, 10.0f); // Limit gain range

        return volumeSmooth;
    }

    void tick() override
    {
        float volume = readVolume();

        Serial.println(volume);

        if (millis() - lastPaint > paintInterval)
        {
            lastPaint = millis();

            // Clear previous frame
            FastLED.clear();

            // Calculate number of LEDs to light up based on volume
            int ledsToLight = (int)(volume * VU_METER_LENGTH);
            auto &vu_segment = vu_meter.segments[0];
            auto leds = vu_segment.getLEDs();

            // Calculate how many LEDs to light on each side
            int ledsPerSide = ledsToLight / 2;

            // Light LEDs from both sides towards the center
            for (int i = 0; i < ledsPerSide; i++) {
                // Left side - starting from 0 going right
                leds[i] = CRGB::Red;
                
                // Right side - starting from the end going left
                leds[VU_METER_LENGTH - 1 - i] = CRGB::Red;
            }

            // If we have an odd number of LEDs to light, add one more in the center
            if (VU_METER_LENGTH % 2 != 0 && volume >= 0.999f) // Use >= 0.99 to account for floating point imprecision
            {
                leds[VU_METER_LENGTH / 2] = CRGB::Red;
            }
            Serial.print("volume == 1.0: ");
            Serial.print(volume == 1.0f);
            Serial.print("\tVU_METER_LENGTH % 2 && volume == 1.0: ");
            Serial.print((VU_METER_LENGTH % 2 != 0) && (volume == 1.0));
            Serial.print("\tvolume: ");
            Serial.print(volume);
            Serial.println();

            FastLED.show();
        }
    }
};

#endif // IRATORIO_H
