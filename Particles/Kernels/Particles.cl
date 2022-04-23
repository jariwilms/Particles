const int PARTICLE_STRUCT_SIZE = 10;

const int POSITION_OFFSET = 0;
const int VELOCITY_OFFSET = POSITION_OFFSET + 7;

typedef struct __attribute__ ((packed)) Particle
{
	float x;
	float y;

	float r;
	float g;
	float b;
	float a;

	float xv;
	float yv;

	float xa;
	float ya;

	float lt;
	float ttl;
} Particle;

typedef struct __attribute ((packed)) Attractor
{
	float x;
	float y;

	float xv;
	float yv;

	float gv;
} Attractor;



__kernel void calculate_movement(__global Particle* particles, int particleCount, int particlesPerUnit, float deltaTime, float speedMultiplier)
{
	size_t globalId = get_global_id(0);
	size_t index = globalId * particlesPerUnit;
	size_t final = ++globalId * particlesPerUnit - 1;

	if (final > particleCount) particlesPerUnit -= final - particleCount;

	for (int i = 0; i < particlesPerUnit; i++)
	{
		particles[index].xv += particles[index].xa * deltaTime * speedMultiplier;
		particles[index].yv += particles[index].ya * deltaTime * speedMultiplier;

		particles[index].x += particles[index].xv * deltaTime * speedMultiplier;
		particles[index].y += particles[index].yv * deltaTime * speedMultiplier;

		++index;
	}
}



__kernel void calculate_color_over_time(__global Particle* particles, int particleCount, int particlesPerUnit, float deltaTime, float speedMultiplier)
{
	size_t globalId = get_global_id(0);
	size_t index = globalId * particlesPerUnit;
	size_t final = ++globalId * particlesPerUnit - 1;

	if (final > particleCount) particlesPerUnit -= final - particleCount;

	for (int i = 0; i < particlesPerUnit; i++)
	{
		particles[index].r = particles[index].r + 1.0f * deltaTime;
		particles[index].g = particles[index].g + 1.0f * deltaTime;
		particles[index].b = particles[index].b + 1.0f * deltaTime;

		if (particles[index].r > 1.0f) particles[index].r -= 1.0f;
		if (particles[index].g > 1.0f) particles[index].g -= 1.0f;
		if (particles[index].b > 1.0f) particles[index].b -= 1.0f;

		++index;
	}
}



__kernel void calculate_gravity(__global Particle* particles, int particleCount, __global Attractor* attractors, int attractorCount, int particlesPerUnit, float deltaTime, float speedMultiplier)
{
	size_t globalId = get_global_id(0);
	size_t index = globalId * particlesPerUnit;
	size_t final = ++globalId * particlesPerUnit - 1;

	if (final > particleCount) particlesPerUnit -= final - particleCount;

	float dx;
	float dy;
	float ir;

	for (int i = 0; i < particlesPerUnit; i++)
	{
		dx = attractors[0].x - particles[index].x;
		dy = attractors[0].y - particles[index].y;

		ir = 1.0f / (dx * dx + dy * dy + 0.001f);
		ir = sqrt(ir);

		particles[index].xa += dx * ir * attractors[0].gv * deltaTime * speedMultiplier;
		particles[index].ya += dy * ir * attractors[0].gv * deltaTime * speedMultiplier;

		++index;
	}
}



__kernel void calculate_life_time(__global Particle* particles, int particleCount, int particlesPerUnit, float deltaTime, float speedMultiplier)
{
	size_t globalId = get_global_id(0);
	size_t index = globalId * particlesPerUnit;
	size_t final = ++globalId * particlesPerUnit - 1;

	if (final > particleCount) particlesPerUnit -= final - particleCount;

	for (int i = 0; i < particlesPerUnit; i++)
	{
		particles[index].lt += deltaTime;
		++index;
	}
}