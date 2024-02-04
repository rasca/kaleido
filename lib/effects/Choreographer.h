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

    std::vector<std::function<void(Choreographer*)>> effects;


    Choreographer(BaseKaleido<NUM_LEDS>& kaleido) : kaleido(kaleido), currentEffect(new Sines(kaleido.lines))
    {
        effects.push_back(&Choreographer::sinesSlow);
        effects.push_back(&Choreographer::sinesFast);
        effects.push_back(&Choreographer::start);
        effects.push_back(&Choreographer::outwards);
        effects.push_back(&Choreographer::outwardsFacets);
        effects.push_back(&Choreographer::stars);
        effects.push_back(&Choreographer::hopeFacets);
        effects.push_back(&Choreographer::hopeLines);
    }

    long effectStart = 0;
    long effectDuration = 30000;  // in millis

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
        FastLED.clear();
        delete currentEffect;

        int i = random(0, effects.size());
        (effects[i])(this);
    }

    void sinesSlow()
    {
        Sines *sinesSlow = new Sines(kaleido.lines);
        sinesSlow->minLength = 1.0f;
        sinesSlow->maxLength = 5.0f;
        sinesSlow->minStepVelocity = 1.0f;
        sinesSlow->maxStepVelocity = 2.5f;
        currentEffect = sinesSlow;
    }

    void sinesFast()
    {
        Sines *sinesFast = new Sines(kaleido.lines);
        sinesFast->minLength = 5.0f;
        sinesFast->maxLength = 15.0f;
        sinesFast->minStepVelocity = 1.0f;
        sinesFast->maxStepVelocity = 2.5f;
        currentEffect = sinesFast;
    }

    void start()
    {
        currentEffect = new Start(kaleido.facets, kaleido.dots, kaleido.lines_all, kaleido.lines);
    }

    void outwards()
    {
        currentEffect = new Outwards(kaleido.lines);
    }

    void outwardsFacets()
    {
        currentEffect = new Outwards(kaleido.facets);
    }

    void stars()
    {
        currentEffect = new Stars(kaleido.dots);
    }

    void hopeFacets()
    {
        currentEffect = new Hope(kaleido.facets);
    }

    void hopeLines()
    {
        currentEffect = new Hope(kaleido.lines);
    }
};

#endif // CHOREOGRAPHER_H