// BaseKaleido.h
#ifndef BASE_KALEIDO_H
#define BASE_KALEIDO_H

#include <Arduino.h>
#include <FastLED.h>
#include "Framework.h"
#include "EspNow.h"
#include "Gyro.h"
#include "Fill.h"
#include "Start.h"
#include "Stars.h"
#include "Outwards.h"
#include "Project.h"
#include "PhysicalSegments.h"
#include "VirtualSegments.h"
#include "Utils.h"


template<size_t NUM_LEDS>
class BaseKaleido : public Project<NUM_LEDS> {

public:

    EspNow<GyroData> espNow;
    VirtualSegments<NUM_LEDS> lines_all;
    VirtualSegments<NUM_LEDS> lines;
    VirtualSegments<NUM_LEDS> dots;
    VirtualSegments<NUM_LEDS> dots_lines;
    VirtualSegments<NUM_LEDS> facets;
    FillEffect fillEffect;
    Start start;
    Outwards outwards;
    Stars stars;

    BaseKaleido() : lines_all(this->physicalSegments.leds),
                    lines(this->physicalSegments.leds),
                    dots(this->physicalSegments.leds),
                    dots_lines(this->physicalSegments.leds),
                    facets(this->physicalSegments.leds),
                    fillEffect(dots, CRGB::Green),
                    start(facets, dots, lines_all, lines),
                    outwards(lines),
                    stars(dots)

    {
        // Constructor initializes FillEffect with lines_all and CRGB::Red
    }

    void initialize(Framework<NUM_LEDS>& framework) {
        espNow.setup(OnDataRecv, OnDataSent);
        FastLED.clear();
    }

    long long lastPaint = 0;
    int paintInterval = 40; // millis

    void tick() override {

        if (millis() - lastPaint > paintInterval) {
            paint();
        }
    }

    void paint() {
        // Add any additional initialization logic here
        // FastLED.clear();
        lastPaint = millis();
        FastLED.show();
        stars.paint();
        // outwards.paint();
    }

    static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
    {
    }
    static void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
        memcpy(&gyroData, incomingData, sizeof(gyroData));

    }
};

class BigKaleido : public BaseKaleido<1886U> {
public:
    BigKaleido() {
        // Add physical segments
        // dots
        auto segment = physicalSegments.addSegment(0, 6 * 50);  // dots 6 x 50led strings
        FastLED.addLeds<WS2812, 26>(&(segment.getLEDs()[0]), segment.getSize());

        // facets
        segment = physicalSegments.addSegment(300, 1 * 50);  // 1 x 50led string
        FastLED.addLeds<WS2812, 14>(&(segment.getLEDs()[0]), segment.getSize());

        // lines
        segment = physicalSegments.addSegment(350, 2 * 192);  // 1 x 50led string
        FastLED.addLeds<WS2812, 12, GRB>(&(segment.getLEDs()[0]), segment.getSize());
        segment = physicalSegments.addSegment(734, 2 * 192);  // 1 x 50led string
        FastLED.addLeds<WS2812, 25, GRB>(&(segment.getLEDs()[0]), segment.getSize());
        segment = physicalSegments.addSegment(1118, 2 * 192);  // 1 x 50led string
        FastLED.addLeds<WS2812, 32, GRB>(&(segment.getLEDs()[0]), segment.getSize());
        segment = physicalSegments.addSegment(1502, 2 * 192);  // 1 x 50led string
        FastLED.addLeds<WS2812, 33, GRB>(&(segment.getLEDs()[0]), segment.getSize());

        // Add virtual segments
        dots.addSegment(0, 6 * 50); // all the dots
        facets.addSegment(300, 1 * 50); // all the facets
        lines_all.addSegment(350, 8 * 192); // all the lines
        lines.start = 350;
        // auto segments = {11, 13, 11, 14, 12, 19, 17, 22, 23, 21, 23};
        // std::vector<size_t> segments = {12, 5, 15, 9, 17, 10, 22, 15, 22, 17, 30, 16, 19, 32, 25, 26, 33, 28, 33, 26, 54, 21, 56, 30, 42, 55, 39, 55, 46, 55, 35, 57, 58, 70, 70, 70, 70, 70, 70};
        std::vector<std::vector<size_t>> segments = {{356, 3}, {363, 4}, {372, 7}, {384, 7}, {395, 10}, {408, 12}, {425, 11}, {441, 14}, {460, 15}, {481, 15}, {502, 16}, {524, 20}, {541, 19}, {566, 22}, {591, 23}, {619, 24}, {646, 25}, {677, 26}, {707, 27}, {736, 29}, {770, 31}, {812, 33}, {849, 33}, {892, 35}, {927, 37}, {974, 37}, {1019, 39}, {1068, 41}, {1117, 42}, {1166, 44}, {1216, 44}, {1265, 46}, {1316, 47}, {1376, 49}, {1435, 49}, {1502, 50}, {1557, 52}, {1624, 54}, {1694, 53}, {1755, 56}, {1817, 57}};
        lines.addSegments(segments);

        std::vector<size_t> dots_segments = {5, 8, 9, 12, 17, 16, 16, 19, 19, 33, 20, 14, 16, 15, 18, 22};
        dots_lines.addSegments(dots_segments);


    }
};

class SmallKaleido : public BaseKaleido<534U> {
public:
    SmallKaleido() {
        auto segment = physicalSegments.addSegment(0, 2 * 50);  // dots 2 x 50led strings
        FastLED.addLeds<WS2812, 12>(&(segment.getLEDs())[0], segment.getSize());

        // facets
        segment = physicalSegments.addSegment(100, 1 * 50);  // 1 x 50led string
        FastLED.addLeds<WS2812, 14>(&(segment.getLEDs())[0], segment.getSize());

        // lines
        segment = physicalSegments.addSegment(150, 2 * 192);  // 1 x 50led string
        FastLED.addLeds<WS2812, 26>(&(segment.getLEDs())[0], segment.getSize());

        // Add virtual segments
        dots.addSegment(0, 2 * 50); // all the dots
        facets.addSegment(100, 1 * 50); // all the facets
        lines_all.addSegment(150, 2 * 192); // all the lines
    }
};

#endif // KALEIDO_H