typedef struct __attribute__ ((packed)) Particle
{
	float px;	
	float py;
	float pz;

	float vx;	
	float vy;
	float vz;

	float ax;	
	float ay;
	float az;

	float cr;	
	float cg;
	float cb;
	float ca;

	float en;	
} Particle;

typedef struct __attribute__ ((packed)) Gravitor
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



//Probably the yankiest pre-compiler shit I have ever done. Can't really complain if it works tho
#define SETUP \
	int globalId = get_global_id(0); \
	int index = globalId * particlesPerUnit; \
	int final = ++globalId * particlesPerUnit - 1; \
	particlesPerUnit -= final > particleCount ? final - particleCount : 0;



__kernel void calculate_movement(__global Particle* particles, int particleCount, int particlesPerUnit, float deltaTime)
{
	SETUP;

	for (int i = 0; i < particlesPerUnit; i++)
	{
		particles[index].px += particles[index].vx * deltaTime;
		particles[index].py += particles[index].vy * deltaTime;

		++index;
	}
}



//Deprecated, settings HSV in fragment shader is an order of magnitude or 2 faster apparently...
__kernel void calculate_color_over_time(__global Particle* particles, int particleCount, int particlesPerUnit, float deltaTime, float speedMultiplier)
{
	SETUP;

	for (int i = 0; i < particlesPerUnit; i++)
	{
		particles[index].cr = fmod(particles[index].cr + deltaTime * speedMultiplier, 1);
		particles[index].cg = fmod(particles[index].cg + deltaTime * speedMultiplier, 1);
		particles[index].cb = fmod(particles[index].cb + deltaTime * speedMultiplier, 1);

		++index;
	}
}



__kernel void calculate_gravity(__global Particle* particles, int particleCount, __global Gravitor* gravitors, int gravitorCount, int particlesPerUnit, float deltaTime)
{
	SETUP;

	float dx, dy;
	float inv, inv_r;
	float ax, ay;

	for (int i = 0; i < particlesPerUnit; ++i)
	{
		for (int j = 0; j < gravitorCount; j++)
		{
			dx = gravitors[j].px - particles[index].px;
			dy = gravitors[j].py - particles[index].py;

			inv = 1.0f / (dx * dx + dy * dy);
			inv_r = sqrt(inv);
	
			ax = gravitors[j].gv * inv_r * dx;
			ay = gravitors[j].gv * inv_r * dy;
	
			particles[index].vx += ax * deltaTime;
			particles[index].vy += ay * deltaTime;
		}

		++index;
	}
}



__kernel void calculate_energy(__global Particle* particles, int particleCount, int particlesPerUnit, float deltaTime)
{
	SETUP;

	for (int i = 0; i < particlesPerUnit; i++)
	{
		particles[index].en -= deltaTime;
		++index;
	}
}