#ifndef PROJECT_H
#define PROJECT_H

#include <PhysicalSegments.h>

// Forward declaration of Framework instead of #include <Framework.h> to avoid circular dependency
template<size_t NUM_LEDS>
class Framework;

template<size_t NUM_LEDS>
class Project {
public:
    PhysicalSegments<NUM_LEDS> physicalSegments;  // Pointer to PhysicalSegmentManager

    Project() {
    }

    virtual void initialize(Framework<NUM_LEDS>& framework) {
        // Add any additional initialization logic here
    }

    virtual void tick() {
        // Add any additional initialization logic here
        Serial.println("project tick");
    }

    // Add methods as needed
};

#endif // PROJECT_HH