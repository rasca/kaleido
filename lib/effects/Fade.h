// Fill.h
#ifndef FADE_H
#define FADE_H

#include "Painter.h"
#include "Effect.h"
#include "ISegment.h"
#include <FastLED.h>

class FadePainter : public Painter {
public:
    static void paint(Segment& segment, uint8_t fadeBy) {
        fadeToBlackBy(segment.getLEDs(), segment.getSize(), fadeBy);
    }
};

class Fade : public Effect {

private:
    uint8_t fadeBy;

public:
    Fade(ISegments& segments, uint8_t fadeBy) : Effect(segments), fadeBy(fadeBy) {}

    void paint() override {
        auto& segmentsVector = segments.getSegments();
        for (auto& segment : segmentsVector) {
            FadePainter::paint(segment, fadeBy);
        }
    }
};

#endif // FADE_H