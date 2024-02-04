// Effect.h
#ifndef EFFECT_H
#define EFFECT_H

#include "ISegments.h"
#include "Painter.h"

class Effect {
protected:
    ISegments& segments;

public:
    Effect(ISegments &segments) : segments(segments) {
        // Constructor initializes the references to ISegment and Painter
    }

    virtual bool paint() {
        return false;
    }

    Effect &operator=(Effect &&other) noexcept
    {
        // Guard self assignment
        if (this == &other)
            return *this;

        // Copy the reference
        segments = other.segments;

        return *this;
    }
};


#endif // EFFECT_H