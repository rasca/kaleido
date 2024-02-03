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

    std::vector<tuple<size_t, long, CRGB>> rays;  // segment index, step, color

    float rayRate = 10;  // every how many frames should we create a new ray
    // values less than 1 means multiple rays should be created each frame

    std::array<int, 50> raysCreated;
    void paint() override {
        Serial.println("");
        for (auto& segment : segments.getSegments()) {
            FadePainter::paint(segment, 80);
        }

        // map to -1, 1
        float mapped = mapf(gyroData.pitch, (float)-M_PI/2, (float)M_PI/2, -1.0f, 1.0f);
        rayRate = 4.0f * powf(10.0f, mapped);
        // Serial.print("mapped ");
        // Serial.print(mapped);
        // Serial.print("\t");
        // Serial.print("rayRate ");
        // Serial.print(rayRate);
        // Serial.print("\t");
        Serial.print(gyroData.yaw);
        Serial.print("\t");
        Serial.print(gyroData.pitch);
        Serial.print("\t");
        Serial.println(gyroData.roll);
        int newRays = calculateRaysToCreate();
        // Serial.print("New rays ");
        // Serial.print(newRays);
        // Serial.print("\t");
        for (int i=0; i < newRays; i++)
        {
            // get a random segment with a linear distribution because a uniform
            // distribution is seen as the first/small segments are more painted
            // than the others
            // from: https://stats.stackexchange.com/questions/171592/generate-random-numbers-with-linear-distribution
            float u = ((float)rand()) / RAND_MAX;
            int segmentIndex = random(segments.getSegments().size());

            float alpha = mapf(gyroData.pitch, (float)-M_PI/2, (float)M_PI/2, -4.0f/5.0f, 4.0f/5.0f);
            float x = (sqrt(pow(alpha, 2) - 2*alpha + 4*alpha*u + 1) - 1) / alpha;
            segmentIndex = map(x, -1.0f, 1.0f, 0, segments.getSegments().size() - 1);
            raysCreated[segmentIndex]++;

            CRGB color = CHSV(map(gyroData.yaw, (float)-M_PI, (float)M_PI, 0, 255), 255, 255);

            this->rays.push_back(make_tuple(segmentIndex, 0, color));
        }

        // print the number of rays created for each segment
        // for (int i=0; i < segments.getSegments().size(); i++)
        // {
        //     Serial.print(raysCreated[i]);
        //     Serial.print("\t");
        // }
        auto& segmentsVector = segments.getSegments();
        size_t i;
        for (i = 0; i < rays.size(); i++) {
            bool ended = OutwardsPainter::paint(segmentsVector[get<0>(rays[i])], get<2>(rays[i]), get<1>(rays[i]), false, false);
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

    int calculateRaysToCreate()
    {
        if (rayRate >= 1)
        {
            return random((int)rayRate + 1) == 1;
        }
        else
        {
            // For rates < 1, calculate the expected number of rays per frame
            // Serial.print(rayRate);
            float averageRaysPerFrame = 1 / rayRate;
            int raysToCreate = static_cast<int>(averageRaysPerFrame);
            // Add an additional ray based on the probability of the fractional part
            if (random((int)(1 / (averageRaysPerFrame - raysToCreate))) == 1)
            {
                raysToCreate += 1;
            }
            return raysToCreate;
        }
    }
};

#endif // OUTWARDS_H
