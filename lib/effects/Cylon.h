#ifndef CYLON_H
#define CYLON_H

#include "Painter.h"
#include "Effect.h"
#include "ISegment.h"
#include <FastLED.h>

class Cylon : public Effect {

public:
    long step = 0;
    CRGB color;
    Cylon(ISegments& segments, CRGB col=CRGB::Blue) : Effect(segments), color(col) {}

    bool paint() override {

        auto& segmentsVector = segments.getSegments();
        size_t i;
        for (i = 0; i < segmentsVector.size(); i++) {
            if (i % 2 == 0)
                segmentsVector[i].getLEDs()[step % segmentsVector[i].getSize()] = color;
            else
                segmentsVector[i].getLEDs()[segmentsVector[i].getSize() - step % segmentsVector[i].getSize()] = color;
        }
        step++;
        return false;
    }
};

#endif // CYLON_H
