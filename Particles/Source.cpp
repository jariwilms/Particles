#define DEBUG 0

#include "Source.hpp"

void create_gravitor(std::vector<Gravitor>& gravitors, size_t& gravitorCount, glm::vec2 position, float strength = 1.0f)
{
    static size_t indexToReplace = 0;

    gravitors[indexToReplace] = Gravitor(glm::vec3(position, 0.0f), glm::vec3(0.0f), glm::vec4(1.0f), strength);
    
    if (gravitorCount < 8) ++gravitorCount;
    indexToReplace = ++indexToReplace % gravitors.size();
}
void remove_dead_particles(Particle* particles, size_t& particleCount)
{
    //Traverse the array of alive particles in reverse
    //Dead particles are switched with the backmost particle, and particleCount is decremented
    for (size_t i = particleCount; i > 0; --i)
    {
        if (particles[i - 1].energy > 0.0f) continue;

        particles[i - 1] = particles[particleCount - 1];
        --particleCount;
    }
}
void adjust_model_scale(float value, float& modelScale)
{
    modelScale += value / 50.0f;

    if (modelScale < 0.001f) modelScale = 0.001f;
    if (modelScale > 1.0f) modelScale = 1.0f;
}
glm::vec2 cursor_to_world_position(glm::vec2 cursorScreenPosition, float modelScale, glm::vec3 viewOffset, glm::uvec2 windowDimensions)
{
    return glm::vec2(
        ( cursorScreenPosition.x * 2 / windowDimensions.x - viewOffset.x - 1.0f) / modelScale,
        (-cursorScreenPosition.y * 2 / windowDimensions.y - viewOffset.y + 1.0f) / modelScale);
}
void generatorTestFunc()
{
    std::vector<Particle> particles;
    GeneratorSettings settings;
    size_t particlesToGenerate = 0;
    size_t particleCount = 0;

    particles.resize(10000000);

    std::chrono::high_resolution_clock::time_point t0;
    std::chrono::high_resolution_clock::time_point t1;
    float singleDelta = 0.0f;
    float multiDelta = 0.0f;

    for (int i = 0; i < 50; ++i) //1k => 10M
    {
        //particlesToGenerate = 1000 * (int)std::pow(10, i);
        particlesToGenerate += 20000;
        std::cout << "Generating " << particlesToGenerate << " particles\n";
        
        t0 = std::chrono::high_resolution_clock::now();
        generate_particles_st(particles.data(), particleCount, particlesToGenerate, _particle_generator_uniform, settings);
        t1 = std::chrono::high_resolution_clock::now();
        singleDelta = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(t1 - t0).count() / 1000.0f;
        std::cout << "ST TIME: " << singleDelta << '\n';

        particleCount = 0;

        t0 = std::chrono::high_resolution_clock::now();
        generate_particles_mt(particles.data(), particleCount, particlesToGenerate, _particle_generator_uniform, settings);
        t1 = std::chrono::high_resolution_clock::now();
        multiDelta = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(t1 - t0).count() / 1000.0f;
        std::cout << "MT TIME: " << multiDelta << "\n";

        if (singleDelta < multiDelta)
            std::cout << "Single thread was " << multiDelta - singleDelta << " seconds faster\n\n";
        else
            std::cout << "Multi thread was " << singleDelta - multiDelta << " seconds faster\n\n";

        particleCount = 0;
    }

    exit(1);
}

int main()
{
    cl_int error;								                                        //OpenCL error code return
    cl_event profiler = nullptr;                                                        //Event for profiling kernel execution time

    cl_uint numPlatforms;                                                               //Number of platforms present on the host. Value from 1 to MAX_PLATFORM_ENTRIES
    cl_platform_id platformId;                                                          //Selected platform id

    cl_uint numDevices;                                                                 //Number of devices present on the current platform. Value from 1 to MAX_DEVICE_ENTRIES
    cl_device_id deviceId;                                                              //Selected device id

    cl_context context;                                                                 //Device context
    cl_command_queue commandQueue;                                                      //Queue for device commands

    cl_program program;                                                                 //Program that contains kernels
    cl_kernel kernelMV;                                                                 //KERNEL: Calculates particle movement
    cl_kernel kernelGV;                                                                 //KERNEL: Calculates particle gravity
    cl_kernel kernelEN;                                                                 //KERNEL: Calculates particle energy

    size_t globalWorkSize[3];                                                           //Amount of work dimensions the kernels are divided in



    cl_mem clParticleBuffer;                                                            //Pointer to GPU allocated particle buffer
    size_t initialParticles = 3000000;                                                  //Amount of particles to generate at the start of the simulation
    size_t particleCount{};                                                             //Amount of particles => separate variable to prevent host/GPU buffer resizing

    std::vector<Gravitor> hostGravitorBuffer;                                           //Gravitor buffer of static size that remains on the host
    cl_mem clGravitorBuffer;                                                            //Pointer to GPU allocated gravitor buffer
    size_t gravitorCount{};                                                             //Amount of gravitors = > separate variable to prevent host / GPU buffer resizing (kinda useless because GRAVITOR_BUFFER_SIZE is 8

    std::vector<Emitter*> emitters{};                                                   //Vector of particle emitters



    glm::uvec2 windowDimensions(800);                                                   //Dimensions of viewport
    GLFWwindow* window = setup_gl(windowDimensions);                                    //OpenGL setup
    InputHandler inputHandler(window);                                                  //InputHandler class
    Shader particleShader("Shaders/particle.vs", "Shaders/particle.fs");                //Particle shader
    Shader gravitorShader("Shaders/gravitor.vs", "Shaders/gravitor.fs");                //Gravitor shader



    std::chrono::high_resolution_clock::time_point startTime;                           //Start time of the simulation, initialized before update loop
    std::chrono::high_resolution_clock::time_point endTime;                             //Time to elapse before stopping the simulation
    std::chrono::high_resolution_clock::time_point t0;                                  //Time point before loop executes
    std::chrono::high_resolution_clock::time_point t1;                                  //Time point after loop has executed
    unsigned int updateCount{};                                                         //Total amount of updates the simulation has done
    float deltaTime{};                                                                  //Elapsed time between each update



    bool calculateMovement = true;                                                      //Should movement be calculated every update?
    bool calculateGravity = true;                                                       //Should gravity be calculated every update? Is only true is calculateMovement is true
    bool calculateEnergy = false;                                                       //Should energy be calculated every update?

    glm::vec3 hsv(0.0f, 0.0f, 0.0f);
    glm::vec2 cursorScreenPosition{};                                                   //The on-screen position of the cursor
    glm::vec2 cursorWorldPosition{};                                                    //The modelScale adjusted world position
    glm::vec2 scrollValue{};                                                            //The inputted scroll direction

    int particleSize = 1;                                                               //Size of particles
    float speedMultiplier = 0.1f;                                                       //Calculation speed modifier (works only on hue shifting atm)
    float modelScale = 0.2f;                                                            //The zoom scale
    glm::vec3 viewOffset(0.0f);                                                         //Camera position



    //OpenCL setup
    error = clGetPlatformIDs(1, &platformId, &numPlatforms);
    error = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_GPU, 1, &deviceId, &numDevices);

    cl_context_properties properties[] = 
    {
       CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
       CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
       CL_CONTEXT_PLATFORM, (cl_context_properties)platformId,
       NULL
    };

    context = clCreateContext(properties, numDevices, &deviceId, nullptr, nullptr, &error);

#if DEBUG
    commandQueue = clCreateCommandQueue(context, deviceId, CL_QUEUE_PROFILING_ENABLE, &error);
#else
    commandQueue = clCreateCommandQueue(context, deviceId, NULL, &error);
#endif

    clGetDeviceInfo(deviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(globalWorkSize), &globalWorkSize, nullptr);
    globalWorkSize[1] = (initialParticles / globalWorkSize[0]) + 1;





    //Create shared particle buffer
    unsigned int PART_VAO, PART_VBO;
    glGenVertexArrays(1, &PART_VAO);
    glBindVertexArray(PART_VAO);

    glGenBuffers(1, &PART_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, PART_VBO);
    if (calculateEnergy) glBufferData(GL_ARRAY_BUFFER, PARTICLE_BUFFER_SIZE * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);
    else glBufferData(GL_ARRAY_BUFFER, PARTICLE_BUFFER_SIZE * sizeof(Particle), nullptr, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(2 * sizeof(glm::vec3)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    clParticleBuffer = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, PART_VBO, &error);
    check_error(error, __LINE__);



    //Create shared gravitor buffer
    hostGravitorBuffer.resize(GRAVITOR_BUFFER_SIZE);
    create_gravitor(hostGravitorBuffer, gravitorCount, glm::vec3(0.0f));
    clGravitorBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, GRAVITOR_BUFFER_SIZE * sizeof(Gravitor), nullptr, &error);
    error = clEnqueueWriteBuffer(commandQueue, clGravitorBuffer, CL_TRUE, 0, sizeof(Gravitor), hostGravitorBuffer.data(), 0, nullptr, nullptr);

    unsigned int GRAV_VAO, GRAV_VBO;
    glGenVertexArrays(1, &GRAV_VAO);
    glBindVertexArray(GRAV_VAO);
    glGenBuffers(1, &GRAV_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, GRAV_VBO);
    glBufferData(GL_ARRAY_BUFFER, GRAVITOR_BUFFER_SIZE * sizeof(Gravitor), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, gravitorCount * sizeof(Gravitor), hostGravitorBuffer.data());

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Gravitor), nullptr);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Gravitor), (void*)(2 * sizeof(glm::vec3)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);





    //Create program and compile GPU kernels
    const char* kernelSource = Utils::read_file("Kernels/particles.cl");

    program = clCreateProgramWithSource(context, 1, &kernelSource, nullptr, &error);
    check_error(error, __LINE__);

    error = clBuildProgram(program, 1, &deviceId, nullptr, nullptr, nullptr);
    check_program_compile_error(error, program, deviceId, __LINE__);

    kernelMV = clCreateKernel(program, "calculate_movement_single", &error);
    check_kernel_compile_error(error, program, deviceId, __LINE__);
    kernelGV = clCreateKernel(program, "calculate_gravity_single", &error);
    check_kernel_compile_error(error, program, deviceId, __LINE__);
    kernelEN = clCreateKernel(program, "calculate_energy_single", &error);
    check_kernel_compile_error(error, program, deviceId, __LINE__);

    error = clSetKernelArg(kernelMV, 0, sizeof(cl_mem), (void*)&clParticleBuffer);
    error = clSetKernelArg(kernelGV, 0, sizeof(cl_mem), (void*)&clParticleBuffer);
    error = clSetKernelArg(kernelGV, 2, sizeof(cl_mem), (void*)&clGravitorBuffer);
    error = clSetKernelArg(kernelEN, 0, sizeof(cl_mem), (void*)&clParticleBuffer);





    GeneratorSettings settings;
    settings.velocity_min = glm::vec3(2.0f, 0.0f, 0.0f);
    settings.velocity_max = glm::vec3(2.0f, 0.0f, 0.0f);

    //Map GPU memory to a host pointer and generate an initial amount of particles
    error = clEnqueueAcquireGLObjects(commandQueue, 1, &clParticleBuffer, 0, nullptr, nullptr);
    Particle* particles = (Particle*)clEnqueueMapBuffer(commandQueue, clParticleBuffer, CL_TRUE, CL_MEM_WRITE_ONLY, 0, initialParticles * sizeof(Particle), 0, nullptr, nullptr, &error);
    Emitter::GenerateOnce(particles, particleCount, initialParticles, _particle_generator_uniform, settings);
    clEnqueueUnmapMemObject(commandQueue, clParticleBuffer, particles, 0, nullptr, nullptr);
    error = clEnqueueReleaseGLObjects(commandQueue, 1, &clParticleBuffer, 0, nullptr, nullptr);



    //emitters.push_back(new PulseEmitter(100.0f, 1.0f));

    startTime = std::chrono::high_resolution_clock::now();
    endTime = std::chrono::high_resolution_clock::now() + std::chrono::seconds(30);

    while (!glfwWindowShouldClose(window))
    {
        t0 = std::chrono::high_resolution_clock::now();
        inputHandler.update();
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);



        error = clEnqueueAcquireGLObjects(commandQueue, 1, &clParticleBuffer, 0, nullptr, nullptr);

        if (inputHandler.is_key_pressed(MOVE_UP_INPUT)) viewOffset.y -= 0.01f * modelScale;
        if (inputHandler.is_key_pressed(MOVE_LEFT_INPUT)) viewOffset.x += 0.01f * modelScale;
        if (inputHandler.is_key_pressed(MOVE_DOWN_INPUT)) viewOffset.y += 0.01f * modelScale;
        if (inputHandler.is_key_pressed(MOVE_RIGHT_INPUT)) viewOffset.x -= 0.01f * modelScale;

        if (inputHandler.is_button_pressed_once(SPAWN_GRAVITOR_INPUT))
        {
            cursorWorldPosition = cursor_to_world_position(inputHandler.cursor_position(), modelScale, viewOffset, windowDimensions);
            create_gravitor(hostGravitorBuffer, gravitorCount, cursorWorldPosition, 1.0f);
            glBindVertexArray(GRAV_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, GRAV_VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, gravitorCount * sizeof(Gravitor), hostGravitorBuffer.data());
            error = clEnqueueWriteBuffer(commandQueue, clGravitorBuffer, CL_TRUE, 0, gravitorCount * sizeof(Gravitor), hostGravitorBuffer.data(), 0, nullptr, nullptr);
        }
        if (inputHandler.is_button_pressed_once(SPAWN_REPULSOR_INPUT))
        {
            cursorWorldPosition = cursor_to_world_position(inputHandler.cursor_position(), modelScale, viewOffset, windowDimensions);
            create_gravitor(hostGravitorBuffer, gravitorCount, cursorWorldPosition, -1.0f);
            glBindVertexArray(GRAV_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, GRAV_VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, gravitorCount * sizeof(Gravitor), hostGravitorBuffer.data());
            error = clEnqueueWriteBuffer(commandQueue, clGravitorBuffer, CL_TRUE, 0, gravitorCount * sizeof(Gravitor), hostGravitorBuffer.data(), 0, nullptr, nullptr);
        }
        if (inputHandler.is_key_pressed_once(TOGGLE_MOVEMENT_INPUT)) calculateMovement = !calculateMovement;
        if (inputHandler.is_key_pressed_once(TOGGLE_GRAVITY_INPUT)) calculateGravity = !calculateGravity;
        if (inputHandler.is_key_pressed_once(INCREASE_POINT_SIZE_INPUT))
        {
            ++particleSize;
        }
        if (inputHandler.is_key_pressed_once(DECREASE_POINT_SIZE_INPUT))
        {
            --particleSize;
            if (particleSize == 0) particleSize = 1;
        }
        if (inputHandler.scroll_direction().y)
        {
            scrollValue = inputHandler.scroll_direction();

            bool hueshiftKey = inputHandler.is_key_pressed(CHANGE_HUE_INPUT);
            bool saturationshiftKey = inputHandler.is_key_pressed(CHANGE_SATURATION_INPUT);
            bool valueshiftKey = inputHandler.is_key_pressed(CHANGE_VALUE_INPUT);

            if (hueshiftKey)
            {
                hsv.x += scrollValue.y * deltaTime;
                if (hsv.x > 1.0f) hsv.x = 0.0f;
                if (hsv.x < 0.0f) hsv.x = 1.0f;
            }
            else if (saturationshiftKey)
            {
                hsv.y += scrollValue.y * deltaTime;
                if (hsv.y > 1.0f) hsv.y = 1.0f;
                if (hsv.y < 0.0f) hsv.y = 0.0f;
            }
            else if (valueshiftKey)
            {
                hsv.z += scrollValue.y * deltaTime;
                if (hsv.z > 1.0f) hsv.z = 1.0f;
                if (hsv.z < 0.0f) hsv.z = 0.0f;
            }
            else
            {
                adjust_model_scale(inputHandler.scroll_direction().y / 10.0f, modelScale);
            }
        }

        if (calculateEnergy)
        {
            size_t count = 0;
            for (auto& emitter : emitters) count += emitter->gen_count(deltaTime);

            Particle* particles = (Particle*)clEnqueueMapBuffer(commandQueue, clParticleBuffer, CL_TRUE, CL_MEM_READ_WRITE, 0, (particleCount + count) * sizeof(Particle), 0, nullptr, nullptr, &error);
            remove_dead_particles(particles, particleCount);
            for (auto emitter : emitters)
            {
                emitter->bind(particles);
                emitter->update(particleCount, deltaTime);
            }
            error = clEnqueueWriteBuffer(commandQueue, clParticleBuffer, CL_TRUE, 0, (particleCount) * sizeof(Particle), particles, 0, nullptr, nullptr);
            error = clEnqueueUnmapMemObject(commandQueue, clParticleBuffer, particles, 0, nullptr, nullptr);

            globalWorkSize[1] = (particleCount / globalWorkSize[0]) + 1;

            error = clSetKernelArg(kernelEN, 1, sizeof(int), &particleCount);
            error = clSetKernelArg(kernelEN, 2, sizeof(float), &deltaTime);
            error = clEnqueueNDRangeKernel(commandQueue, kernelEN, 2, nullptr, globalWorkSize, nullptr, 0, nullptr, nullptr);
        }
        if (calculateGravity && calculateMovement)
        {
#if DEBUG
            error = clEnqueueNDRangeKernel(commandQueue, kernelGVS, 2, nullptr, &globalWorkSize, nullptr, 0, nullptr, &profiler);
            clWaitForEvents(1, &profiler);
            cl_ulong start = 0, end = 0;
            clGetEventProfilingInfo(profiler, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
            clGetEventProfilingInfo(profiler, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
            std::cout << "Gravity kernel time: " << end - start << '\n';
#else
            error = clSetKernelArg(kernelGV, 1, sizeof(int), &particleCount);
            error = clSetKernelArg(kernelGV, 3, sizeof(int), &gravitorCount);
            error = clSetKernelArg(kernelGV, 4, sizeof(float), &deltaTime);
            error = clEnqueueNDRangeKernel(commandQueue, kernelGV, 2, nullptr, globalWorkSize, nullptr, 0, nullptr, nullptr);
#endif
        }
        if (calculateMovement)
        {
#if DEBUG
            error = clEnqueueNDRangeKernel(commandQueue, kernelMVS, 2, nullptr, &globalWorkSize, nullptr, 0, nullptr, &profiler);
            clWaitForEvents(1, &profiler);
            cl_ulong start = 0, end = 0;
            clGetEventProfilingInfo(profiler, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
            clGetEventProfilingInfo(profiler, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
            std::cout << "Movement kernel time: " << end - start << '\n';
#else
            error = clSetKernelArg(kernelMV, 1, sizeof(int), &particleCount);
            error = clSetKernelArg(kernelMV, 2, sizeof(float), &deltaTime);
            error = clEnqueueNDRangeKernel(commandQueue, kernelMV, 2, nullptr, globalWorkSize, nullptr, 0, nullptr, nullptr);
#endif
        }

        error = clFinish(commandQueue);
        error = clEnqueueReleaseGLObjects(commandQueue, 1, &clParticleBuffer, 0, nullptr, nullptr);



        //Draw gravitors
        gravitorShader.use();
        gravitorShader.setMat4("model", glm::scale(glm::mat4(1.0f), glm::vec3(modelScale)));
        gravitorShader.setMat4("view", glm::translate(glm::mat4(1.0f), viewOffset));

        glPointSize(6);
        glBindVertexArray(GRAV_VAO);
        glDrawArrays(GL_POINTS, 0, (GLsizei)gravitorCount);



        //Draw particles
        particleShader.use();
        particleShader.setMat4("model", glm::scale(glm::mat4(1.0f), glm::vec3(modelScale)));
        particleShader.setMat4("view", glm::translate(glm::mat4(1.0f), viewOffset));
        particleShader.setVec2("screenSize", windowDimensions);
        particleShader.setFloat("modelScale", modelScale);
        particleShader.setVec3("hsv", hsv);

        glPointSize((GLfloat)particleSize);
        glBindVertexArray(PART_VAO);
        glDrawArrays(GL_POINTS, 0, (GLsizei)particleCount);



        glFinish();
        glfwSwapBuffers(window);

        t1 = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(t1 - t0).count() / 1000.0f;
        
        ++updateCount;
        if (t1 > endTime) break;
    }



    auto now = std::chrono::high_resolution_clock::now();
    float timeElapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(now - startTime).count() / 1000.0f;

    system("cls");
    std::cout << "  RESULTS\n" << "  ------------------------\n";
    std::cout << std::setw(24) << std::left << "| INITIAL PARTICLES" << initialParticles << '\n';
    std::cout << std::setw(24) << std::left << "| UPDATES" << updateCount << '\n';
    std::cout << std::setw(24) << std::left << "| TIME ELAPSED" << timeElapsed << '\n';
    std::cout << std::setw(24) << std::left << "| UPDATES/SECOND" << updateCount / timeElapsed << '\n';



    clReleaseMemObject(clGravitorBuffer);

    clReleaseKernel(kernelMV);
    clReleaseKernel(kernelGV);
    clReleaseKernel(kernelEN);
    clReleaseProgram(program);

    clReleaseCommandQueue(commandQueue);
    clReleaseContext(context);
    
    glfwTerminate();

    return 0;
}