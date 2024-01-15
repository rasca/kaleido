// Segment.h
#ifndef SEGMENT_H
#define SEGMENT_H

#include "ISegment.h"

class Segment : public ISegment {
private:
    CRGB* leds;
    size_t size;

public:
    Segment(CRGB* leds, size_t size) : leds(leds), size(size) {}

    CRGB* getLEDs() const override {
        return leds;
    }

    size_t getSize() const override {
        return size;
    }
};

#endif // SEGMENT_H
