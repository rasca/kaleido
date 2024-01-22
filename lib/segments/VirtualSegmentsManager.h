#ifndef VIRTUAL_SEGMENTS_MANAGER_H
#define VIRTUAL_SEGMENTS_MANAGER_H

#include "VirtualSegments.h"
#include <vector>

class CenteredSegment {
public:
    size_t center;
    size_t radius;
    bool is_single;

    CenteredSegment(size_t center, size_t radius, bool is_single) : center(center), radius(radius), is_single(is_single) {}

    size_t getStart() {
        return center - radius;
    }

    size_t getLength() {
        return radius * 2 + (is_single ? 1 : 0);
    }
};

template<size_t NUM_LEDS>
class VirtualSegmentsManager {
private:
    std::array<CRGB, NUM_LEDS>& leds;
    std::vector<CenteredSegment> segments;  // Store center and
    // radius of each segment and in the center is single or not

    VirtualSegments<NUM_LEDS> virtualSegments;
    int currentSegment;
    size_t start;

public:
    VirtualSegmentsManager(std::array<CRGB, NUM_LEDS>& leds, size_t start = 0) : leds(leds), currentSegment(0), virtualSegments(leds), start(start) {
        segments.emplace_back(new CenteredSegment(start, 0, true)); // Start with one segment of radius 0 position 1
        updateVirtualSegments();
    }

    VirtualSegmentsManager(VirtualSegments<1886U>& virtualSegmentsOrigin) : currentSegment(0), leds(virtualSegmentsOrigin.leds), virtualSegments(virtualSegmentsOrigin.leds), start(virtualSegmentsOrigin.start)
    {
        for (auto &segment : virtualSegmentsOrigin.getSegments())
        {
            auto radius = segment.getSize()  / 2;
            auto center = segment.getStart() + radius;

            bool is_single = segment.getSize() % 2 == 1;  // save if the center is one led or two

            // we need to calculate the center and it's radius
            segments.emplace_back(center, radius, is_single);
        }
        updateVirtualSegments();
    }

    void handleSerialCommand(char command) {
        auto lastSegment = segments[segments.size() - 1];
        switch (command) {
            case 'e':
                segments[currentSegment].radius++;
                updateVirtualSegments();
                break;
            case 'q':
                if (
                    (segments[currentSegment].is_single && segments[currentSegment].radius > 0)
                    || segments[currentSegment].radius > 1)
                    {
                        segments[currentSegment].radius--;
                        updateVirtualSegments();
                    }
                break;
            case 'a':
                if (segments[currentSegment].center > start + 1 &&
                    (!(currentSegment > 1) || segments[currentSegment].center > segments[currentSegment - 1].center + 1))
                {
                    if (segments[currentSegment].is_single) {
                        segments[currentSegment].is_single = false;

                    } else {
                        segments[currentSegment].is_single = true;
                        segments[currentSegment].center--;
                        if (segments[currentSegment].radius == 0) {
                            segments[currentSegment].radius++;
                        }
                    }
                    updateVirtualSegments();
                }
                break;
            case 'd':
                if (!(currentSegment + 1 < segments.size()) || segments[currentSegment].center < segments[currentSegment+1].center - 1) {
                    if (segments[currentSegment].is_single) {
                        segments[currentSegment].is_single = false;
                        segments[currentSegment].center++;
                        if (segments[currentSegment].radius == 0) {
                            segments[currentSegment].radius++;
                        }
                    } else {
                        segments[currentSegment].is_single = true;
                    }
                    updateVirtualSegments();
                }
                break;
            case '+':
            case '=':
                segments.emplace_back(lastSegment.center + lastSegment.radius * 2, lastSegment.radius, lastSegment.is_single);
                currentSegment = segments.size() - 1;
                updateVirtualSegments();
                break;
            case 'w':
                if (currentSegment > 0) {
                    --currentSegment;
                    updateVirtualSegments();
                }
                break;
            case 's':
                if (currentSegment < segments.size() - 1) {
                    ++currentSegment;
                    updateVirtualSegments();
                }
                break;
            case 'p':
                print();
                break;
        }
    }

    void print()
    {
        Serial.println("Printing current state");
        Serial.println("Segment starts and lengths: ");
        for (auto &segment : segments)
        {
            Serial.print("{");
            Serial.print(segment.getStart());
            Serial.print(", ");
            Serial.print(segment.getLength());
            Serial.print("}, ");
        }
        Serial.println("");
        Serial.println("Segments: ");
        for (auto &segment : segments)
        {
            Serial.print("{");
            Serial.print(segment.center);
            Serial.print(", ");
            Serial.print(segment.radius);
            Serial.print(", ");
            Serial.print(segment.is_single);
            Serial.print("}, ");
        }
        Serial.println();
        Serial.print("Current segment: ");
        Serial.println(currentSegment);
    }
    void updateVirtualSegments() {
        print();
        virtualSegments.segments.clear();
        size_t amount = 0;
        for (auto& segment : segments) {
            virtualSegments.addSegment(segment.getStart(), segment.getLength());
        }
    }

    void tick() {
        FastLED.clear();
        CRGB colors[] = {CRGB::Green, CRGB::Blue};
        int colorIndex = 0;

        for (int i=0; i< segments.size(); i++) {
            CRGB color = (i == currentSegment) ? CRGB::Red : colors[colorIndex];

            auto segment = segments[i];
            auto vSegment = virtualSegments.segments[i];
            for (int j = 0; j < vSegment.getSize(); ++j) {
                vSegment.getLEDs()[j] = color;
            }
            vSegment.getLEDs()[segment.radius] = CRGB::White;
            if (!segment.is_single) {
                vSegment.getLEDs()[segment.radius-1] = CRGB::White;
            }
            colorIndex = (colorIndex + 1) % 2;  // Cycle through the colors
        }
        FastLED.show();
    }
};

#endif // VIRTUAL_SEGMENTS_MANAGER