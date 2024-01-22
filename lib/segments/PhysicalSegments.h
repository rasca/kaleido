#ifndef PHYSICAL_SEGMENTS_MANAGER_H
#define PHYSICAL_SEGMENTS_MANAGER_H

#include "ISegments.h"
#include <FastLED.h>
#include <vector>
#include <array>
#include "Segment.h"

template<size_t NUM_LEDS>
class PhysicalSegments : public ISegments {
public:
    std::array<CRGB, NUM_LEDS> leds;  // Main LED array

    Segment& addSegment(size_t start, size_t size) {
        segments.emplace_back(&(leds)[0], start, size);
        return segments.back();
    }

    void initialize() override {
        size_t totalSize = 0;
        for (const auto& segment : segments) {
            totalSize += segment.getSize();
        }

        if (totalSize != NUM_LEDS) {
            // Handle the error: total size of segments does not match NUM_LEDS
            return;
        }
    }
};

#endif // PHYSICAL_SEGMENTS_MANAGER_H