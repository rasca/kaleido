#ifndef OUTWARDS_H
#define OUTWARDS_H

using namespace std;
#include <vector>
#include <tuple>
#include "Painter.h"
#include "Effect.h"
#include "ISegment.h"
#include "Fade.h"
#include <FastLED.h>


class OutwardsPainter : public Painter {
public:
    static bool paint(Segment& segment, CRGB color, long step, bool fill, bool reverse) {
        // paints the segment from the center at the given step
        long size = segment.getSize();

        int sidesEnded = 0;
        if (size / 2 - step > 0)
        {
            segment.getLEDs()[size / 2 - step] = color;
        } else {
            sidesEnded++;
        }

        if (size / 2 + step < size)
        {
            segment.getLEDs()[size / 2 + step] = color;
        } else {
            sidesEnded++;
        }

        return sidesEnded == 2;
    }
};

class Outwards : public Effect {

public:
    long step = 0;
    CRGB color;
    Outwards(ISegments& segments, CRGB col=CRGB::Blue) : Effect(segments), color(col) {}

    std::vector<tuple<size_t, long>> rays;  // segment index, step

    void paint() override {
        for (auto& segment : segments.getSegments()) {
            FadePainter::paint(segment, 80);
        }
        if (step % 5 == 0)
        {
            // get a random segment
            int segmentIndex = random(segments.getSegments().size());
            rays.push_back(make_tuple(segmentIndex, 0));
        }
        auto& segmentsVector = segments.getSegments();
        size_t i;
        for (i = 0; i < rays.size(); i++) {
            bool ended = OutwardsPainter::paint(segmentsVector[get<0>(rays[i])], color, get<1>(rays[i]), false, false);
            if (ended)
            {
                rays.erase(rays.begin() + i);
                i--;
            } else {
                get<1>(rays[i])++;
            }
        }
        step++;
    }
};

#endif // OUTWARDS_H
