#ifndef RUNNING_H
#define RUNNING_H

#include "Effect.h"
#include "Utils.h"
#include "Gyro.h"


class RunningPainter : public Painter {
public:
    static void paint(Segment& segment, int hue, long step, uint8_t x_scale, bool reverse=true)
    {
        for (int i = 0; i < segment.getSize(); ++i) {
            uint16_t a = i*x_scale - step;
            // if (saw) {
            //     a &= 0xFF;
            //     if (a < 16)
            //     {
            //         a = 192 + a * 8;
            //     }
            //     else
            //     {
            //         a = mapi((int)a, 16, 255, 64, 192);
            //     }
            //     a = 255 - a;
            // }
            // uint8_t s = dual ? sin_gap(a) : sin8(a);
            uint8_t s = sin8(a);
            CRGB color = CHSV(hue, 255, s);
            // uint32_t ca = color_blend(SEGCOLOR(1), SEGMENT.color_from_palette(i, true, PALETTE_SOLID_WRAP, 0), s);

            // if (dual)
            // {
            //     uint16_t b = (SEGLEN - 1 - i) * x_scale - step;
            //     uint8_t t = sin_gap(b);
            //     uint32_t cb = color_blend(SEGCOLOR(1), SEGMENT.color_from_palette(i, true, PALETTE_SOLID_WRAP, 2), t);
            //     ca = color_blend(ca, cb, 127);
            // }

            const int8_t j = reverse ? segment.getSize() - 1 - i : i;
            segment.getLEDs()[j] = color;  // Set each LED to white
        }
    }
};

class Running : public Effect {
public:
    Running(ISegments& segments) : Effect(segments) {}
    float step;

    bool opposite = true;

    bool paint() override {
        step += 10;
        // step += mapf(gyroData.pitch, (float)-M_PI/2, (float)M_PI/2, minStepVelocity, maxStepVelocity);
        // int length = mapf(gyroData.roll, (float)-M_PI/2, (float)M_PI/2, minLength, maxLength);
        auto& segmentsVector = segments.getSegments();

        for (int i = 0; i < segmentsVector.size(); i++) {

            auto& segment = segmentsVector[i];
            // uint8_t speed = map(sin(mapf(((int)(step + i * length) % 255), 0, 255, 0, 2 * M_PI)), -1, 1, 0, 255);
            // RunningPainter::paint(segment, CHSV(map(gyroData.yaw, (float)-M_PI, (float)M_PI, 0, 255), 255, brightness));
            RunningPainter::paint(
                segment,
                map(gyroData.yaw, (float)-M_PI, (float)M_PI, 0, 255),
                step,
                map(gyroData.pitch, (float)-M_PI/2, (float)M_PI/2, 5, 20),
                opposite ? false : i % 2 == 0
            );
        }
        return false;
    }
};

#endif // RUNNING_H