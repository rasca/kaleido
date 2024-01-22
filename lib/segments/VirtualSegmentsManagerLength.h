
#ifndef VIRTUAL_SEGMENTS_MANAGER_H
#define VIRTUAL_SEGMENTS_MANAGER_H

#include "VirtualSegments.h"
#include <vector>

template<size_t NUM_LEDS>
class VirtualSegmentsManager {
private:
    std::array<CRGB, NUM_LEDS>& leds;
    std::vector<unsigned size_t> segments;
    VirtualSegments<NUM_LEDS> virtualSegments;
    int currentSegment;
    size_t start;

public:
    VirtualSegmentsManager(std::array<CRGB, NUM_LEDS>& leds, size_t start = 0) : leds(leds), currentSegment(0), virtualSegments(leds), start(start) {
        segments.emplace_back(1); // Start with one segment of size 1
        virtualSegments.addSegment(0, 1);
        updateVirtualSegments();
    }

    VirtualSegmentsManager(VirtualSegments<1886U>& virtualSegmentsOrigin) : currentSegment(0), leds(virtualSegmentsOrigin.leds), virtualSegments(virtualSegmentsOrigin.leds), start(virtualSegmentsOrigin.start)
    {
        for (auto &segment : virtualSegmentsOrigin.getSegments())
        {
            segments.emplace_back(segment.getSize());
        }
        updateVirtualSegments();
    }

    void handleSerialCommand(char command) {
        switch (command) {
            case '+':
            case '=':
                segments[currentSegment]++;
                updateVirtualSegments();
                break;
            case '-':
                if (segments[currentSegment] > 1) {
                    segments[currentSegment]--;
                    updateVirtualSegments();
                }
                break;
            case 'q':
                if (segments[currentSegment-1] > 1) {
                    segments[currentSegment-1]--;
                    segments[currentSegment]++;
                    updateVirtualSegments();
                }
                break;
            case 'w':
                if (segments[currentSegment] > 1) {
                    segments[currentSegment-1]++;
                    segments[currentSegment]--;
                    updateVirtualSegments();
                }
                break;
            case 'e':
                if (segments[currentSegment] > 1) {
                    segments[currentSegment+1]++;
                    segments[currentSegment]--;
                    updateVirtualSegments();
                }
                break;
            case 'r':
                if (segments[currentSegment+1] > 1) {
                    segments[currentSegment+1]--;
                    segments[currentSegment]++;
                    updateVirtualSegments();
                }
                break;
            case 'a':
                segments.emplace_back(1);
                currentSegment = segments.size() - 1;
                updateVirtualSegments();
                break;
            case 'p':
                if (currentSegment > 0) {
                    --currentSegment;
                }
                break;
            case 'n':
                if (currentSegment < segments.size() - 1) {
                    ++currentSegment;
                }
                break;
            case 's':
                print();
                break;
        }
    }

    void print()
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
        Serial.println(currentSegment);
    }
    void updateVirtualSegments() {
        print();
        virtualSegments.segments.clear();
        size_t amount = 0;
        for (auto& length : segments) {
            virtualSegments.addSegment(start + amount, length);
            amount += length;
        }
    }

    void tick() {
        FastLED.clear();
        CRGB colors[] = {CRGB::Green, CRGB::Blue};
        int colorIndex = 0;

        for (int i=0; i< segments.size(); i++) {
            CRGB color = (i == currentSegment) ? CRGB::Red : colors[colorIndex];
            for (int j = 0; j < segments[i]; ++j) {
                virtualSegments.segments[i].getLEDs()[j] = color;
            }
            virtualSegments.segments[i].getLEDs()[segments[i] / 2 + 1] = CRGB::White;
            if (segments[i] % 2 == 0) {
                virtualSegments.segments[i].getLEDs()[segments[i] / 2] = CRGB::White;
            }
            colorIndex = (colorIndex + 1) % 2;  // Cycle through the colors
        }
        FastLED.show();
    }
};

#endif // VIRTUAL_SEGMENTS_MANAGER