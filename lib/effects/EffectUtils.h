#ifndef EFFECT_UTILS_H
#define EFFECT_UTILS_H

#include <vector>
#include <Effect.h>


template <typename T>
class Emit : public Effect
{
    // Create particles and paint them every frame

public:

    Emit(ISegments& segments) : Effect(segments) {}

    // particleRate is the number of frames between creating a new particle
    // values less than 1 means multiple particles should be created each frame
    float particleRate = 10;

    std::vector<T> particles;  // segment index, step, color

    bool paint() override
    {
        createParticles();
        paintParticles();
        return false;
    }

    void paintParticles()
    {
        for (int i = 0; i < particles.size(); i++)
        {
            if (particles[i].paint(segments.getSegments()[particles[i].segmentIndex]))
            {
                particles.erase(particles.begin() + i);
                i--;
            }
        }
    }

    virtual void createParticles()
    {
        int newParticles = calculateParticlesToCreate();

        for (int i = 0; i < newParticles; i++)
        {
            size_t segmentIndex = random(segments.getSegments().size());
            T t(segmentIndex);
            particles.push_back(t);
        }
    }

    int calculateParticlesToCreate()
    {
        if (particleRate >= 1)
        {
            return random((int)particleRate + 1) == 1;
        }
        else
        {
            // For rates < 1, calculate the expected number of particles per frame
            // Serial.print(particleRate);
            float averageParticlesPerFrame = 1 / particleRate;
            int particlesToCreate = static_cast<int>(averageParticlesPerFrame);
            // Add an additional particle based on the probability of the fractional part
            if (random((int)(1 / (averageParticlesPerFrame - particlesToCreate))) == 1)
            {
                particlesToCreate += 1;
            }
            return particlesToCreate;
        }
    }

};

#endif // EFFECT_UTILS_H