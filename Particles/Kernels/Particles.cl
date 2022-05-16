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

	float pad1;
	float pad2;
	float pad3;
	float pad4;
	float pad5;
} Gravitor;

//Functions with _single postfix run only a single calculation per kernel while _looped functions iterate over a set amount of particles.
//Running work items in multiple dimensions over a single function grants a large performance gain over the looped version

__kernel void calculate_movement_single(__global Particle* particles, int particleCount, float deltaTime)
{
	int index = get_global_id(0) + get_global_id(1) * get_global_size(0);
	particles[index].px += particles[index].vx * deltaTime;
	particles[index].py += particles[index].vy * deltaTime;
	particles[index].pz += particles[index].vz * deltaTime;
}
__kernel void calculate_movement_looped(__global Particle* particles, int particleCount, float deltaTime)
{
	int globalId = get_global_id(0);
	int stride = get_global_size(0);

	for (int i = globalId; i < particleCount; i += stride)
	{
		particles[i].px += particles[i].vx * deltaTime;
		particles[i].py += particles[i].vy * deltaTime;
	}
}

__kernel void calculate_gravity_single(__global Particle* particles, int particleCount, __global Gravitor* gravitors, int gravitorCount, float deltaTime)
{
	int index = get_global_id(0) + get_global_id(1) * get_global_size(0);

	float dx, dy, dz;
	float inv_r;

	for (int i = 0; i < gravitorCount; ++i)
	{
		dx = gravitors[i].px - particles[index].px;
		dy = gravitors[i].py - particles[index].py;
		dz = gravitors[i].pz - particles[index].pz;

		inv_r = rsqrt(dx * dx + dy * dy + dz * dz);

		particles[index].vx += gravitors[i].gv * inv_r * dx * deltaTime;
		particles[index].vy += gravitors[i].gv * inv_r * dy * deltaTime;
		particles[index].vz += gravitors[i].gv * inv_r * dz * deltaTime;
	}
}
__kernel void calculate_gravity_single_alt(__global Particle* particles, int particleCount, __global Gravitor* gravitors, int gravitorCount, float deltaTime)
{
	int index = get_global_id(0) + get_global_id(1) * get_global_size(0);

	float dx, dy, dz;
	float inv_r;

	for (int i = 0; i < gravitorCount; ++i)
	{
		const int G = 1;
		const int EARTH_MASS = 10;

		float3 partPos = (float3)(particles[index].px, particles[index].py, particles[index].pz);
		float3 gravPos = (float3)(gravitors[i].px, gravitors[i].py, gravitors[i].pz);

		float3 r = partPos - gravPos;
		float3 F = G * EARTH_MASS * normalize(r) / -pow(length(r), 2);
		particles[index].vx += F.x * 0.1f * deltaTime;
		particles[index].vy += F.y * 0.1f * deltaTime;
		particles[index].vz += F.z * 0.1f * deltaTime;
	}
}
__kernel void calculate_gravity_looped(__global Particle* particles, int particleCount, __global Gravitor* gravitors, int gravitorCount, float deltaTime)
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
}

__kernel void calculate_energy_single(__global Particle* particles, int particleCount, float deltaTime)
{
	int index = get_global_id(0) + get_global_id(1) * get_global_size(0);
	particles[index].en -= deltaTime;
}
__kernel void calculate_energy_looped(__global Particle* particles, int particleCount, float deltaTime)
{
	int globalId = get_global_id(0);
	int stride = get_global_size(0);

	for (int i = globalId; i < particleCount; i += stride)
	{
		particles[i].en -= deltaTime;
	}
}

//DEPRECATED || transferring the particle array and computing it on the cpu is a lot faster
//__kernel void remove_dead_particles(__global Particle* particles, __global int* particleCount)
//{
//	for (int i = *particleCount; i > 0; --i)
//	{
//		if (particles[i - 1].en > 0.0f) continue;
//
//		particles[i - 1] = particles[*particleCount - 1];
//		*particleCount -= 1;
//	}
//}

//DEPRECATED || settings HSV in fragment shader is an order of magnitude or 2 faster apparently...
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
