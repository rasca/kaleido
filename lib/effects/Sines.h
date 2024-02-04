#ifndef SINES_H
#define SINES_H

#include "Effect.h"
#include "Fill.h"
#include "Utils.h"

class Sines : public Effect {
public:
    Sines(ISegments& segments) : Effect(segments) {}

    long step;
    bool paint() override {
        step++;
        auto& segmentsVector = segments.getSegments();

        for (int i = 0; i < segmentsVector.size(); i++) {

            auto& segment = segmentsVector[i];
            uint8_t brightness = map(sin(mapf(((step + i) % 255), 0, 255, 0, 2 * M_PI)), -1, 1, 0, 255);
            FillPainter::paint(segment, CHSV(map(gyroData.yaw, (float)-M_PI, (float)M_PI, 0, 255), 255, brightness));

            // for (int i = 0; i < segment.getSize(); ++i) {
            //     segment.getLEDs()[i] = CRGB(
            //         127 + 127 * sin(i + millis() / 1000.0),
            //         127 + 127 * sin(i + millis() / 1000.0 + 2 * PI / 3),
            //         127 + 127 * sin(i + millis() / 1000.0 + 4 * PI / 3)
            //     );
            // }
        }
        return false;
    }
};




#endif // SINES_H