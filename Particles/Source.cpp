#include "Source.h"

void create_gravitor(std::vector<Gravitor>& gravitors, size_t& gravitorCount, glm::vec3 position, float strength = 10.0f)
{
    static size_t indexToReplace = 0;

    gravitors[indexToReplace] = Gravitor(position, glm::vec3(0.0f), glm::vec4(1.0f), strength);
    
    if (gravitorCount < 8) ++gravitorCount;
    indexToReplace = ++indexToReplace % GRAVITOR_BUFFER_SIZE;
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
    
    Shader particleShader("Shaders/particle.vs", "Shaders/particle.fs");
    Shader gravitorShader("Shaders/gravitor.vs", "Shaders/gravitor.fs");


    std::vector<Particle> hostParticleBuffer;
    cl_mem clParticleBuffer;
    size_t particlesPerUnit;
    size_t particleCount{};
    size_t particlesToGenerate = 100000;

    std::vector<Gravitor> hostGravitorBuffer;
    cl_mem clGravitorBuffer;
    size_t gravitorCount{};

    bool calculateGravity = true;



    //Setup platform and device
    error = clGetPlatformIDs(1, &platformId, &numPlatforms);
    check_error(error, __LINE__);

    error = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_GPU, 1, &deviceId, &numDevices);
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





    //Create GL and CL Particle buffer
    hostParticleBuffer.resize(PARTICLE_BUFFER_SIZE);
    generate_particles_mt(hostParticleBuffer, particleCount, particlesToGenerate, _particle_generator_uniform);

    unsigned int PART_VAO, PART_VBO;
    glGenVertexArrays(1, &PART_VAO);
    glBindVertexArray(PART_VAO);

    glGenBuffers(1, &PART_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, PART_VBO);
    glBufferData(GL_ARRAY_BUFFER, PARTICLE_BUFFER_SIZE * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(Particle), hostParticleBuffer.data());

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    clParticleBuffer = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, PART_VBO, &error);
    check_error(error, __LINE__);



    //Create CL and GL Gravitor buffer
    hostGravitorBuffer.resize(GRAVITOR_BUFFER_SIZE);
    create_gravitor(hostGravitorBuffer, gravitorCount, glm::vec3(0.0f));
    clGravitorBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, GRAVITOR_BUFFER_SIZE * sizeof(Gravitor), nullptr, &error);
    check_error(error, __LINE__);
    error = clEnqueueWriteBuffer(commandQueue, clGravitorBuffer, CL_TRUE, 0, sizeof(Gravitor), hostGravitorBuffer.data(), 0, nullptr, nullptr);
    check_error(error, __LINE__);

    unsigned int GRAV_VAO, GRAV_VBO;
    glGenVertexArrays(1, &GRAV_VAO);
    glBindVertexArray(GRAV_VAO);
    glGenBuffers(1, &GRAV_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, GRAV_VBO);
    glBufferData(GL_ARRAY_BUFFER, GRAVITOR_BUFFER_SIZE * sizeof(Gravitor), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, gravitorCount * sizeof(Gravitor), hostGravitorBuffer.data());

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Gravitor), nullptr);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Gravitor), (void*)(2 * sizeof(glm::vec3)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);



    //Create program with kernels
    const char* kernelSource = Utils::read_file("Kernels/particles.cl");

    program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, &error);
    check_error(error, __LINE__);

    error = clBuildProgram(program, 1, &deviceId, NULL, NULL, NULL);
    check_program_compile_error(error, program, deviceId, __LINE__);

    kernelMV = clCreateKernel(program, "calculate_movement", &error);
    check_kernel_compile_error(error, program, deviceId, __LINE__);
    kernelGV = clCreateKernel(program, "calculate_gravity", &error);
    check_kernel_compile_error(error, program, deviceId, __LINE__);
    kernelLT = clCreateKernel(program, "calculate_energy", &error);
    check_kernel_compile_error(error, program, deviceId, __LINE__);

    error = clSetKernelArg(kernelMV, 0, sizeof(cl_mem), (void*)&clParticleBuffer);
    error = clSetKernelArg(kernelLT, 0, sizeof(cl_mem), (void*)&clParticleBuffer);
    error = clSetKernelArg(kernelGV, 0, sizeof(cl_mem), (void*)&clParticleBuffer);





    //Timing
    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point t0;
    std::chrono::high_resolution_clock::time_point t1;
    float deltaTime = 0.0f;

    unsigned int totalPasses{};
    auto stopTime = std::chrono::steady_clock::now() + std::chrono::seconds(30);

    //Extra kernel arguments
    float speedMultiplier = 0.1f;
    

    glm::vec3 hsv(1.0f, 1.0f, 1.0f);


    while (!glfwWindowShouldClose(window))
    {
        t0 = std::chrono::steady_clock::now();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);



        error = clEnqueueAcquireGLObjects(commandQueue, 1, &clParticleBuffer, 0, nullptr, nullptr);

        if (mousePressedOnce)
        {
            mousePressedOnce = false;
            create_gravitor(hostGravitorBuffer, gravitorCount, glm::vec3(mouseX, mouseY, 0.0f));
            glBindVertexArray(GRAV_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, GRAV_VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, gravitorCount * sizeof(Gravitor), hostGravitorBuffer.data());
            error = clEnqueueWriteBuffer(commandQueue, clGravitorBuffer, CL_TRUE, 0, gravitorCount * sizeof(Gravitor), hostGravitorBuffer.data(), 0, nullptr, nullptr);
            calculateGravity = !calculateGravity;
        }

        //error = clEnqueueReadBuffer(commandQueue, clParticleBuffer, CL_TRUE, 0, particleCount * sizeof(Particle), hostParticleBuffer.data(), 0, nullptr, nullptr);
        //remove_dead_particles(hostParticleBuffer, particleCount);
        //generate_particles_st(hostParticleBuffer, particleCount, 10, _particle_generator_cone);
        //error = clEnqueueWriteBuffer(commandQueue, clParticleBuffer, CL_TRUE, 0, particleCount * sizeof(Particle), hostParticleBuffer.data(), 0, nullptr, nullptr);

        particlesPerUnit = particleCount / globalWorkSize;
        particlesPerUnit++;



        if (calculateGravity && 0)
        {
            error = clSetKernelArg(kernelGV, 1, sizeof(int), &particleCount);
            error = clSetKernelArg(kernelGV, 2, sizeof(cl_mem), &clGravitorBuffer);
            error = clSetKernelArg(kernelGV, 3, sizeof(int), &gravitorCount);
            error = clSetKernelArg(kernelGV, 4, sizeof(int), &particlesPerUnit);
            error = clSetKernelArg(kernelGV, 5, sizeof(float), &deltaTime);
            error = clEnqueueNDRangeKernel(commandQueue, kernelGV, 1, nullptr, &globalWorkSize, nullptr, 0, nullptr, nullptr);
        }

        error = clSetKernelArg(kernelMV, 1, sizeof(int), &particleCount);
        error = clSetKernelArg(kernelMV, 2, sizeof(int), &particlesPerUnit);
        error = clSetKernelArg(kernelMV, 3, sizeof(float), &deltaTime);
        error = clEnqueueNDRangeKernel(commandQueue, kernelMV, 1, nullptr, &globalWorkSize, nullptr, 0, nullptr, nullptr);

        error = clFinish(commandQueue);
        error = clEnqueueReleaseGLObjects(commandQueue, 1, &clParticleBuffer, 0, nullptr, nullptr);



        gravitorShader.use();
        gravitorShader.setMat4("model", glm::scale(glm::mat4(1.0f), glm::vec3(MODELSCALE)));

        glPointSize(12);
        glBindVertexArray(GRAV_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, GRAV_VBO);
        glDrawArrays(GL_POINTS, 0, (GLsizei)gravitorCount);



        particleShader.use();
        particleShader.setMat4("model", glm::scale(glm::mat4(1.0f), glm::vec3(MODELSCALE)));
        particleShader.setVec3("hsv", hsv);
        hsv.x += 0.001f;

        glPointSize(1);
        glBindVertexArray(PART_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, PART_VBO);
        glDrawArrays(GL_POINTS, 0, (GLsizei)particleCount);



        glFlush();
        glfwSwapBuffers(window);
        glfwPollEvents();

        t1 = std::chrono::steady_clock::now();
        deltaTime = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(t1 - t0).count() / 1000.0f;
        
        totalPasses++;
        if (t1 > stopTime) break;
    }

    glDeleteProgram(particleShader.ID);

    glDeleteVertexArrays(1, &PART_VAO);
    glDeleteBuffers(1, &PART_VBO);

    glDeleteVertexArrays(1, &GRAV_VAO);
    glDeleteBuffers(1, &GRAV_VBO);

    auto now = std::chrono::high_resolution_clock::now();
    float timeElapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(now - startTime).count() / 1000.0f;

    system("cls");
    std::cout << "  RESULTS\n" << "  ------------------------\n";
    std::cout << "| Total updates: " << totalPasses << '\n';
    std::cout << "| Time elapsed: " << timeElapsed << '\n';
    std::cout << "| Average updates per second: " << totalPasses / timeElapsed << '\n';



    glfwTerminate();
    return 0;
}