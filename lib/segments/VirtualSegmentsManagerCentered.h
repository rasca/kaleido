#ifndef VIRTUAL_SEGMENTS_MANAGER_CENTERED_H
#define VIRTUAL_SEGMENTS_MANAGER_CENTERED_H

#include "VirtualSegments.h"
#include "VirtualSegmentsManager.h"
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
class VirtualSegmentsManagerCentered: public VirtualSegmentsManager<NUM_LEDS> {
private:
    std::vector<CenteredSegment> segments;  // Store center and
    // radius of each segment and in the center is single or not

public:
    VirtualSegmentsManagerCentered(std::array<CRGB, NUM_LEDS>& leds, size_t start = 0) : VirtualSegmentsManager<NUM_LEDS>(leds, start)
    {
        segments.emplace_back(new CenteredSegment(start, 0, true)); // Start with one segment of radius 0 position 1
        updateVirtualSegments();
    }

    VirtualSegmentsManagerCentered(VirtualSegments<1886U>& virtualSegmentsOrigin) : VirtualSegmentsManager<NUM_LEDS>(virtualSegmentsOrigin)
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

    void handleSerialCommand(char command) override
    {
        auto lastSegment = segments[segments.size() - 1];
        switch (command) {
            case 'e':
            {
                size_t currentSegment = 0; // Declare the variable currentSegment
                segments[this->currentSegment].radius++;
                updateVirtualSegments();
            }
            break;
            case 'q':
                if (
                    (segments[this->currentSegment].is_single && segments[this->currentSegment].radius > 0)
                    || segments[this->currentSegment].radius > 1)
                    {
                        segments[this->currentSegment].radius--;
                        updateVirtualSegments();
                    }
                break;
            case 'a':
                if (segments[this->currentSegment].center > this->start + 1 &&
                    (!(this->currentSegment > 1) || segments[this->currentSegment].center > segments[this->currentSegment - 1].center + 1))
                {
                    if (segments[this->currentSegment].is_single) {
                        segments[this->currentSegment].is_single = false;

                    } else {
                        segments[this->currentSegment].is_single = true;
                        segments[this->currentSegment].center--;
                        if (segments[this->currentSegment].radius == 0) {
                            segments[this->currentSegment].radius++;
                        }
                    }
                    updateVirtualSegments();
                }
                break;
            case 'd':
                if (!(this->currentSegment + 1 < segments.size()) || segments[this->currentSegment].center < segments[this->currentSegment+1].center - 1) {
                    if (segments[this->currentSegment].is_single) {
                        segments[this->currentSegment].is_single = false;
                        segments[this->currentSegment].center++;
                        if (segments[this->currentSegment].radius == 0) {
                            segments[this->currentSegment].radius++;
                        }
                    } else {
                        segments[this->currentSegment].is_single = true;
                    }
                    updateVirtualSegments();
                }
                break;
            case '+':
            case '=':
                segments.emplace_back(lastSegment.center + lastSegment.radius * 2, lastSegment.radius, lastSegment.is_single);
                this->currentSegment = segments.size() - 1;
                updateVirtualSegments();
                break;
            case 'w':
                if (this->currentSegment > 0) {
                    this->currentSegment--;
                    updateVirtualSegments();
                }
                break;
            case 's':
                if (this->currentSegment < segments.size() - 1) {
                    this->currentSegment++;
                    updateVirtualSegments();
                }
                break;
            case 'p':
                print();
                break;
        }
    }

    void print() override
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
        Serial.println(this->currentSegment);
    }

    void updateVirtualSegments() override
    {
        print();
        this->virtualSegments.segments.clear();
        size_t amount = 0;
        for (auto& segment : segments) {
            this->virtualSegments.addSegment(segment.getStart(), segment.getLength());
        }
    }

    void tick() override
   {
        FastLED.clear();
        CRGB colors[] = {CRGB::Green, CRGB::Blue};
        int colorIndex = 0;

        for (int i=0; i< segments.size(); i++) {
            CRGB color = (i == this->currentSegment) ? CRGB::Red : colors[colorIndex];

            auto segment = segments[i];
            auto vSegment = this->virtualSegments.segments[i];
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

#endif // VIRTUAL_SEGMENTS_MANAGER_CENTERED
