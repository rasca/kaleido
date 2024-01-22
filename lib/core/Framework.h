// BaseFramework.h
#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <Project.h>
#include <VirtualSegments.h>
#include <VirtualSegmentsManager.h>
#include <VirtualSegmentsManagerLength.h>
#include <VirtualSegmentsManagerCentered.h>

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

    void setupVirtualSegmentsLength(std::array<CRGB, NUM_LEDS>& leds) {
        virtualSegmentsManager = new VirtualSegmentsManagerLength<NUM_LEDS>(leds, 350);
    }

    void setupVirtualSegmentsCentered(std::array<CRGB, NUM_LEDS>& leds) {
        virtualSegmentsManager = new VirtualSegmentsManagerCentered<NUM_LEDS>(leds, 350);
    }

    void setupVirtualSegmentsLength(VirtualSegments<NUM_LEDS>& virtualSegments) {
        virtualSegmentsManager = new VirtualSegmentsManagerLength<NUM_LEDS>(virtualSegments);
    }

    void setupVirtualSegmentsCentered(VirtualSegments<NUM_LEDS>& virtualSegments) {
        virtualSegmentsManager = new VirtualSegmentsManagerCentered<NUM_LEDS>(virtualSegments);
    }
};

#endif // FRAMEWORK_H