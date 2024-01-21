// ISegments.h
#ifndef ISEGMENTS_H
#define ISEGMENTS_H

#include <FastLED.h>
#include <vector>
#include "Segment.h"

class ISegments {
public:
    std::vector<Segment> segments;

    virtual ~ISegments() {}
    virtual Segment& addSegment(size_t start, size_t size) = 0;
    virtual void initialize() = 0;

    std::vector<Segment>& getSegments() {
        return segments;
    }

};

#endif // ISEGMENTS_H
