// BaseFramework.h
#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <Project.h>
#include <VirtualSegments.h>
#include <VirtualSegmentsManager.h>

template<size_t NUM_LEDS>
class Framework {
private:
    Project<NUM_LEDS>& project;  // Reference to Project
    VirtualSegmentsManager<NUM_LEDS>* virtualSegmentsManager;

public:
    Framework(Project<NUM_LEDS>& proj) : project(proj) {
        // Constructor initializes the reference to Project
    }

    void initialize() {
        // Initialization code
    }

    void tick() {
        if (virtualSegmentsManager) {
            virtualSegmentsManager->handleSerialCommand(Serial.read());
            virtualSegmentsManager->tick();
            return;
        }
        project.tick();
    }

    void setupVirtualSegments(std::array<CRGB, NUM_LEDS>& leds) {
        virtualSegmentsManager = new VirtualSegmentsManager<NUM_LEDS>(leds, 350);
    }

    void setupVirtualSegments(VirtualSegments<NUM_LEDS>& virtualSegments) {
        virtualSegmentsManager = new VirtualSegmentsManager<NUM_LEDS>(virtualSegments);
    }
};

#endif // FRAMEWORK_H