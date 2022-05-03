#include "Generators.hpp"

void _particle_generator_uniform(Particle* particles, size_t offset, size_t amount, GeneratorSettings settings)
{
    SEED_GENERATOR;

    float px, py, pz;
    float vx, vy, vz;
    float cr, cg, cb, ca;
    float en;

    for (size_t i = 0; i < amount; i++)
    {
        px = RAND_F(settings.position_min.x, settings.position_max.x);
        py = RAND_F(settings.position_min.y, settings.position_max.y);
        pz = RAND_F(settings.position_min.z, settings.position_max.z);

        vx = RAND_F(settings.velocity_min.x, settings.velocity_max.x);
        vy = RAND_F(settings.velocity_min.y, settings.velocity_max.y);
        vz = RAND_F(settings.velocity_min.z, settings.velocity_max.z);

        cr = RAND_F(settings.color_min.r, settings.color_max.r);
        cg = RAND_F(settings.color_min.g, settings.color_max.g);
        cb = RAND_F(settings.color_min.b, settings.color_max.b);
        ca = RAND_F(settings.color_min.a, settings.color_max.a);

        en = RAND_F(settings.energy_min, settings.energy_max);

        particles[offset + i] = Particle(glm::vec3(px, py, pz), glm::vec3(vx, vy, vz), glm::vec4(cr, cg, cb, ca), en);
    }
}
void _particle_generator_circle(Particle* particles, size_t offset, size_t amount, GeneratorSettings settings)
{
    SEED_GENERATOR;

    float rx, ry, rz;
    float tx, ty, tz;

    float px, py, pz;
    float vx, vy, vz;
    float cr, cg, cb, ca;
    float en;

    for (int i = 0; i < amount; i++)
    {
        rx = RAND_F(settings.position_min.x, settings.position_max.x);
        ry = RAND_F(settings.position_min.y, settings.position_max.y);
        rz = RAND_F(settings.position_min.z, settings.position_max.z);

        tx = RAND_F(settings.position_min.x, settings.position_max.x) * 2 * float(CL_M_PI);
        ty = RAND_F(settings.position_min.y, settings.position_max.y) * 2 * float(CL_M_PI);
        tz = RAND_F(settings.position_min.z, settings.position_max.z) * 2 * float(CL_M_PI);

        px = rx * cos(tx);
        py = ry * sin(ty);
        pz = rz * cos(tz);

        vx = RAND_F(settings.velocity_min.x, settings.velocity_max.x);
        vy = RAND_F(settings.velocity_min.y, settings.velocity_max.y);
        vz = RAND_F(settings.velocity_min.z, settings.velocity_max.z);

        cr = RAND_F(settings.color_min.r, settings.color_max.r);
        cg = RAND_F(settings.color_min.g, settings.color_max.g);
        cb = RAND_F(settings.color_min.b, settings.color_max.b);
        ca = RAND_F(settings.color_min.a, settings.color_max.a);

        en = RAND_F(settings.energy_min, settings.energy_max);

        particles[offset + i] = Particle(glm::vec3(px, py, pz), glm::vec3(vx, vy, vz), glm::vec4(cr, cg, cb, ca), en);
    }
}
void _particle_generator_cone(Particle* particles, size_t offset, size_t amount, GeneratorSettings settings)
{
    SEED_GENERATOR;

    float an;

    float px, py, pz;
    float vx, vy, vz;
    float cr, cg, cb, ca;
    float en;

    for (int i = 0; i < amount; i++)
    {
        an = (settings.angle_max - settings.angle_min) * RAND_F(0.0f, 1.0f) + settings.angle_min * (float)CL_M_PI / 180.0f;

        px = RAND_F(settings.position_min.x, settings.position_max.x);
        py = RAND_F(settings.position_min.y, settings.position_max.y);
        pz = RAND_F(settings.position_min.z, settings.position_max.z);

        vx = cos(an);
        vy = sin(an);
        vz = 0.0f;

        cr = RAND_F(settings.color_min.r, settings.color_max.r);
        cg = RAND_F(settings.color_min.g, settings.color_max.g);
        cb = RAND_F(settings.color_min.b, settings.color_max.b);
        ca = RAND_F(settings.color_min.a, settings.color_max.a);

        en = RAND_F(settings.energy_min, settings.energy_max);

        particles[offset + i] = Particle(glm::vec3(px, py, pz), glm::vec3(vx, vy, vz), glm::vec4(cr, cg, cb, ca), en);
    }
}

void generate_particles_st(Particle* particles, size_t& particleCount, size_t amount, ParticleGenerator generator, GeneratorSettings settings)
{
    amount = particleCount + amount > PARTICLE_BUFFER_SIZE ? PARTICLE_BUFFER_SIZE - particleCount : amount;

    generator(particles, particleCount, amount, settings);
    particleCount += amount;
}
void generate_particles_mt(Particle* particles, size_t& particleCount, size_t amount, ParticleGenerator generator, GeneratorSettings settings)
{
    amount = particleCount + amount > PARTICLE_BUFFER_SIZE ? PARTICLE_BUFFER_SIZE - particleCount : amount;

    constexpr auto THREAD_COUNT = 8;
    size_t particlesPerThread = amount / THREAD_COUNT;
    size_t particleRemainder = amount % THREAD_COUNT;

    std::vector<std::thread> threads;

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        size_t offset = i * particlesPerThread + particleCount;
        threads.emplace_back([&, offset]() { generator(particles, offset, particlesPerThread, settings); });
    }

    if (particleRemainder)
    {
        size_t offset = THREAD_COUNT * particlesPerThread;
        generator(particles, offset, particleRemainder, settings);
    }

    for (auto& thread : threads)
        thread.join();

    particleCount += amount;
}
