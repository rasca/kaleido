// BaseKaleido.h
#ifndef BASE_KALEIDO_H
#define BASE_KALEIDO_H

#include <FastLED.h>
#include "Fill.h"
#include "Project.h"
#include "PhysicalSegments.h"
#include "VirtualSegments.h"

template<size_t NUM_LEDS>
class BaseKaleido : public Project<NUM_LEDS> {
protected:
    VirtualSegments<NUM_LEDS> lines_all;
    VirtualSegments<NUM_LEDS> lines;
    VirtualSegments<NUM_LEDS> dots;
    VirtualSegments<NUM_LEDS> facets;
    FillEffect fillEffect;

public:
    BaseKaleido() : lines_all(this->physicalSegments.leds),
                    lines(this->physicalSegments.leds),
                    dots(this->physicalSegments.leds),
                    facets(this->physicalSegments.leds),
                    fillEffect(dots, CRGB::Green)

    {
        // Constructor initializes FillEffect with lines_all and CRGB::Red
    }

    void initialize() {
        // Add any additional initialization logic here
    }

    void tick() override {
        // Add any additional initialization logic here
        FastLED.clear();
        FillPainter::paint(dots.getSegments()[0], CRGB::Green);
        FillPainter::paint(lines_all.getSegments()[0], CRGB::Red);
        FillPainter::paint(facets.getSegments()[0], CRGB::Yellow);
        FastLED.show();
    }
};

class BigKaleido : public BaseKaleido<1886U> {
public:
    BigKaleido() {
        // Add physical segments
        // dots
        auto segment = physicalSegments.addSegment(0, 6 * 50);  // dots 6 x 50led strings
        FastLED.addLeds<WS2812, 26>(segment.getLEDs(), segment.getSize());

        // facets
        segment = physicalSegments.addSegment(300, 1 * 50);  // 1 x 50led string
        FastLED.addLeds<WS2812, 14>(segment.getLEDs(), segment.getSize());

        // lines
        segment = physicalSegments.addSegment(350, 2 * 192);  // 1 x 50led string
        FastLED.addLeds<WS2812, 12, GRB>(segment.getLEDs(), segment.getSize());
        segment = physicalSegments.addSegment(734, 6 * 192);  // 1 x 50led string
        FastLED.addLeds<WS2812, 25, GRB>(segment.getLEDs(), segment.getSize());

        // Add virtual segments
        dots.addSegment(0, 6 * 50); // all the dots
        facets.addSegment(300, 1 * 50); // all the facets
        lines_all.addSegment(350, 8 * 192); // all the lines
    }
};

class SmallKaleido : public BaseKaleido<534U> {
public:
    SmallKaleido() {
        auto segment = physicalSegments.addSegment(0, 2 * 50);  // dots 2 x 50led strings
        FastLED.addLeds<WS2812, 12>(segment.getLEDs(), segment.getSize());

        // facets
        segment = physicalSegments.addSegment(100, 1 * 50);  // 1 x 50led string
        FastLED.addLeds<WS2812, 14>(segment.getLEDs(), segment.getSize());

        // lines
        segment = physicalSegments.addSegment(150, 2 * 192);  // 1 x 50led string
        FastLED.addLeds<WS2812, 26>(segment.getLEDs(), segment.getSize());

        // Add virtual segments
        dots.addSegment(0, 2 * 50); // all the dots
        facets.addSegment(100, 1 * 50); // all the facets
        lines_all.addSegment(150, 2 * 192); // all the lines
    }
};

#endif // KALEIDO_H