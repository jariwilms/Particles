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

	float time_alive;
	float life_time;
} Particle;



//__kernel void wang_hash(uint seed, uint* result)
//{
//        seed = (seed ^ 61) ^ (seed >> 16);
//        seed *= 9;
//
//        seed = seed ^ (seed >> 4);
//        seed *= 0x27D4EB2D;
//
//        seed = seed ^ (seed >> 15);
//
//        *result = seed;
// }
//
//
//
//__kernel void generate_random_float(int global_id, float* result)
//{
//     uint random_uint;
//	 wang_hash(global_id, &random_uint);
//
//     *result = (float)random_uint / (float)UINT_MAX;
//}
//
//
//
//__kernel void generate_particles_uniform(__global Particle* particles, int particlesPerUnit, uint amount, uint2 random)
//{
//	int global_id = get_global_id(0);
//	int index;
//
//	int random_x;
//	int random_y;
//
//	//int random_vx;
//	//int random_vy;
//
//	for (int i = 0; i < particlesPerUnit; i++)
//	{
//		index = global_id * particlesPerUnit + i;
//
//		generate_random_float(global_id * i + random.x, &random_x);
//		generate_random_float(global_id * i + random.y, &random_y);
//
//		particles[index].x = random_x;
//		particles[index].y = random_y;
//
//		particles[index].z = 0.0f;
//
//		particles[index].r = 1.0f;
//		particles[index].g = 1.0f;
//		particles[index].b = 1.0f;
//		particles[index].a = 1.0f;
//
//		particles[index].xv = 0.0f;
//		particles[index].yv = 0.0f;
//	}
//}



__kernel void adjust_time_alive(__global Particle* particles, int particlesPerUnit, float deltaTime)
{
	int global_id = get_global_id(0) * particlesPerUnit;
	int index;

	for (int i = 0; i < particlesPerUnit; i++)
	{
		index = global_id + i;
		particles[index].time_alive += deltaTime;
	}
}

__kernel void calculate_position(__global Particle* particles, int particlesPerUnit, float deltaTime, float speedMultiplier)
{
	int global_id = get_global_id(0) * particlesPerUnit;
	int index;

	for (int i = 0; i < particlesPerUnit; i++)
	{
		index = global_id + i;

		particles[index].x += particles[index].xv * deltaTime * speedMultiplier;
		particles[index].y += particles[index].yv * deltaTime * speedMultiplier;
	}
}



__kernel void calculate_color_over_time(__global Particle* particles, int particlesPerUnit, float deltaTime)
{
	int global_id = get_global_id(0) * particlesPerUnit;
	deltaTime /= 1000.0f;
	int index;

	for (int i = 0; i < particlesPerUnit; i++)
	{
		index = global_id + i;

		particles[index].r = particles[index].r + 1.0f * deltaTime;
		particles[index].g = particles[index].g + 1.0f * deltaTime;
		particles[index].b = particles[index].b + 1.0f * deltaTime;

		if (particles[index].r > 1.0f) particles[index].r -= 1.0f;
		if (particles[index].g > 1.0f) particles[index].g -= 1.0f;
		if (particles[index].b > 1.0f) particles[index].b -= 1.0f;
	}
}