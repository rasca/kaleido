#ifndef CHOREOGRAPHER_H
#define CHOREOGRAPHER_H

#include "Effect.h"
#include "Project.h"

// Effects
#include "Fill.h"
#include "Start.h"
#include "Stars.h"
#include "Outwards.h"
#include "Sines.h"

template<size_t NUM_LEDS>  // forward declaration
class BaseKaleido;

template <size_t NUM_LEDS>
class Choreographer
{
    BaseKaleido<NUM_LEDS>& kaleido;
    Effect* currentEffect;

    public:

    Choreographer(BaseKaleido<NUM_LEDS>& kaleido) : kaleido(kaleido), currentEffect(new Sines(kaleido.lines)) {}

    long effectStart = 0;
    long effectDuration = 10000;  // in millis

    void paint()
    {
        Serial.println("painting");
        if (millis() - effectStart >= effectDuration)
        {
            effectStart = millis();
            nextEffect();
        }
        currentEffect->paint();
    }

    void nextEffect()
    {
        Serial.println("nextEffect");
        delete currentEffect;

        int i = random(0, 5);
        switch (i)
        {
        case 0: {
            Sines *sinesSlow = new Sines(kaleido.lines);
            sinesSlow->minLength = 1.0f;
            sinesSlow->maxLength = 5.0f;
            sinesSlow->minStepVelocity = 1.0f;
            sinesSlow->maxStepVelocity = 2.5f;
            currentEffect = sinesSlow;
            break;
        }
        case 1: {
            Sines *sinesFast = new Sines(kaleido.lines);
            sinesFast->minLength = 5.0f;
            sinesFast->maxLength = 15.0f;
            sinesFast->minStepVelocity = 1.0f;
            sinesFast->maxStepVelocity = 2.5f;
            currentEffect = sinesFast;
            break;
        }
        case 2: {
            currentEffect = new Start(kaleido.facets, kaleido.dots, kaleido.lines_all, kaleido.lines);
            break;
        }
        case 3: {
            currentEffect = new Outwards(kaleido.lines);
            break;
        }
        case 4: {
            currentEffect = new Stars(kaleido.dots);
            break;
        }
        }
    }
};

#endif // CHOREOGRAPHER_H