#ifndef SINES_H
#define SINES_H

#include "Effect.h"
#include "Fill.h"
#include "Utils.h"

class Sines : public Effect {
public:
    Sines(ISegments& segments) : Effect(segments) {}
    float step;

    float minLength = 1.0f;
    float maxLength = 5.0f;
    float minStepVelocity = 1.0f;
    float maxStepVelocity = 2.5f;

    bool paint() override {
        Serial.println("Sines::paint()");
        Serial.print("gyroData.pitch: ");
        Serial.println(gyroData.pitch);
        step += mapf(gyroData.pitch, (float)-M_PI/2, (float)M_PI/2, minStepVelocity, maxStepVelocity);
        int length = mapf(gyroData.roll, (float)-M_PI/2, (float)M_PI/2, minLength, maxLength);
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