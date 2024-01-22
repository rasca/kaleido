#ifndef ISEGMENT_H
#define ISEGMENT_H

#include <FastLED.h>

class ISegment {
public:
    virtual ~ISegment() {}
    virtual CRGB* getLEDs() const = 0;
    virtual size_t getSize() const = 0;
    virtual size_t getStart() const = 0;
};

#endif // ISEGMENT_H
