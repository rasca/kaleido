// Fill.h
#ifndef FILL_H
#define FILL_H

#include "Painter.h"
#include "Effect.h"
#include "ISegment.h"
#include <FastLED.h>

class FillPainter : public Painter {
public:
    static void paint(Segment& segment, CRGB color) {
        for (int i = 0; i < segment.getSize(); ++i) {
            segment.getLEDs()[i] = color;  // Set each LED to white
        }
    }
};

class FillEffect : public Effect {

private:
    CRGB color;

public:
    FillEffect(ISegments& segments, CRGB col) : Effect(segments), color(col) {}

    bool paint() override {
        auto& segmentsVector = segments.getSegments();
        for (auto& segment : segmentsVector) {
            FillPainter::paint(segment, color);
        }
        return false;
    }
};

#endif // FILL_H