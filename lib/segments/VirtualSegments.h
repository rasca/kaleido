// VirtualSegments.h
#ifndef VIRTUAL_SEGMENTS_H
#define VIRTUAL_SEGMENTS_H

#include "ISegments.h"
#include <array>


template<size_t NUM_LEDS>
class VirtualSegments : public ISegments {
public:
    std::array<CRGB, NUM_LEDS>& leds;  // Reference to the LED array in PhysicalSegmentManager
    size_t start;
    size_t lastLED;

    VirtualSegments(std::array<CRGB, NUM_LEDS>& ledsArray, size_t start = 0) : leds(ledsArray), start(start) {
        // Constructor initializes the reference to the LED array
        lastLED = start;
    }

    Segment& addSegment(size_t start, size_t size) override {
        // Logic for adding a virtual segment
        // Note: `start` and `size` need to be interpreted in the context of virtual segments
        // Assuming segments refer to a larger, already existing LED array
        // Adjust as per your specific logic for virtual segments
        segments.emplace_back(&(leds)[0], start, size); // Adjust the constructor call as needed
        lastLED = start + size;
        return segments.back();
    }

    Segment& addNextSegment(size_t size) {
        segments.emplace_back(&leds[0], lastLED, size);
        lastLED += size;
        return segments.back();
    }

    void addSegments(std::vector<std::vector<size_t>> newSegments) {
        for (const auto& segment : newSegments) {
            addSegment(segment[0], segment[1]);
        }
    }

    void addSegments(std::vector<size_t> sizes) {
        for (const auto& size : sizes) {
            addNextSegment(size);
        }
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

#endif // VIRTUAL_SEGMENTS_H
