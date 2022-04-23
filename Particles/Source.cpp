#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>
#include <thread>
#include <Windows.h>

#include "CL/opencl.h"
#include "CL/cl_gl.h"

#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "GLM/glm.hpp"
#include "GLM/gtc/type_ptr.hpp"
#include "GLM/gtc/matrix_transform.hpp"

#include "Utils.h"
#include "Shader.hpp"
#include "Particle.hpp"
#include "Attractor.hpp"
#include "ArrayObject.h"

constexpr auto PARTICLE_BUFFER_SIZE = 1000000; //1M

constexpr auto SCREEN_WIDTH = 800;
constexpr auto SCREEN_HEIGHT = 800;

constexpr auto MAX_PLATFORM_ENTRIES = 8;
constexpr auto MAX_DEVICE_ENTRIES = 16;

//shared buffer for error logging
constexpr auto BUFFER_SIZE = 1024;
char PARAM_BUFFER[BUFFER_SIZE];
size_t ANSWER_SIZE;

const char* get_error_string(cl_int error)
{
    switch (error) {
        // run-time and JIT compiler errors
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT_AVAILABLE";
    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11: return "CL_BUILD_PROGRAM_FAILURE";
    case -12: return "CL_MAP_FAILURE";
    case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case -15: return "CL_COMPILE_PROGRAM_FAILURE";
    case -16: return "CL_LINKER_NOT_AVAILABLE";
    case -17: return "CL_LINK_PROGRAM_FAILURE";
    case -18: return "CL_DEVICE_PARTITION_FAILED";
    case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

        // compile-time errors
    case -30: return "CL_INVALID_VALUE";
    case -31: return "CL_INVALID_DEVICE_TYPE";
    case -32: return "CL_INVALID_PLATFORM";
    case -33: return "CL_INVALID_DEVICE";
    case -34: return "CL_INVALID_CONTEXT";
    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
    case -36: return "CL_INVALID_COMMAND_QUEUE";
    case -37: return "CL_INVALID_HOST_PTR";
    case -38: return "CL_INVALID_MEM_OBJECT";
    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40: return "CL_INVALID_IMAGE_SIZE";
    case -41: return "CL_INVALID_SAMPLER";
    case -42: return "CL_INVALID_BINARY";
    case -43: return "CL_INVALID_BUILD_OPTIONS";
    case -44: return "CL_INVALID_PROGRAM";
    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46: return "CL_INVALID_KERNEL_NAME";
    case -47: return "CL_INVALID_KERNEL_DEFINITION";
    case -48: return "CL_INVALID_KERNEL";
    case -49: return "CL_INVALID_ARG_INDEX";
    case -50: return "CL_INVALID_ARG_VALUE";
    case -51: return "CL_INVALID_ARG_SIZE";
    case -52: return "CL_INVALID_KERNEL_ARGS";
    case -53: return "CL_INVALID_WORK_DIMENSION";
    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
    case -56: return "CL_INVALID_GLOBAL_OFFSET";
    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
    case -58: return "CL_INVALID_EVENT";
    case -59: return "CL_INVALID_OPERATION";
    case -60: return "CL_INVALID_GL_OBJECT";
    case -61: return "CL_INVALID_BUFFER_SIZE";
    case -62: return "CL_INVALID_MIP_LEVEL";
    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
    case -64: return "CL_INVALID_PROPERTY";
    case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
    case -66: return "CL_INVALID_COMPILER_OPTIONS";
    case -67: return "CL_INVALID_LINKER_OPTIONS";
    case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

        // extension errors
    case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
    case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
    case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
    case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
    case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
    case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
    default: return "Unknown OpenCL error";
    }
}
void check_error(cl_int error, int line)
{
    if (error != CL_SUCCESS)
    {
        std::cerr << "CL::ERROR::" << get_error_string(error) << std::endl << "LINE " << line << std::endl;
        exit(EXIT_FAILURE);
    }
}
void check_program_compile_error(cl_int error, cl_program program, cl_device_id device_id, int line)
{
    if (error == CL_SUCCESS)
        return;

    size_t len = 0;
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, NULL, NULL, &len);

    char* log = new char[len];
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, len, log, NULL);

    std::cout << log;
    exit(EXIT_FAILURE);
}
void check_kernel_compile_error(cl_int error, cl_program program, cl_device_id device_id, int line) //zelfde code als hierboven GG uninstall
{
    if (error == CL_SUCCESS)
        return;

    size_t len = 0;
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, NULL, NULL, &len);

    char* log = new char[len];
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, len, log, NULL);

    std::cout << log;
    exit(EXIT_FAILURE);
}

GLFWwindow* setup_gl()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Particles", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(1);
    }

    glfwSwapInterval(0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glPointSize(1);

    return window;
}
cl_platform_id _select_platform()
{
    unsigned int answer;

    cl_int error;

    cl_int platform_info;
    cl_uint num_platforms;
    cl_platform_id platform_arr[MAX_PLATFORM_ENTRIES];
    cl_platform_id platform_id;



    error = clGetPlatformIDs(MAX_PLATFORM_ENTRIES, platform_arr, &num_platforms);
    check_error(error, __LINE__);

    system("cls");
    std::cout << "Select a platform" << "\n" << "-----------------\n";
    for (size_t i = 0; i < num_platforms; i++)
    {
        platform_info = clGetPlatformInfo(platform_arr[i], CL_PLATFORM_NAME, BUFFER_SIZE, PARAM_BUFFER, &ANSWER_SIZE);
        std::cout << i + 1 << ": " << PARAM_BUFFER << '\n';
    }

    std::cin >> answer;

    if (answer < 1 || answer > num_platforms)
    {
        std::cout << "\nInvalid platform selected\n";
        exit(EXIT_FAILURE);
    }

    platform_id = platform_arr[answer - 1];
    return platform_id;
}
cl_device_id _select_device(cl_platform_id platform_id)
{
    unsigned int answer;
    int device_type;

    cl_int error;

    cl_int device_info;
    cl_uint num_devices;
    cl_device_id device_arr[MAX_DEVICE_ENTRIES];
    cl_device_id device_id;



    system("cls");
    std::cout << "Select a device type" << "\n" << "-----------------\n";
    std::cout << "1. CPU\n" << "2. GPU\n";
    std::cin >> answer;

    if (answer < 1 || answer > 2)
    {
        std::cout << "\nInvalid device selected\n";
        exit(EXIT_FAILURE);
    }

    switch (answer)
    {
    case 1:
        device_type = CL_DEVICE_TYPE_CPU;
        break;
    case 2:
        device_type = CL_DEVICE_TYPE_GPU;
        break;
    case 3:
        device_type = CL_DEVICE_TYPE_ALL;
        break;
    default:
        exit(EXIT_FAILURE);
    }

    error = clGetDeviceIDs(platform_id, device_type, MAX_DEVICE_ENTRIES, device_arr, &num_devices);
    check_error(error, __LINE__);



    system("cls");
    std::cout << "Select a device" << "\n" << "-----------------\n";
    for (size_t i = 0; i < num_devices; i++)
    {
        device_info = clGetDeviceInfo(device_arr[i], CL_DEVICE_NAME, BUFFER_SIZE, PARAM_BUFFER, &ANSWER_SIZE);
        std::cout << i + 1 << ": " << PARAM_BUFFER << '\n';
    }

    std::cin >> answer;

    if (answer < 1 || answer > num_devices)
    {
        std::cout << "\nInvalid device selected\n";
        exit(EXIT_FAILURE);
    }

    device_id = device_arr[answer - 1];
    return device_id;
}
array_object create_particle_vertex_object()
{
    array_object ao{};

    glGenVertexArrays(1, &ao.id);
    glBindVertexArray(ao.id);

    glGenBuffers(1, &ao.buffer);
    glBindBuffer(GL_ARRAY_BUFFER, ao.buffer);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);
    glEnableVertexAttribArray(0); //position

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1); //color

    return ao;
}

void setup_cl()
{
    cl_platform_id platform_id;
    cl_device_id device_id;

    //cl_context context;
    //cl_command_queue command_queue;

    platform_id = _select_platform();
    device_id = _select_device(platform_id);
}

typedef void(* particle_generator)(std::vector<Particle>&, size_t, size_t);
void _particle_generator_uniform(std::vector<Particle>& particles, size_t offset, size_t amount)
{
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<float> positionDistribution(-1.0f, 1.0f);
    std::uniform_real_distribution<float> colorDistribution(0.0f, 1.0f);
    std::uniform_real_distribution<float> velocityDistribution(-0.1f, 0.1f);
    std::uniform_real_distribution<float> lifeTimeDistribution(0.0f, 10.0f);

    for (size_t i = 0; i < amount; i++)
    {
        particles[offset + i] = Particle(
            positionDistribution(generator),
            positionDistribution(generator),

            0.0f, //colorDistribution(generator),
            1.0f, //colorDistribution(generator),
            0.0f, //colorDistribution(generator),
            0.2f, //colorDistribution(generator),

            //velocityDistribution(generator),
            //velocityDistribution(generator),
            0.0f, 
            0.0f, 

            0.0f,
            0.0f,

            //lifeTimeDistribution(generator)
            100.0f
        );
    }
}
void _particle_generator_circle(std::vector<Particle>& particles, size_t offset, size_t amount)
{
    float r;
    float theta;

    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<float> positionDistribution(0.0f, 1.0f);
    std::uniform_real_distribution<float> angleDistribution(0.0f, 2 * (float)CL_M_PI);
    std::uniform_real_distribution<float> colorDistribution(0.0f, 1.0f);
    std::uniform_real_distribution<float> velocityDistribution(-0.1f, 0.1f);
    //std::uniform_real_distribution<float> accelerationDistribution(0.0f, -0.1f);
    std::uniform_real_distribution<float> lifeTimeDistribution(0.0f, 10.0f);

    glm::vec2 velocity;

    for (int i = 0; i < amount; i++)
    {
        r = sqrt(positionDistribution(generator));
        theta = positionDistribution(generator) * 2 * (float)CL_M_PI;

        velocity = glm::vec2(velocityDistribution(generator), velocityDistribution(generator));
        velocity = glm::normalize(velocity);

        particles[offset + i] = Particle(
            r * cos(theta) * 0.3f,
            r * sin(theta) * 0.3f,

            colorDistribution(generator),
            colorDistribution(generator),
            colorDistribution(generator),
            colorDistribution(generator),

            velocity.x,
            velocity.y,

            0.0f,
            -2.0f, 
            //accelerationDistribution(generator), 
            //accelerationDistribution(generator), 

            lifeTimeDistribution(generator)
        );
    }
}
void _particle_generator_cone(std::vector<Particle>& particles, size_t offset, size_t amount)
{
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<float> positionDistribution(0.0f, 1.0f);
    std::uniform_real_distribution<float> angleDistribution(0.0f, 2 * (float)CL_M_PI);
    std::uniform_real_distribution<float> colorDistribution(0.0f, 1.0f);
    std::uniform_real_distribution<float> velocityDistribution(-0.1f, 0.1f);
    //std::uniform_real_distribution<float> accelerationDistribution(0.0f, -0.1f);
    std::uniform_real_distribution<float> lifeTimeDistribution(0.0f, 10.0f);

    glm::vec2 velocity;

    for (int i = 0; i < amount; i++)
    {
        float angle = (90.0f - 0.0f) * positionDistribution(generator) + 0.0f;
        angle = angle * (float)CL_M_PI / 180.0f;
        velocity = glm::vec2(cos(angle), sin(angle));

        particles[offset + i] = Particle(
            0.0f, 
            0.0f, 

            colorDistribution(generator),
            colorDistribution(generator),
            colorDistribution(generator),
            colorDistribution(generator),

            velocity.x,
            velocity.y,

            0.0f,
            0.0f, 

            lifeTimeDistribution(generator)
        );
    }
}

void generate_particles_st(std::vector<Particle>& particles, size_t& particleCount, size_t amount, particle_generator generator)
{
    if (particleCount + amount > PARTICLE_BUFFER_SIZE) amount = PARTICLE_BUFFER_SIZE - particleCount;

    generator(particles, particleCount, amount);
    particleCount += amount;
}
void generate_particles_mt(std::vector<Particle>& particles, size_t& particleCount, size_t amount, particle_generator generator)
{
    if (particleCount + amount > PARTICLE_BUFFER_SIZE) amount = PARTICLE_BUFFER_SIZE - particleCount;

    constexpr auto THREAD_COUNT = 8;
    size_t particlesPerThread = amount / THREAD_COUNT;
    size_t particleRemainder = amount % THREAD_COUNT;

    std::vector<std::thread> threads;

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        size_t offset = i * particlesPerThread + particleCount;
        threads.emplace_back([&, offset]() { generator(particles, offset, particlesPerThread); });
    }

    if (particleRemainder)
    {
        size_t offset = THREAD_COUNT * particlesPerThread;
        generator(particles, offset, particleRemainder);
    }

    for (auto& thread : threads)
        thread.join();

    particleCount += amount;
}
void remove_dead_particles(std::vector<Particle>& particles, size_t& particleCount)
{
    Particle* pPtr;
    size_t removedParticles = 0;

    for (size_t i = particleCount; i > 0; i--)
    {
        pPtr = &particles[i - 1];

        if (pPtr->life_time > pPtr->time_to_live)
        {
            *pPtr = particles[particleCount - removedParticles - 1];
            particleCount--;
        }
    }
}

int main()
{
    //OpenGL setup
    GLFWwindow* window = setup_gl();

    //OpenCL setup
    cl_int error;										    //OpenCL error code

    cl_uint numPlatforms;                                   //Number of platforms present on the host. Value between 1 and MAX_PLATFORM_ENTRIES + 1
    cl_platform_id platformId;                              //Selected platform id

    cl_uint numDevices;                                     //Number of devices present on the current platform. Value between 1 and MAX_DEVICE_ENTRIES + 1
    cl_device_id deviceId;                                  //Selected device id

    cl_context context;                                     //context for command queues
    cl_command_queue commandQueue;                          //program queue

    cl_program program;
    cl_kernel kernelLT;
    cl_kernel kernelMV;
    cl_kernel kernelCOT;
    cl_kernel kernelGV;

    size_t globalWorkSize;
    //size_t local_work_size;

    std::vector<Particle> hostParticleBuffer;
    cl_mem clParticleBuffer;
    size_t particlesPerUnit;
    size_t particleCount = 0;
    size_t particlesToGenerate = 10000;

    std::vector<Attractor> hostAttractorBuffer;
    cl_mem clAttractorBuffer;
    size_t attractorCount = 1;
    //size_t attractorsToGenerate;

    Shader shader("Shaders/ParticleVertex.glsl", "Shaders/ParticleFragment.glsl");
    


    //Setup platform and device
    error = clGetPlatformIDs(1, &platformId, &numPlatforms);
    check_error(error, __LINE__);

    error = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_GPU, MAX_DEVICE_ENTRIES, &deviceId, &numDevices);
    check_error(error, __LINE__);

    cl_context_properties properties[] =
    {
       CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
       CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
       CL_CONTEXT_PLATFORM, (cl_context_properties)platformId,
       NULL
    };
    context = clCreateContext(properties, numDevices, &deviceId, nullptr, nullptr, &error);
    check_error(error, __LINE__);

    commandQueue = clCreateCommandQueue(context, deviceId, NULL, &error);
    check_error(error, __LINE__);

    clGetDeviceInfo(deviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(globalWorkSize), &globalWorkSize, nullptr);



    //Generate initial particles and move to gl buffer
    hostParticleBuffer.resize(PARTICLE_BUFFER_SIZE);
    generate_particles_mt(hostParticleBuffer, particleCount, particlesToGenerate, _particle_generator_uniform);

    array_object ao = create_particle_vertex_object();
    glBufferData(GL_ARRAY_BUFFER, PARTICLE_BUFFER_SIZE * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(Particle), hostParticleBuffer.data());
    clParticleBuffer = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, ao.buffer, &error);
    check_error(error, __LINE__);

    //Generate attractor and move to cl buffer
    hostAttractorBuffer.push_back(Attractor(0.3f, 0.3f, 0.0f, 0.0f, 1.0f));
    clAttractorBuffer = clCreateBuffer(context, CL_MEM_COPY_HOST_PTR, 1 * sizeof(Attractor), hostAttractorBuffer.data(), &error);
    check_error(error, __LINE__);



    //Create program with kernels
    const char* kernelSource = Utils::read_file("Kernels/Particles.cl");

    program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, &error);
    check_error(error, __LINE__);

    error = clBuildProgram(program, 1, &deviceId, NULL, NULL, NULL);
    check_program_compile_error(error, program, deviceId, __LINE__);

    kernelLT = clCreateKernel(program, "calculate_life_time", &error);
    check_kernel_compile_error(error, program, deviceId, __LINE__);
    kernelMV = clCreateKernel(program, "calculate_movement", &error);
    check_kernel_compile_error(error, program, deviceId, __LINE__);
    kernelCOT = clCreateKernel(program, "calculate_color_over_time", &error);
    check_kernel_compile_error(error, program, deviceId, __LINE__);
    kernelGV = clCreateKernel(program, "calculate_gravity", &error);
    check_kernel_compile_error(error, program, deviceId, __LINE__);

    error = clSetKernelArg(kernelMV, 0, sizeof(cl_mem), (void*)&clParticleBuffer);
    error = clSetKernelArg(kernelCOT, 0, sizeof(cl_mem), (void*)&clParticleBuffer);
    error = clSetKernelArg(kernelLT, 0, sizeof(cl_mem), (void*)&clParticleBuffer);
    error = clSetKernelArg(kernelGV, 0, sizeof(cl_mem), (void*)&clParticleBuffer);



    //Timing
    std::chrono::high_resolution_clock::time_point t0;
    std::chrono::high_resolution_clock::time_point t1;
    float deltaTime = 0.0f;

    unsigned int totalPasses{};
    auto stopTime = std::chrono::steady_clock::now() + std::chrono::seconds(10);

    //Extra kernel arguments
    float speedMultiplier = 0.1f;

    while (!glfwWindowShouldClose(window))
    {
        t0 = std::chrono::steady_clock::now();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);



        error = clEnqueueAcquireGLObjects(commandQueue, 1, &clParticleBuffer, 0, nullptr, nullptr);

        error = clEnqueueReadBuffer(commandQueue, clParticleBuffer, CL_TRUE, 0, particleCount * sizeof(Particle), hostParticleBuffer.data(), 0, nullptr, nullptr);
        remove_dead_particles(hostParticleBuffer, particleCount);
        //generate_particles_st(host_particle_buf, particleCount, 1, _particle_generator_cone);
        error = clEnqueueWriteBuffer(commandQueue, clParticleBuffer, CL_TRUE, 0, particleCount * sizeof(Particle), hostParticleBuffer.data(), 0, nullptr, nullptr);

        particlesPerUnit = particleCount / globalWorkSize;
        particlesPerUnit++;

        error = clSetKernelArg(kernelMV, 1, sizeof(int), &particleCount);
        error = clSetKernelArg(kernelMV, 2, sizeof(int), &particlesPerUnit);
        error = clSetKernelArg(kernelMV, 3, sizeof(float), &deltaTime);
        error = clSetKernelArg(kernelMV, 4, sizeof(float), &speedMultiplier);
        error = clEnqueueNDRangeKernel(commandQueue, kernelMV, 1, nullptr, &globalWorkSize, nullptr, 0, nullptr, nullptr);

        //error = clSetKernelArg(kernelCOT, 1, sizeof(int), &particleCount);
        //error = clSetKernelArg(kernelCOT, 2, sizeof(int), &particlesPerUnit);
        //error = clSetKernelArg(kernelCOT, 3, sizeof(float), &deltaTime);
        //error = clSetKernelArg(kernelCOT, 4, sizeof(float), &speedMultiplier);
        //error = clEnqueueNDRangeKernel(commandQueue, kernelCOT, 1, nullptr, &globalWorkSize, nullptr, 0, nullptr, nullptr);

        error = clSetKernelArg(kernelLT, 1, sizeof(int), &particleCount);
        error = clSetKernelArg(kernelLT, 2, sizeof(int), &particlesPerUnit);
        error = clSetKernelArg(kernelLT, 3, sizeof(float), &deltaTime);
        error = clSetKernelArg(kernelLT, 4, sizeof(float), &speedMultiplier);
        error = clEnqueueNDRangeKernel(commandQueue, kernelLT, 1, nullptr, &globalWorkSize, nullptr, 0, nullptr, nullptr);

        //__global Particle* particles, int particleCount, __global Attractor* attractors, int attractorCount, int particlesPerUnit, float deltaTime, float speedMultiplier
        error = clSetKernelArg(kernelGV, 1, sizeof(int), &particleCount);
        error = clSetKernelArg(kernelGV, 2, sizeof(cl_mem), &clAttractorBuffer);
        error = clSetKernelArg(kernelGV, 3, sizeof(int), &attractorCount);
        error = clSetKernelArg(kernelGV, 4, sizeof(int), &particlesPerUnit);
        error = clSetKernelArg(kernelGV, 5, sizeof(float), &deltaTime);
        error = clSetKernelArg(kernelGV, 6, sizeof(float), &speedMultiplier);
        error = clEnqueueNDRangeKernel(commandQueue, kernelGV, 1, nullptr, &globalWorkSize, nullptr, 0, nullptr, nullptr);

        error = clFinish(commandQueue);
        error = clEnqueueReleaseGLObjects(commandQueue, 1, &clParticleBuffer, 0, nullptr, nullptr);



        shader.use();
        glBindVertexArray(ao.id);
        glBindBuffer(GL_ARRAY_BUFFER, ao.buffer);
        glDrawArrays(GL_POINTS, 0, (GLsizei)particleCount);



        glFlush();
        glfwSwapBuffers(window);
        glfwPollEvents();

        t1 = std::chrono::steady_clock::now();
        deltaTime = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(t1 - t0).count() / 1000.0f;
        
        totalPasses++;
        //if (t1 > stopTime) break;
    }

    glDeleteProgram(shader.ID);

    glDeleteVertexArrays(1, &ao.id);
    glDeleteBuffers(1, &ao.buffer);

    std::cout << "Total passes: " << totalPasses << '\n';
    std::cout << "Frames per second: " << totalPasses / 10.0f << '\n';

    glfwTerminate();
    return 0;
}