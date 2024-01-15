#ifndef PROJECT_H
#define PROJECT_H

#include "PhysicalSegments.h"

template<size_t NUM_LEDS>
class Project {
protected:
    PhysicalSegments<NUM_LEDS> physicalSegments;  // Pointer to PhysicalSegmentManager

public:
    Project() {
    }

    void initialize() {
        // Add any additional initialization logic here
    }

    virtual void tick() {
        // Add any additional initialization logic here
        Serial.println("project tick");
    }

    // Add methods as needed
};

#endif // PROJECT_HH