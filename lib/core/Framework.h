// BaseFramework.h
#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <Project.h>

template<size_t NUM_LEDS>
class Framework {
private:
    Project<NUM_LEDS>& project;  // Reference to Project

public:
    Framework(Project<NUM_LEDS>& proj) : project(proj) {
        // Constructor initializes the reference to Project
    }

    void initialize() {
        // Initialization code
    }

    void tick() {
        project.tick();
    }
};

#endif // FRAMEWORK_H