#ifndef VIRTUAL_SEGMENTS_MANAGER_H
#define VIRTUAL_SEGMENTS_MANAGER_H

#include "VirtualSegments.h"
#include <vector>

template<size_t NUM_LEDS>
class VirtualSegmentsManager {
protected:
    std::array<CRGB, NUM_LEDS>& leds;
    VirtualSegments<NUM_LEDS> virtualSegments;
    int currentSegment;
    size_t start;

public:
    VirtualSegmentsManager(std::array<CRGB, NUM_LEDS>& leds, size_t start = 0) : leds(leds), currentSegment(0), virtualSegments(leds), start(start) {}

    VirtualSegmentsManager(VirtualSegments<1886U>& virtualSegmentsOrigin) : currentSegment(0), leds(virtualSegmentsOrigin.leds), virtualSegments(virtualSegmentsOrigin.leds), start(virtualSegmentsOrigin.start) {}

    virtual void handleSerialCommand(char command) {}

    virtual void print() {}

    virtual void updateVirtualSegments() {}

    virtual void tick() {}
};

#endif // VIRTUAL_SEGMENTS_MANAGER