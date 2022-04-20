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



__kernel void calculate_position(__global Particle* particles, int particlesPerUnit, float deltaTime)
{
	int index = get_global_id(0) * particlesPerUnit;

	for (int i = 0; i < particlesPerUnit; i++)
	{
		particles[index + i].x += particles[index + i].xv * deltaTime; //refactor naar Particle particle = particles[index + i]; en verander dat dan
		particles[index + i].y += particles[index + i].yv * deltaTime;
	}
}
