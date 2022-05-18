#include "Generators.hpp"

void _particle_generator_line(Particle* particles, size_t offset, size_t amount, GeneratorSettings settings)
{
    float px, py, pz;
    float vx, vy, vz;
    float cr, cg, cb, ca;
    float en;

    for (size_t i = 0; i < amount; i++)
    {
        px = RAND_F(settings.position_min.x, settings.position_max.x);
        py = 0.5f;//py = RAND_F(settings.position_min.y, settings.position_max.y);
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
void _particle_generator_cube(Particle* particles, size_t offset, size_t amount, GeneratorSettings settings)
{
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

        //cr = RAND_F(settings.color_min.r, settings.color_max.r);
        //cg = RAND_F(settings.color_min.g, settings.color_max.g);
        //cb = RAND_F(settings.color_min.b, settings.color_max.b);
        cr = (px + 1.0f) / 2.0f;
        cg = (py + 1.0f) / 2.0f;
        cb = (pz + 1.0f) / 2.0f;
        ca = RAND_F(settings.color_min.a, settings.color_max.a);

        en = RAND_F(settings.energy_min, settings.energy_max);

        particles[offset + i] = Particle(glm::vec3(px, py, pz), glm::vec3(vx, vy, vz), glm::vec4(cr, cg, cb, ca), en);
    }
}
void _particle_generator_sphere(Particle* particles, size_t offset, size_t amount, GeneratorSettings settings)
{
    float theta, phi;
    float rx;

    float px, py, pz;
    float vx, vy, vz;
    float cr, cg, cb, ca;
    float en;

    for (int i = 0; i < amount; i++)
    {
        theta = RAND_F(0.0f, 2 * (float)M_PI);
        phi = acosf(2 * RAND_F(0.0f, 1.0f) - 1);
        rx = pow(RAND_F(0.0f, 1.0f), 1.0f / 3.0f);

        px = rx * sinf(phi) * cosf(theta);
        py = rx * sinf(phi) * sinf(theta);
        pz = rx * cosf(phi);

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
    CHECK_BOUNDS;

    generator(particles, particleCount, amount, settings);
    particleCount += amount;
}
void generate_particles_mt(Particle* particles, size_t& particleCount, size_t amount, ParticleGenerator generator, GeneratorSettings settings)
{
    CHECK_BOUNDS;

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
