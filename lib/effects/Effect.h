// Effect.h
#ifndef EFFECT_H
#define EFFECT_H

#include "ISegments.h"
#include "Painter.h"

class Effect {
protected:
    ISegments& segments;

public:
    Effect(ISegments& segments) : segments(segments) {
        // Constructor initializes the references to ISegment and Painter
    }

    virtual void paint() {
    }
};

#endif // EFFECT_H