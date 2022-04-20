#include <iostream>
#include <vector>
#include <random>
#include <chrono>
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

constexpr auto NUM_PARTICLES = 10000;

constexpr auto SCREEN_WIDTH = 800;
constexpr auto SCREEN_HEIGHT = 600;

constexpr auto MAX_PLATFORM_ENTRIES = 8;
constexpr auto MAX_DEVICE_ENTRIES = 16;

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
void check_error(cl_int error, int line, int compareTo = CL_SUCCESS)
{
    if (error != compareTo)
    {
        std::cerr << "CL::ERROR::" << get_error_string(error) << std::endl << "LINE " << line << std::endl;
        exit(EXIT_FAILURE);
    }
}

GLFWwindow* setup_gl()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
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

    return window;
}
cl_platform_id select_platform()
{
    int answer;

    cl_int error;

    cl_int platform_info;
    cl_uint num_platforms;
    cl_platform_id platform_arr[MAX_PLATFORM_ENTRIES];
    cl_platform_id platform_id;



    error = clGetPlatformIDs(MAX_PLATFORM_ENTRIES, platform_arr, &num_platforms);
    check_error(error, __LINE__);

    system("cls");
    std::cout << "Select a platform" << "\n" << "-----------------\n";
    for (int i = 0; i < num_platforms; i++)
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

    return platform_arr[answer - 1];
}
cl_device_id select_device(cl_platform_id platform_id)
{
    int answer;
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
    for (int i = 0; i < num_devices; i++)
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

    return device_arr[answer - 1];
}
void setup_cl()
{
    cl_platform_id platform_id;
    cl_device_id device_id;

    cl_context context;
    cl_command_queue command_queue;

    platform_id = select_platform();
    device_id = select_device(platform_id);





}

std::vector<Particle> generateParticles()
{
    std::vector<Particle> particles{};

    std::random_device rd;
    std::default_random_engine generator(rd()); // rd() provides a random seed
    std::uniform_real_distribution<float> positionDistribution(-1.0f, 1.0f);
    std::uniform_real_distribution<float> colorDistribution(0.0f, 1.0f);
    std::uniform_real_distribution<float> velocityDistribution(-0.001f, 0.001f);

    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        particles.push_back(Particle(
            positionDistribution(generator),
            positionDistribution(generator),
            0.0f,

            colorDistribution(generator),
            colorDistribution(generator),
            colorDistribution(generator),
            colorDistribution(generator),

            velocityDistribution(generator),
            velocityDistribution(generator),
            0.0f
        ));
    }

    return particles;
}

int main()
{
    //OpenGL setup
    GLFWwindow* window = setup_gl();

    //OpenCL setup
    cl_int error;											//OpenCL error code

    cl_uint num_platforms;                                  //Number of platforms present on the host. Value between 1 and MAX_PLATFORM_ENTRIES + 1
    cl_platform_id platform_id;                             //Selected platform id

    cl_uint num_devices;                                    //Number of devices present on the current platform. Value between 1 and MAX_DEVICE_ENTRIES + 1
    cl_device_id device_id;                                 //Selected device id

    cl_context context;                                     //context for command queues
    cl_command_queue command_queue;                         //program queue

    cl_program program; 
    cl_kernel kernel;

    cl_mem particleBuffer;

    const char* kernelSource;


    //Timing
    unsigned int totalPasses{};
    std::chrono::steady_clock::time_point t0;
    std::chrono::steady_clock::time_point t1;
    float deltaTime = 0.0f;



    error = clGetPlatformIDs(1, &platform_id, &num_platforms);
    check_error(error, __LINE__);

    error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, MAX_DEVICE_ENTRIES, &device_id, &num_devices);
    check_error(error, __LINE__);

    cl_context_properties properties[] =
    {
       CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
       CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
       CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id,
       NULL
    };

    context = clCreateContext(properties, num_devices, &device_id, nullptr, nullptr, &error);
    check_error(error, __LINE__);

    command_queue = clCreateCommandQueue(context, device_id, NULL, &error);
    check_error(error, __LINE__);



    Shader shader("Shaders/ParticleVertex.glsl", "Shaders/ParticleFragment.glsl");
    shader.use();

    std::vector<Particle> particleVertices = generateParticles();



    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, particleVertices.size() * sizeof(Particle), particleVertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);
    glEnableVertexAttribArray(0); //position

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); //color



    kernelSource = Utils::read_file("Kernels/Particles.cl");

    program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, &error);
    check_error(error, __LINE__);

    error = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    check_error(error, __LINE__);

    kernel = clCreateKernel(program, "calculate_position", &error);
    check_error(error, __LINE__);

    particleBuffer = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, VBO, &error);
    check_error(error, __LINE__);

    error = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&particleBuffer);
    int ppu = 10;
    error = clSetKernelArg(kernel, 1, sizeof(int), &ppu);
    check_error(error, __LINE__);
    error = clSetKernelArg(kernel, 2, sizeof(float), &deltaTime);
    check_error(error, __LINE__);



    size_t val;
    clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(val), &val, nullptr);
    size_t workSize = val;



    glPointSize(1);

    while (!glfwWindowShouldClose(window))
    {
        t0 = std::chrono::steady_clock::now();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);



        error = clEnqueueAcquireGLObjects(command_queue, 1, &particleBuffer, 0, nullptr, nullptr);
        error = clSetKernelArg(kernel, 2, sizeof(float), &deltaTime);
        error = clEnqueueNDRangeKernel(command_queue, kernel, 1, nullptr, &workSize, nullptr, 0, nullptr, nullptr);
        error = clEnqueueReleaseGLObjects(command_queue, 1, &particleBuffer, 0, nullptr, nullptr);
        error = clFinish(command_queue);



        shader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);


        glFlush();
        glfwSwapBuffers(window);
        glfwPollEvents();

        t1 = std::chrono::steady_clock::now();
        deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    }

    glDeleteProgram(shader.ID);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);



    //std::cout << "Total passes in 10 seconds: " << totalPasses;
    //std::cin.get();



    glfwTerminate();
    return 0;
}