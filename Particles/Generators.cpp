#include "Generators.h"

extern size_t PARTICLE_BUFFER_SIZE;

typedef void(*particle_generator)(std::vector<Particle>&, size_t, size_t);
void _particle_generator_uniform(std::vector<Particle>& particles, size_t offset, size_t amount)
{
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<float> positionDistribution(-1.0f, 1.0f);
    std::uniform_real_distribution<float> colorDistribution(0.0f, 1.0f);
    std::uniform_real_distribution<float> velocityDistribution(-1.0f, 1.0f);
    std::uniform_real_distribution<float> lifeTimeDistribution(0.0f, 10.0f);

    for (size_t i = 0; i < amount; i++)
    {
        particles[offset + i] = Particle(
            glm::vec3(positionDistribution(generator), positionDistribution(generator), 0.0f),
            glm::vec3(velocityDistribution(generator), velocityDistribution(generator), 0.0f),
            glm::vec3(0.0f),

            glm::vec4(1.0f, 1.0f, 0.0f, 0.5f),

            lifeTimeDistribution(generator)
        );
    }
}
void _particle_generator_circle(std::vector<Particle>& particles, size_t offset, size_t amount)
{
    float r;
    float theta;

    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<float> positionDistribution(-1.0f, 1.0f);
    std::uniform_real_distribution<float> angleDistribution(0.0f, 2 * (float)CL_M_PI);
    std::uniform_real_distribution<float> colorDistribution(0.0f, 1.0f);
    std::uniform_real_distribution<float> velocityDistribution(-0.1f, 0.1f);
    //std::uniform_real_distribution<float> accelerationDistribution(0.0f, -0.1f);
    std::uniform_real_distribution<float> lifeTimeDistribution(0.0f, 10.0f);

    for (int i = 0; i < amount; i++)
    {
        r = sqrt(positionDistribution(generator));
        theta = positionDistribution(generator) * 2 * (float)CL_M_PI;

        particles[offset + i] = Particle(
            glm::vec3(r * cos(theta) * 0.3f, r * sin(theta) * 0.3f, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.0f),

            glm::vec4(glm::vec3(colorDistribution(generator)), 1.0f),

            lifeTimeDistribution(generator)
        );
    }
}
void _particle_generator_cone(std::vector<Particle>& particles, size_t offset, size_t amount)
{
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<float> positionDistribution(0.0f, 1.0f);
    std::uniform_real_distribution<float> angleDistribution(0.0f, 2 * (float)CL_M_PI);
    std::uniform_real_distribution<float> colorDistribution(0.0f, 1.0f);
    std::uniform_real_distribution<float> velocityDistribution(5.0f, 50.0f);
    //std::uniform_real_distribution<float> accelerationDistribution(0.0f, -0.1f);
    std::uniform_real_distribution<float> lifeTimeDistribution(0.0f, 60.0f);

    glm::vec2 velocity;

    for (int i = 0; i < amount; i++)
    {
        float angle = (90.0f - 0.0f) * positionDistribution(generator) + 0.0f;
        angle = angle * (float)CL_M_PI / 180.0f;
        velocity = glm::vec2(cos(angle), sin(angle));

        particles[offset + i] = Particle(
            glm::vec3(2.0f, 2.0f, 0.0f), 
            glm::vec3(velocityDistribution(generator), velocityDistribution(generator), 0.0f),
            glm::vec3(0.0f),

            glm::vec4(colorDistribution(generator), 0.0f, colorDistribution(generator), 0.5f),

            lifeTimeDistribution(generator)
        );
    }
}

void generate_particles_st(std::vector<Particle>& particles, size_t& particleCount, size_t amount, particle_generator generator)
{
    if (particleCount + amount > PARTICLE_BUFFER_SIZE) amount = PARTICLE_BUFFER_SIZE - particleCount;

    generator(particles, particleCount, amount);
    particleCount += amount;
}
void generate_particles_mt(std::vector<Particle>& particles, size_t& particleCount, size_t amount, particle_generator generator)
{
    if (particleCount + amount > PARTICLE_BUFFER_SIZE) amount = PARTICLE_BUFFER_SIZE - particleCount;

    constexpr auto THREAD_COUNT = 8;
    size_t particlesPerThread = amount / THREAD_COUNT;
    size_t particleRemainder = amount % THREAD_COUNT;

    std::vector<std::thread> threads;

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        size_t offset = i * particlesPerThread + particleCount;
        threads.emplace_back([&, offset]() { generator(particles, offset, particlesPerThread); });
    }

    if (particleRemainder)
    {
        size_t offset = THREAD_COUNT * particlesPerThread;
        generator(particles, offset, particleRemainder);
    }

    for (auto& thread : threads)
        thread.join();

    particleCount += amount;
}
void remove_dead_particles(std::vector<Particle>& particles, size_t& particleCount)
{
    Particle* pPtr;
    size_t removedParticles = 0;

    for (size_t i = particleCount; i > 0; i--)
    {
        pPtr = &particles[i - 1];

        if (pPtr->energy > 0) continue;

        *pPtr = particles[particleCount - removedParticles - 1];
        particleCount--;
    }
}
