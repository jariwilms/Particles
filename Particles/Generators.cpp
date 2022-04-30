#include "Generators.h"

#define RAND_F(min, max) (min + std::rand() / (RAND_MAX / (max - min)))

extern size_t PARTICLE_BUFFER_SIZE;

void _particle_generator_uniform(std::vector<Particle>& particles, size_t offset, size_t amount, GeneratorSettings settings)
{
    std::srand(time((time_t*)nullptr));

    for (size_t i = 0; i < amount; i++)
    {
        float px = RAND_F(settings.position_min.x, settings.position_max.x);
        float py = RAND_F(settings.position_min.y, settings.position_max.y);
        float pz = RAND_F(settings.position_min.z, settings.position_max.z);

        float vx = RAND_F(settings.velocity_min.x, settings.velocity_max.x);
        float vy = RAND_F(settings.velocity_min.y, settings.velocity_max.y);
        float vz = RAND_F(settings.velocity_min.z, settings.velocity_max.z);

        float cr = RAND_F(settings.color_min.r, settings.color_max.r);
        float cg = RAND_F(settings.color_min.g, settings.color_max.g);
        float cb = RAND_F(settings.color_min.b, settings.color_max.b);
        float ca = RAND_F(settings.color_min.a, settings.color_max.a);

        float en = RAND_F(settings.energy_min, settings.energy_max);

        particles[offset + i] = Particle(glm::vec3(px, py, pz), glm::vec3(vx, vy, vz), glm::vec3(0.0f), glm::vec4(cr, cg, cb, ca), en);
    }
}
void _particle_generator_circle(std::vector<Particle>& particles, size_t offset, size_t amount, GeneratorSettings settings)
{
    std::srand(time((time_t*)nullptr));

    for (int i = 0; i < amount; i++)
    {
        float rx = RAND_F(settings.position_min.x, settings.position_max.x);
        float ry = RAND_F(settings.position_min.y, settings.position_max.y);
        float rz = RAND_F(settings.position_min.z, settings.position_max.z);

        float tx = RAND_F(settings.position_min.x, settings.position_max.x) * 2 * float(CL_M_PI);
        float ty = RAND_F(settings.position_min.y, settings.position_max.y) * 2 * float(CL_M_PI);
        float tz = RAND_F(settings.position_min.z, settings.position_max.z) * 2 * float(CL_M_PI);

        float px = rx * cos(tx);
        float py = ry * sin(ty);
        float pz = rz * cos(tz);

        float vx = RAND_F(settings.velocity_min.x, settings.velocity_max.x);
        float vy = RAND_F(settings.velocity_min.y, settings.velocity_max.y);
        float vz = RAND_F(settings.velocity_min.z, settings.velocity_max.z);

        float cr = RAND_F(settings.color_min.r, settings.color_max.r);
        float cg = RAND_F(settings.color_min.g, settings.color_max.g);
        float cb = RAND_F(settings.color_min.b, settings.color_max.b);
        float ca = RAND_F(settings.color_min.a, settings.color_max.a);

        float en = RAND_F(settings.energy_min, settings.energy_max);

        particles[offset + i] = Particle(glm::vec3(px, py, pz), glm::vec3(vx, vy, vz), glm::vec3(0.0f), glm::vec4(cr, cg, cb, ca), en);
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

//void generate_particles_st(std::vector<Particle>& particles, size_t& particleCount, size_t amount, ParticleGenerator generator)
//{
//    if (particleCount + amount > PARTICLE_BUFFER_SIZE) amount = PARTICLE_BUFFER_SIZE - particleCount;
//
//    generator(particles, particleCount, amount);
//    particleCount += amount;
//}
//void generate_particles_mt(std::vector<Particle>& particles, size_t& particleCount, size_t amount, ParticleGenerator generator)
//{
//    if (particleCount + amount > PARTICLE_BUFFER_SIZE) amount = PARTICLE_BUFFER_SIZE - particleCount;
//
//    constexpr auto THREAD_COUNT = 8;
//    size_t particlesPerThread = amount / THREAD_COUNT;
//    size_t particleRemainder = amount % THREAD_COUNT;
//
//    std::vector<std::thread> threads;
//
//    for (int i = 0; i < THREAD_COUNT; i++)
//    {
//        size_t offset = i * particlesPerThread + particleCount;
//        threads.emplace_back([&, offset]() { generator(particles, offset, particlesPerThread); });
//    }
//
//    if (particleRemainder)
//    {
//        size_t offset = THREAD_COUNT * particlesPerThread;
//        generator(particles, offset, particleRemainder);
//    }
//
//    for (auto& thread : threads)
//        thread.join();
//
//    particleCount += amount;
//}
