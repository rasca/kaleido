#ifndef START_H
#define START_H

#include "Painter.h"
#include "Effect.h"
#include "ISegment.h"
#include <FastLED.h>
#include "Hope.h"
#include "Gyro.h"
#include "Fade.h"

class Start : public Effect {

public:
    Hope facets, dots, lines, lines_all;
    Fade facets_fade, dots_fade, lines_fade, lines_all_fade;

    long step = 0;

    Start(ISegments &facets,
          ISegments &dots,
          ISegments &lines_all,
          ISegments &lines)
        : Effect(segments),
          facets(facets),
          dots(dots),
          lines(lines),
          lines_all(lines_all),
          facets_fade(facets, 10),
          dots_fade(dots, 10),
          lines_fade(lines, 10),
          lines_all_fade(lines_all, 10)
    {
    }

    const long START_DOTS = 200;
    const long START_LINES = 450;
    const long ONLY_LINES = 1000;

    bool paint() override
    {
        // facets.start(
        //     new Hope(facets)
        // ).runfor(2200)
        // .at(2000) {
        //     new Fade(facets)
        // }
        // onend([] {

        // });

        step++;
        if (step < ONLY_LINES)
        {
            facets.paint();
            if (step >= START_DOTS)
            {
                dots.paint();
            }
            if (step >= START_LINES)
            {
                lines_all.paint();
            }
        }
        if (step >= ONLY_LINES)
        {
            facets_fade.paint();
            dots_fade.paint();
            lines.paint();
        }
        return false;
    }
};

#endif // START_H
