
#ifndef VIRTUAL_SEGMENTS_MANAGER_LENGTH_H
#define VIRTUAL_SEGMENTS_MANAGER_LENGTH_H

#include "VirtualSegments.h"
#include "VirtualSegmentsManager.h"
#include <vector>

template<size_t NUM_LEDS>
class VirtualSegmentsManagerLength: public VirtualSegmentsManager<NUM_LEDS> {
private:
    std::vector<unsigned size_t> segments;

public:
    VirtualSegmentsManagerLength(std::array<CRGB, NUM_LEDS>& leds, size_t start = 0) : VirtualSegmentsManager<NUM_LEDS>(leds, start)
    {
        segments.emplace_back(1); // Start with one segment of size 1
        this->virtualSegments.addSegment(0, 1);
        updateVirtualSegments();
    }

    VirtualSegmentsManagerLength(VirtualSegments<1886U>& virtualSegmentsOrigin) : VirtualSegmentsManager<NUM_LEDS>(virtualSegmentsOrigin)
    {
        for (auto &segment : virtualSegmentsOrigin.getSegments())
        {
            segments.emplace_back(segment.getSize());
        }
        updateVirtualSegments();
    }

    void handleSerialCommand(char command) override
    {
        switch (command) {
            case '+':
            case '=':
                segments[this->currentSegment]++;
                updateVirtualSegments();
                break;
            case '-':
                if (segments[this->currentSegment] > 1) {
                    segments[this->currentSegment]--;
                    updateVirtualSegments();
                }
                break;
            case 'q':
                if (segments[this->currentSegment-1] > 1) {
                    segments[this->currentSegment-1]--;
                    segments[this->currentSegment]++;
                    updateVirtualSegments();
                }
                break;
            case 'w':
                if (segments[this->currentSegment] > 1) {
                    segments[this->currentSegment-1]++;
                    segments[this->currentSegment]--;
                    updateVirtualSegments();
                }
                break;
            case 'e':
                if (segments[this->currentSegment] > 1) {
                    segments[this->currentSegment+1]++;
                    segments[this->currentSegment]--;
                    updateVirtualSegments();
                }
                break;
            case 'r':
                if (segments[this->currentSegment+1] > 1) {
                    segments[this->currentSegment+1]--;
                    segments[this->currentSegment]++;
                    updateVirtualSegments();
                }
                break;
            case 'a':
                segments.emplace_back(1);
                this->currentSegment = segments.size() - 1;
                updateVirtualSegments();
                break;
            case 'p':
                if (this->currentSegment > 0) {
                    --this->currentSegment;
                }
                break;
            case 'n':
                if (this->currentSegment < segments.size() - 1) {
                    ++this->currentSegment;
                }
                break;
            case 's':
                print();
                break;
        }
    }

    void print() override
    {
        Serial.println("Printing current state");
        Serial.println("Segment lengths: ");
        for (auto &length : segments)
        {
            Serial.print(length);
            Serial.print(", ");
        }
        Serial.println();
        Serial.print("Current segment: ");
        Serial.println(this->currentSegment);
    }

    void updateVirtualSegments() override
    {
        print();
        this->virtualSegments.segments.clear();
        size_t amount = 0;
        for (auto& length : segments) {
            this->virtualSegments.addSegment(this->start + amount, length);
            amount += length;
        }
    }

    void tick() override
    {
        FastLED.clear();
        CRGB colors[] = {CRGB::Green, CRGB::Blue};
        int colorIndex = 0;

        for (int i=0; i< segments.size(); i++) {
            CRGB color = (i == this->currentSegment) ? CRGB::Red : colors[colorIndex];
            for (int j = 0; j < segments[i]; ++j) {
                this->virtualSegments.segments[i].getLEDs()[j] = color;
            }
            this->virtualSegments.segments[i].getLEDs()[segments[i] / 2 + 1] = CRGB::White;
            if (segments[i] % 2 == 0) {
                this->virtualSegments.segments[i].getLEDs()[segments[i] / 2] = CRGB::White;
            }
            colorIndex = (colorIndex + 1) % 2;  // Cycle through the colors
        }
        FastLED.show();
    }
};

#endif // VIRTUAL_SEGMENTS_MANAGER_LENGTH