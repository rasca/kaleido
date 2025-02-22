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

    // Mode selection
    const bool SCREAM_MODE = true;  // Set to true for scream contest, false for VU meter

    // Audio processing variables
    const int SAMPLE_WINDOW = 50;            // Sample window in ms
    const int MIC_PIN = 15;                  // Analog pin for microphone
    float peakToPeak = 0;                    // Peak-to-peak level
    float level = 0;                         // Normalized level (0-1)
    float autoGain = 1.0;                    // Auto gain multiplier
    const float GAIN_ADJUST_RATE = 0.01;     // How quickly the gain adjusts
    const float TARGET_LEVEL = 0.6;          // Target level (0.0 to 1.0)
    const int VU_METER_LENGTH = 60 * 5 - 31; // 5m x 60led/m strip minus 20 leds cut

    // Scream contest variables
    const float REF_VOLTAGE = 3.3;           // Reference voltage for ADC
    const float VREF_DB = 0.00631;           // Reference voltage for 94dB SPL calibration
    float currentDb = 0;                     // Current dB level
    float peakDb = 0;                        // Peak dB level
    bool isScreaming = false;                // Whether currently screaming
    const float SCREAM_THRESHOLD_DB = 85.0;  // Minimum dB to count as screaming
    const float MAX_DB = 100.0;              // Maximum dB to measure (for safety)

    // Calibration variables
    const int CALIBRATION_TIME = 2000;  // 2 seconds of calibration
    float ambientDb = 0;                // Calibrated ambient noise level
    bool isCalibrated = false;          // Whether calibration is complete
    const float CALIBRATION_SAMPLES = 20; // Number of samples to average

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
        
        // Run calibration
        calibrate();
    }

    void calibrate() {
        Serial.println("Starting calibration...");
        FastLED.clear();
        
        // Visual indicator that we're calibrating
        auto &vu_segment = vu_meter.segments[0];
        auto leds = vu_segment.getLEDs();
        
        // Show blue moving dot during calibration
        int calibrationDot = 0;
        float totalDb = 0;
        int validSamples = 0;
        
        unsigned long startTime = millis();
        while (millis() - startTime < CALIBRATION_TIME) {
            // Move blue dot back and forth
            FastLED.clear();
            leds[calibrationDot] = CRGB::Blue;
            FastLED.show();
            calibrationDot = (calibrationDot + 1) % VU_METER_LENGTH;
            
            // Take measurements
            float measurement = measureDb();
            if (measurement > 0) {  // Valid measurement
                totalDb += measurement;
                validSamples++;
            }
            
            delay(CALIBRATION_TIME / (VU_METER_LENGTH * 2));  // Complete 2 passes during calibration
        }
        
        // Calculate ambient level
        if (validSamples > 0) {
            ambientDb = totalDb / validSamples;
        } else {
            ambientDb = 60;  // Fallback value if no valid samples
        }
        
        // Visual confirmation of calibration complete
        FastLED.clear();
        for (int i = 0; i < VU_METER_LENGTH; i++) {
            leds[i] = CRGB::Green;
        }
        FastLED.show();
        delay(500);
        FastLED.clear();
        FastLED.show();
        
        isCalibrated = true;
        Serial.print("Calibration complete. Ambient dB: ");
        Serial.println(ambientDb);
    }

    float measureDb() {
        unsigned long startMillis = millis();
        float maxAmplitude = 0;
        float sumSquares = 0;
        int sampleCount = 0;
        
        // Collect samples for SAMPLE_WINDOW ms
        while (millis() - startMillis < SAMPLE_WINDOW) {
            float sample = analogRead(MIC_PIN);
            sample = (sample - 2048.0) / 2048.0; // Normalize to -1 to 1
            sumSquares += sample * sample;
            sampleCount++;
            
            float amplitude = abs(sample);
            if (amplitude > maxAmplitude) {
                maxAmplitude = amplitude;
            }
        }
        
        // Calculate RMS value
        float rms = sqrt(sumSquares / sampleCount);
        
        // Convert to dB (simplified for auto-gain mic)
        return 60 + (rms * 60); // Maps 0-1 to 60-MAX_DB range
    }

    float readLevel()
    {
        if (SCREAM_MODE) {
            if (!isCalibrated) {
                calibrate();
                return 0;
            }
            
            currentDb = measureDb();
            
            // Map level to 0-1 range only for screaming range
            // Use ambient level + 3dB as the minimum threshold
            float minDb = ambientDb + 3;
            if (currentDb < minDb) {
                level = 0;
            } else {
                level = constrain(map(currentDb, minDb, MAX_DB, 0, 100) / 100.0f, 0.0f, 1.0f);
            }
            
            // Update scream state
            if (currentDb > SCREAM_THRESHOLD_DB) {
                isScreaming = true;
                if (currentDb > peakDb) {
                    peakDb = currentDb;
                }
            } else {
                isScreaming = false;
            }
            
            return level;
            
        } else {
            unsigned long startMillis = millis();
            float signalMax = 0;
            float signalMin = 4096;

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

            // Simple normalization, no auto-gain needed
            float volume = peakToPeak / 4096.0;
            volume = constrain(volume, 0.0f, 1.0f);

            // Light smoothing
            level = level * 0.5f + volume * 0.5f;

            return level;
        }
    }

    CRGB getColorForLevel(float level) {
        if (SCREAM_MODE) {
            if (level == 0) {
                return CRGB::Black;
            } else {
                // Use HSV for smooth transition from orange to red
                // Orange is around hue 32, Red is hue 0
                // As level increases, hue decreases from 32 to 0
                uint8_t hue = 32 - (level * 32);
                uint8_t sat = 255;  // Full saturation
                uint8_t val = 255;  // Full brightness
                return CHSV(hue, sat, val);
            }
        } else {
            return CRGB::Red;  // Original VU meter color
        }
    }

    void updateDisplay() {
        auto &vu_segment = vu_meter.segments[0];
        auto leds = vu_segment.getLEDs();
        
        // Clear previous frame
        FastLED.clear();
        
        // Calculate number of LEDs to light up based on level
        int ledsToLight = (int)(level * VU_METER_LENGTH);
        CRGB color = getColorForLevel(level);
        
        // Calculate how many LEDs to light on each side
        int ledsPerSide = ledsToLight / 2;
        
        // Light LEDs from both sides towards the center
        for (int i = 0; i < ledsPerSide; i++) {
            // Calculate color with position-based hue variation for more dynamic effect
            float positionLevel = (float)i / ledsPerSide;  // 0 to 1 based on position
            float adjustedLevel = level * (1.0 - (positionLevel * 0.2));  // Slightly vary color based on position
            CRGB posColor = getColorForLevel(adjustedLevel);
            
            // Left side - starting from 0 going right
            leds[i] = posColor;
            if (SCREAM_MODE && isScreaming) {
                uint8_t brightness = 192 + (sin8(millis() / 20) >> 2); // Range 192-255 for subtler pulse
                leds[i].nscale8(brightness);
            }
            
            // Right side - starting from the end going left
            leds[VU_METER_LENGTH - 1 - i] = posColor;
            if (SCREAM_MODE && isScreaming) {
                uint8_t brightness = 192 + (sin8(millis() / 20) >> 2);
                leds[VU_METER_LENGTH - 1 - i].nscale8(brightness);
            }
        }
        
        // If we have an odd number of LEDs to light, add one more in the center
        if (VU_METER_LENGTH % 2 != 0 && level >= 0.999f) {
            leds[VU_METER_LENGTH / 2] = color;
            if (SCREAM_MODE && isScreaming) {
                uint8_t brightness = 192 + (sin8(millis() / 20) >> 2);
                leds[VU_METER_LENGTH / 2].nscale8(brightness);
            }
        }
        
        FastLED.show();
    }

    void tick() override
    {
        level = readLevel();

        if (millis() - lastPaint > paintInterval)
        {
            lastPaint = millis();
            updateDisplay();
        }

        // Debug output
        if (SCREAM_MODE) {
            Serial.print("dB: ");
            Serial.print(currentDb, 1);  // Show one decimal place
            Serial.print("\tPeak dB: ");
            Serial.print(peakDb, 1);
            Serial.print("\tLevel: ");
            Serial.print(level, 3);
            Serial.print("\tScreaming: ");
            Serial.println(isScreaming);
        } else {
            Serial.print("Level: ");
            Serial.print(level, 3);
            Serial.print("\tPeak-to-Peak: ");
            Serial.println(peakToPeak);
        }
    }

private:
    long long lastPaint = 0;
    int paintInterval = 40; // millis
};

#endif // IRATORIO_H
