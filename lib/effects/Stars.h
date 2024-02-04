#ifndef STARS_H
#define STARS_H

using namespace std;
#include <vector>
#include <tuple>
#include "Painter.h"
#include "Effect.h"
#include "ISegments.h"
#include "ISegment.h"
#include "Fade.h"
#include <FastLED.h>
#include <EffectUtils.h>
#include <Utils.h>


class SegmentPainter : public Painter {
public:
    static bool paint(Segment& segment, CRGB color, size_t start, size_t size) {
        // paints the segment from the center at the given step

        for (size_t i = 0; i < size; i++)
        {
            if (start + i < segment.getSize())
            {
                segment.getLEDs()[start + i] = color;
            }
        }
        return false;
    }
};

class Star {
public:
    size_t segmentIndex;
    size_t start;
    size_t size;
    CRGB color;


    long life;

    Star(size_t segmentIndex, size_t start=0, size_t size=1, CRGB color=CHSV(128, 0, 125)) : segmentIndex(segmentIndex), start(), size(size), color(color)
    {
        life = random(1, 10);
    }

    bool paint(Segment& segment) {
        life--;
        if (life < 0)
        {
            SegmentPainter::paint(segment, CRGB::Black, start, size);
            return true;
        } else {
            SegmentPainter::paint(segment, getColor(), start, size);
            return false;
        }
    }

    CRGB getColor()
    {
        return color;
    }

};

class Stars : public Emit<Star> {

public:
    long step = 0;
    Stars(ISegments& segments) : Emit(segments) {}

    std::array<int, 50> raysCreated;


    void createParticles() override
    {

        // map to -1, 1
        float input_mapped = mapf(gyroData.pitch, (float)-M_PI/2, (float)M_PI/2, -1.0f, 1.0f);
        particleRate = 4.0f * powf(10.0f, input_mapped);

        int newParticles = calculateParticlesToCreate();

        for (int i = 0; i < newParticles; i++)
        {
            int segmentIndex = random(segments.getSegments().size());

            CRGB color = CHSV(map(gyroData.yaw, (float)-M_PI, (float)M_PI, 0, 255), 255, 125);

            this->particles.push_back(Star(segmentIndex));
            particles.back().start = random(0, segments.getSegments()[segmentIndex].getSize());
            particles.back().size = 1;
            particles.back().color = color;
        }
    }
};

#endif // STARS_H

