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
        step += mapf(gyroData.pitch, (float)-M_PI/2, (float)M_PI/2, 1.0f, 2.5f);
        // 2 good options
        // int length = mapf(gyroData.roll, (float)-M_PI/2, (float)M_PI/2, 1.0f, 5.0f);
        int length = mapf(gyroData.roll, (float)-M_PI/2, (float)M_PI/2, 5.0f, 15.0f);
        auto& segmentsVector = segments.getSegments();

        for (int i = 0; i < segmentsVector.size(); i++) {

            auto& segment = segmentsVector[i];
            uint8_t brightness = map(sin(mapf(((int)(step + i * length) % 255), 0, 255, 0, 2 * M_PI)), -1, 1, 0, 255);
            FillPainter::paint(segment, CHSV(map(gyroData.yaw, (float)-M_PI, (float)M_PI, 0, 255), 255, brightness));



            // for (int j = 0; j < segment.getSize(); ++j) {
            //     segment.getLEDs()[j] = CRGB(
            //         127 * sin(j / 5 + millis() / 1000.0),
            //         127 * sin(j / 5 + millis() / 1000.0 + 2 * PI / 3),
            //         127 * sin(j / 5 + millis() / 1000.0 + 4 * PI / 3)
            //     );
            // }
        }
        return false;
    }
};




#endif // SINES_H