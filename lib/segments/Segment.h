// Segment.h
#ifndef SEGMENT_H
#define SEGMENT_H

#include "ISegment.h"

class Segment : public ISegment {
private:
    CRGB* leds;
    size_t start;
    size_t size;

public:
    Segment(CRGB* leds, size_t start, size_t size) : leds(leds), start(start), size(size) {}

    CRGB* getLEDs() const override {
        return &(leds)[start];
    }

    size_t getSize() const override {
        return size;
    }

    size_t getStart() const override {
        return start;
    }
};

#endif // SEGMENT_H
