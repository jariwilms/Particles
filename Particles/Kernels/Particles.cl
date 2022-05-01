typedef struct Particle
{
	float px;	
	float py;
	float pz;

	float vx;	
	float vy;
	float vz;

	float cr;	
	float cg;
	float cb;
	float ca;

	float en;

	float pad1;
	float pad2;
	float pad3;
	float pad4;
	float pad5;
} Particle __attribute__ ((aligned(64)));

typedef struct Gravitor
{
	float px;
	float py;
	float pz;

	float vx;
	float vy;
	float vz;

	float cr;
	float cg;
	float cb;
	float ca;

	float gv;
} Gravitor;



__kernel void calculate_movement(__global Particle* particles, int particleCount, float deltaTime)
{
	int globalId = get_global_id(0);
	int stride = get_global_size(0);

	for (int i = globalId; i < particleCount; i += stride)
	{
		particles[i].px += particles[i].vx * deltaTime;
		particles[i].py += particles[i].vy * deltaTime;
	}
}

__kernel void calculate_gravity(__global Particle* particles, int particleCount, __global Gravitor* gravitors, int gravitorCount, float deltaTime)
{
	int globalId = get_global_id(0);
	int stride = get_global_size(0);

	float dx, dy;
	float inv, inv_r;
	float ax, ay;

	for (int i = globalId; i < particleCount; i += stride)
	{
		for (int j = 0; j < gravitorCount; ++j)
		{
			dx = gravitors[j].px - particles[i].px;
			dy = gravitors[j].py - particles[i].py;
	
			inv_r = rsqrt(dx * dx + dy * dy);
	
			particles[i].vx += gravitors[j].gv * inv_r * dx * deltaTime;
			particles[i].vy += gravitors[j].gv * inv_r * dy * deltaTime;
		}
	}

	//for (int i = 0; i < ParticlesPerWorkItem; ++i)
	//{
	//	const int G = 1;
	//	const int EARTH_MASS = 10;
	//
	//	for (int j = 0; j < gravitorCount; ++j)
	//	{
	//		float2 partPos = (float2)(particles[index].px, particles[index].py);
	//		float2 gravPos = (float2)(gravitors[j].px, gravitors[j].py);
	//
	//		float2 r = partPos - gravPos;
	//		float2 F = G * EARTH_MASS * normalize(r) / -pow(length(r), 2);
	//
	//		particles[index].vx += F.x * deltaTime;
	//		particles[index].vy += F.y * deltaTime;
	//	}
	//
	//	++index;
	//}
}

__kernel void calculate_energy(__global Particle* particles, int particleCount, float deltaTime)
{
	int globalId = get_global_id(0);
	int stride = get_global_size(0);

	for (int i = globalId; i < particleCount; i += stride)
	{
		particles[i].en -= deltaTime;
	}
}

__kernel void remove_dead_particles(__global Particle* particles, __global int* particleCount)
{
	for (int i = *particleCount; i > 0; --i)
	{
		if (particles[i - 1].en > 0.0f) continue;

		particles[i - 1] = particles[*particleCount - 1];
		*particleCount -= 1;
	}
}

//Deprecated, settings HSV in fragment shader is an order of magnitude or 2 faster apparently...
//__kernel void calculate_color_over_time(__global Particle* particles, int particleCount, int ParticlesPerWorkItem, float deltaTime, float speedMultiplier)
//{
//	SETUP;
//
//	for (int i = 0; i < ParticlesPerWorkItem; ++i)
//	{
//		particles[index].cr = fmod(particles[index].cr + deltaTime * speedMultiplier, 1);
//		particles[index].cg = fmod(particles[index].cg + deltaTime * speedMultiplier, 1);
//		particles[index].cb = fmod(particles[index].cb + deltaTime * speedMultiplier, 1);
//
//		++index;
//	}
//}