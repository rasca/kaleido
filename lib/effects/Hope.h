#ifndef HOPE_H
#define HOPE_H

#include "Painter.h"
#include "Effect.h"
#include "ISegment.h"
#include <FastLED.h>
#include "Cylon.h"
#include "Gyro.h"

class Hope : public Effect {

public:
    Cylon cylon;

    float hue = 0;
    long step = 0;

    Hope(ISegments& segments) : Effect(segments), cylon(segments) {}

    bool paint() override
    {
        // Gyro::print(gyroData);
        hue = map(gyroData.yaw, (float)-M_PI, (float)M_PI, 0, 255);
        step++;
        cylon.step = step + gyroData.accel_x / 1000;

        // Serial.print(gyroData.accel_x / 1000);
        // Serial.println("\t");
        if (hue > 360) hue -= 360;
        if (hue < 0) hue += 360;
        cylon.color = CHSV(hue, 255, 128);
        cylon.paint();
        FastLED.show();
        return false;
    }
};

#endif // HOPE_H