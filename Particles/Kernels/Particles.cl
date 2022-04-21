const int PARTICLE_STRUCT_SIZE = 10;

const int POSITION_OFFSET = 0;
const int VELOCITY_OFFSET = POSITION_OFFSET + 7;

typedef struct __attribute__ ((packed)) Particle
{
	float x;
	float y;
	float z;

	float r;
	float g;
	float b;
	float a;

	float xv;
	float yv;
	float zv;
} Particle;



__kernel void generate_particles_uniform(__global Particle* particles, int particlesPerUnit, uint amount, uint2 random)
{
	int index = get_global_id(0);

	uint seed = random.x + index;
	uint t = seed ^ (seed << 11);
	uint result = random.y ^ (random.y >> 19) ^ (t ^ (t >> 8));

	for (int i = 0; i < particlesPerUnit; i++)
	{
		
	}
}


__kernel void calculate_position(__global Particle* particles, int particlesPerUnit, float deltaTime, float speedMultiplier)
{
	int index = get_global_id(0) * particlesPerUnit;

	for (int i = 0; i < particlesPerUnit; i++)
	{
		particles[index + i].x += particles[index + i].xv * deltaTime * speedMultiplier;
		particles[index + i].y += particles[index + i].yv * deltaTime * speedMultiplier;
	}
}

__kernel void calculate_color_over_time(__global Particle* particles, int particlesPerUnit, float deltaTime)
{
	int index = get_global_id(0) * particlesPerUnit;
	deltaTime /= 1000.0f;

	for (int i = 0; i < particlesPerUnit; i++)
	{
		particles[index + i].r = particles[index + i].r + 1.0f * deltaTime;
		particles[index + i].g = particles[index + i].g + 1.0f * deltaTime;
		particles[index + i].b = particles[index + i].b + 1.0f * deltaTime;

		if (particles[index + i].r > 1.0f) particles[index + i].r -= 1.0f;
		if (particles[index + i].g > 1.0f) particles[index + i].g -= 1.0f;
		if (particles[index + i].b > 1.0f) particles[index + i].b -= 1.0f;
	}
}