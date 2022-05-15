#include "Source.hpp"

void create_gravitor(std::vector<Gravitor>& gravitors, size_t& gravitorCount, glm::vec3 position, float strength = 1.0f)
{
    static size_t indexToReplace = 0;

    gravitors[indexToReplace] = Gravitor(position, glm::vec3(0.0f), glm::vec4(1.0f), strength);
    
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
        generate_particles_st(particles.data(), particleCount, particlesToGenerate, _particle_generator_cube, settings);
        t1 = std::chrono::high_resolution_clock::now();
        singleDelta = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(t1 - t0).count() / 1000.0f;
        std::cout << "ST TIME: " << singleDelta << '\n';

        particleCount = 0;

        t0 = std::chrono::high_resolution_clock::now();
        generate_particles_mt(particles.data(), particleCount, particlesToGenerate, _particle_generator_cube, settings);
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

int main(int argc, char* argv[])
{
    cl_int error;								                                        //OpenCL error code return

    cl_uint numPlatforms;                                                               //Number of platforms present on the host. Value from 1 to MAX_PLATFORM_ENTRIES
    cl_platform_id platformId;                                                          //Selected platform id

    cl_uint numDevices;                                                                 //Number of devices present on the current platform. Value from 1 to MAX_DEVICE_ENTRIES
    cl_device_id deviceId;                                                              //Selected device id

    cl_context context;                                                                 //Device context
    cl_command_queue commandQueue;                                                      //Queue for device commands
    size_t globalWorkSize[3];                                                           //Amount of work dimensions the kernels are divided in

    cl_program program;                                                                 //Program that contains kernels
    cl_kernel kernelMV;                                                                 //KERNEL: Calculates particle movement
    cl_kernel kernelGV;                                                                 //KERNEL: Calculates particle gravity
    cl_kernel kernelEN;                                                                 //KERNEL: Calculates particle energy

    bool calculateMovement = true;                                                      //Should movement be calculated every update?
    bool calculateGravity = true;                                                       //Should gravity be calculated every update? Is only true is calculateMovement is true
    bool calculateEnergy = false;                                                       //Should energy be calculated every update?

    cl_mem clParticleBuffer;                                                            //Pointer to GPU allocated particle buffer
    size_t initialParticles = 1000000;                                                   //Amount of particles to generate at the start of the simulation
    size_t particleCount{};                                                             //Amount of particles => separate variable to prevent host/GPU buffer resizing

    std::vector<Gravitor> hostGravitorBuffer;                                           //Gravitor buffer of static size that remains on the host
    size_t initialGravitors = 1;
    cl_mem clGravitorBuffer;                                                            //Pointer to GPU allocated gravitor buffer
    size_t gravitorCount{};                                                             //Amount of gravitors = > separate variable to prevent host / GPU buffer resizing (kinda useless because GRAVITOR_BUFFER_SIZE is 8

    std::vector<Emitter*> emitters{};                                                   //Vector of particle emitters
    ParticleGenerator generator = _particle_generator_cube;                             //Generator function pointer for initial particles
    GeneratorSettings settings{};



    glm::vec2 windowDimensions{ 800.0f };                                               //Dimensions of viewport
    GLFWwindow* window = setup_gl(windowDimensions);                                    //OpenGL setup
    InputHandler inputHandler(window);                                                  //Class that handles input events

    Transform transform{};
    glm::vec2 rotation{ 0.0f };                                                         //Current rotation of the camera around the origin
    glm::vec3 cameraPosition{ 0.0f };                                                   //Position of the camera after rotation

    ShaderInput shaderInput{};
    Program particleShader("Shaders/particle.vs", nullptr, "Shaders/particle.fs");      //Particle shader
    Program gravitorShader("Shaders/gravitor.vs", nullptr, "Shaders/gravitor.fs");      //Gravitor shader

    const unsigned int inputBufferId = 0;
    const unsigned int transformBufferId = 1;                                           
    float zoom{ 1.0f };                                                                 //Zoom level



    glm::vec3 hsv{ 0.0f, 1.0f, 1.0f };                                                  //Hue Saturation Value shift of particle colors
    glm::vec4 backgroundColor{ 0.1f, 0.1f, 0.1f, 0.0f };                                //Color of the background
    bool hueShift = false;                                                              //Shift hue with time?
    int particleSize = 1;                                                               //Size of particles
    float scrollMultiplier = 10.0f;



    std::chrono::high_resolution_clock::time_point startTime;                           //Start time of the simulation, initialized before update loop
    std::chrono::high_resolution_clock::time_point endTime;                             //Time to elapse before stopping the simulation
    std::chrono::high_resolution_clock::time_point t0;                                  //Time point before loop executes
    std::chrono::high_resolution_clock::time_point t1;                                  //Time point after loop has executed
    std::vector<float> frameTimes{};                                                    //Vector of time taken per frame
    unsigned int updateCount{};                                                         //Total amount of updates the simulation has done
    float deltaTime{};                                                                  //Elapsed time between each update
    float totalTime{};
    float benchmarkDuration{};


    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "-p")
        {
            std::string val = argv[i + 1];
            int num = std::stoi(val);

            if (num != 0)
            {
                std::cout << "particle count = " << val << '\n';
                initialParticles = num;
            }

            ++i;
        }
        else if (arg == "-e")
        {
            std::cout << "energy enabled\n";
            calculateEnergy = true;

            std::string val = argv[i + 1];
            int num = std::stoi(val);

            if (num != 0)
            {
                std::cout << "particle energy: " << val << '\n';
                settings.energy_min = 0.0f;
                settings.energy_max = float(num);
            }

            ++i;
        }
        else if (arg == "-g")
        {
            std::string val = argv[i + 1];

            if (val.length() != 0)
            {
                if (val == "l")
                {
                    std::cout << "generator = line\n";
                    generator = _particle_generator_line;
                }
                else if (val == "c")
                {
                    std::cout << "generator = cube\n";
                    generator = _particle_generator_cube;
                }
                else if (val == "s")
                {
                    std::cout << "generator = sphere\n";
                    generator = _particle_generator_sphere;
                }
            }

            ++i;
        }
        else if (arg == "-b")
        {
            std::cout << "benchmark enabled\n";
            std::string val = argv[i + 1];
            int num = std::stoi(val);

            if (num != 0)
            {
                std::cout << "benchmark duration: " << val << "seconds\n";
                benchmarkDuration = (float)num;
            }

            ++i;
        }
        else
        {
            std::cout << "Invalid command line argument entered: " << arg << '\n';
        }
    }



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
    commandQueue = clCreateCommandQueue(context, deviceId, NULL, &error);

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
    clGravitorBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, GRAVITOR_BUFFER_SIZE * sizeof(Gravitor), nullptr, &error);
#ifndef MOUSE_GRAV
    create_gravitor(hostGravitorBuffer, gravitorCount, glm::vec3(0.0f), 0.1f);
    error = clEnqueueWriteBuffer(commandQueue, clGravitorBuffer, CL_TRUE, 0, sizeof(Gravitor), hostGravitorBuffer.data(), 0, nullptr, nullptr);
#endif // !MOUSE_GRAV

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



    //Create Shader input buffer
    unsigned int INPUT_UBO;
    glGenBuffers(1, &INPUT_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, INPUT_UBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, inputBufferId, INPUT_UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ShaderInput), nullptr, GL_DYNAMIC_DRAW);



    //Create shared uniform buffer
    unsigned int TRANS_UBO;
    glGenBuffers(1, &TRANS_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, TRANS_UBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, transformBufferId, TRANS_UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Transform), nullptr, GL_DYNAMIC_DRAW);





    //Create program and compile GPU kernels
    const char* kernelSource = Utils::read_file("Kernels/particles.cl");

    program = clCreateProgramWithSource(context, 1, &kernelSource, nullptr, &error);
    check_error(error, __LINE__);

    error = clBuildProgram(program, 1, &deviceId, nullptr, nullptr, nullptr);
    check_program_compile_error(error, program, deviceId, __LINE__);

    kernelMV = clCreateKernel(program, "calculate_movement_single", &error);
    check_kernel_compile_error(error, program, deviceId, __LINE__);
#ifdef ALT_GRAVITY
    kernelGV = clCreateKernel(program, "calculate_gravity_single_alt", &error);
#else
    kernelGV = clCreateKernel(program, "calculate_gravity_single", &error);
#endif // ALT_GRAVITY
    check_kernel_compile_error(error, program, deviceId, __LINE__);
    kernelEN = clCreateKernel(program, "calculate_energy_single", &error);
    check_kernel_compile_error(error, program, deviceId, __LINE__);

    error = clSetKernelArg(kernelMV, 0, sizeof(cl_mem), (void*)&clParticleBuffer);
    error = clSetKernelArg(kernelGV, 0, sizeof(cl_mem), (void*)&clParticleBuffer);
    error = clSetKernelArg(kernelGV, 2, sizeof(cl_mem), (void*)&clGravitorBuffer);
    error = clSetKernelArg(kernelEN, 0, sizeof(cl_mem), (void*)&clParticleBuffer);





    settings.color_min = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    settings.color_max = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    //Map GPU memory to a host pointer and generate an initial amount of particles
    std::srand((unsigned int)time((time_t*)nullptr));
    error = clEnqueueAcquireGLObjects(commandQueue, 1, &clParticleBuffer, 0, nullptr, nullptr);
    Particle* particles = (Particle*)clEnqueueMapBuffer(commandQueue, clParticleBuffer, CL_TRUE, CL_MEM_WRITE_ONLY, 0, initialParticles * sizeof(Particle), 0, nullptr, nullptr, &error);
    Emitter::GenerateOnce(particles, particleCount, initialParticles, generator, settings);
    clEnqueueUnmapMemObject(commandQueue, clParticleBuffer, particles, 0, nullptr, nullptr);
    error = clEnqueueReleaseGLObjects(commandQueue, 1, &clParticleBuffer, 0, nullptr, nullptr);



    glm::vec4 projectionDimensions{ -1.0f, 1.0f, -1.0f, 1.0f };
    transform.projection = glm::ortho(projectionDimensions.x, projectionDimensions.y, projectionDimensions.z, projectionDimensions.w, 0.01f, 10.0f);

    shaderInput.resolution = windowDimensions;

    startTime = std::chrono::high_resolution_clock::now();
    endTime = std::chrono::high_resolution_clock::now() + std::chrono::seconds((int)benchmarkDuration);

    while (!glfwWindowShouldClose(window))
    {
        t0 = std::chrono::high_resolution_clock::now();
        inputHandler.update();
        
        shaderInput.mouse = inputHandler.cursor_position();
        shaderInput.deltaTime = deltaTime;
        shaderInput.time = totalTime;

        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        glClear(GL_COLOR_BUFFER_BIT);



        error = clEnqueueAcquireGLObjects(commandQueue, 1, &clParticleBuffer, 0, nullptr, nullptr);

        if (inputHandler.is_key_pressed_once(CLOSE_WINDOW_KEY)) glfwSetWindowShouldClose(window, true);

        if (inputHandler.is_key_pressed(MOVE_UP_INPUT))
        {
            rotation.y += deltaTime;
            rotation.y = std::min(rotation.y, CL_M_PI_2_F - 0.001f);
        }
        if (inputHandler.is_key_pressed(MOVE_LEFT_INPUT))
        {
            rotation.x += deltaTime;
        }
        if (inputHandler.is_key_pressed(MOVE_DOWN_INPUT))
        {
            rotation.y -= deltaTime;
            rotation.y = std::max(rotation.y, -CL_M_PI_2_F + 0.001f);
        }
        if (inputHandler.is_key_pressed(MOVE_RIGHT_INPUT))
        {
            rotation.x -= deltaTime;
        }

        glm::mat4 rotationMatrix{ 1.0f };
        rotationMatrix = glm::rotate(rotationMatrix, rotation.x, glm::vec3(0.0f, 1.0f, 0.0f));
        rotationMatrix = glm::rotate(rotationMatrix, rotation.y, glm::vec3(1.0f, 0.0f, 0.0f));
        cameraPosition = glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

        transform.view = glm::lookAt(cameraPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        transform.view = glm::scale(transform.view, glm::vec3(zoom));

        if (hueShift)
        {
            hsv.x += deltaTime * 0.01f;
        }

#ifdef MOUSE_GRAV
        if (inputHandler.is_button_pressed(SPAWN_GRAVITOR_INPUT) || inputHandler.is_button_pressed(SPAWN_REPULSOR_INPUT))
        {
            glm::vec2 screenPosition = glm::vec2(inputHandler.cursor_position() / windowDimensions);
            glm::vec2 absolutePosition = glm::vec2(screenPosition.x, -screenPosition.y) * 2.0f + glm::vec2(projectionDimensions.x, projectionDimensions.w);
            glm::vec4 rotatedPosition = rotationMatrix * glm::vec4(absolutePosition / zoom, 0.0f, 0.0f);

            float gravity = 0.0f;
            if (inputHandler.is_button_pressed(SPAWN_GRAVITOR_INPUT)) gravity = 1.0f;
            if (inputHandler.is_button_pressed(SPAWN_REPULSOR_INPUT)) gravity = -1.0f;

            hostGravitorBuffer[0] = Gravitor(rotatedPosition, glm::vec3(0.0f), glm::vec4(1.0f), gravity);
            gravitorCount = 1;

            error = clEnqueueWriteBuffer(commandQueue, clGravitorBuffer, CL_TRUE, 0, gravitorCount * sizeof(Gravitor), hostGravitorBuffer.data(), 0, nullptr, nullptr);

            glBindVertexArray(GRAV_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, GRAV_VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, gravitorCount * sizeof(Gravitor), hostGravitorBuffer.data());
        }
        else
        {
            gravitorCount = 0;
        }
#else
        if (inputHandler.is_button_pressed_once(SPAWN_GRAVITOR_INPUT) || inputHandler.is_button_pressed_once(SPAWN_REPULSOR_INPUT))
        {
            glm::vec2 screenPosition = glm::vec2(inputHandler.cursor_position() / windowDimensions);
            glm::vec2 absolutePosition = glm::vec2(screenPosition.x, -screenPosition.y) * 2.0f + glm::vec2(projectionDimensions.x, projectionDimensions.w);
            glm::vec4 rotatedPosition = rotationMatrix * glm::vec4(absolutePosition / zoom, 0.0f, 0.0f);

            float gravity = 0.0f;
            if (inputHandler.is_button_pressed_once(SPAWN_GRAVITOR_INPUT)) gravity = 1.0f;
            if (inputHandler.is_button_pressed_once(SPAWN_REPULSOR_INPUT)) gravity = -1.0f;

            create_gravitor(hostGravitorBuffer, gravitorCount, glm::vec3(rotatedPosition), gravity);
            error = clEnqueueWriteBuffer(commandQueue, clGravitorBuffer, CL_TRUE, 0, gravitorCount * sizeof(Gravitor), hostGravitorBuffer.data(), 0, nullptr, nullptr);

            glBindVertexArray(GRAV_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, GRAV_VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, gravitorCount * sizeof(Gravitor), hostGravitorBuffer.data());
        }
#endif // MOUSE_GRAV


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
            float scrollValueY = inputHandler.scroll_direction().y;
            float scrollValueYMultiplied = scrollValueY * scrollMultiplier;

            if (inputHandler.is_key_pressed(CHANGE_HUE_INPUT))
            {
                hsv.x += scrollValueY * deltaTime;
                hsv.x = std::fmod(hsv.x, 1.0f);
            }
            else if (inputHandler.is_key_pressed(CHANGE_SATURATION_INPUT))
            {
                hsv.y += scrollValueY * deltaTime;
                hsv.y = std::clamp(hsv.y, -1.0f, 1.0f);
                std::cout << "S: " << hsv.y << '\n';
            }
            else if (inputHandler.is_key_pressed(CHANGE_VALUE_INPUT))
            {
                hsv.z += scrollValueY * deltaTime;
                hsv.z = std::clamp(hsv.z, -1.0f, 1.0f);
                std::cout << "V: " << hsv.z << '\n';
            }
            else if (inputHandler.is_key_pressed(CHANGE_BACKGROUND_R_INPUT))
            {
                backgroundColor.r += scrollValueYMultiplied * deltaTime;
                backgroundColor.r = std::min(backgroundColor.r, 1.0f);
                backgroundColor.r = std::max(backgroundColor.r, 0.0f);
            }
            else if (inputHandler.is_key_pressed(CHANGE_BACKGROUND_G_INPUT))
            {
                backgroundColor.g += scrollValueYMultiplied * deltaTime;
                backgroundColor.g = std::min(backgroundColor.g, 1.0f);
                backgroundColor.g = std::max(backgroundColor.g, 0.0f);
            }
            else if (inputHandler.is_key_pressed(CHANGE_BACKGROUND_B_INPUT))
            {
                backgroundColor.b += scrollValueYMultiplied * deltaTime;
                backgroundColor.b = std::min(backgroundColor.b, 1.0f);
                backgroundColor.b = std::max(backgroundColor.b, 0.0f);
            }
            else
            {
                zoom += scrollValueY * 0.1f;
                zoom = std::clamp(zoom, 0.1f, 100.0f);
            }

            particleShader.set_vec3("uHSV", hsv);
        }

        if (calculateEnergy)
        {
            size_t count = 0;

            Particle* particles = (Particle*)clEnqueueMapBuffer(commandQueue, clParticleBuffer, CL_TRUE, CL_MEM_READ_WRITE, 0, (particleCount + count) * sizeof(Particle), 0, nullptr, nullptr, &error);
            remove_dead_particles(particles, particleCount);
            error = clEnqueueWriteBuffer(commandQueue, clParticleBuffer, CL_TRUE, 0, (particleCount) * sizeof(Particle), particles, 0, nullptr, nullptr);
            error = clEnqueueUnmapMemObject(commandQueue, clParticleBuffer, particles, 0, nullptr, nullptr);

            globalWorkSize[1] = (particleCount / globalWorkSize[0]) + 1;

            error = clSetKernelArg(kernelEN, 1, sizeof(int), &particleCount);
            error = clSetKernelArg(kernelEN, 2, sizeof(float), &deltaTime);
            error = clEnqueueNDRangeKernel(commandQueue, kernelEN, 2, nullptr, globalWorkSize, nullptr, 0, nullptr, nullptr);
        }
        if (calculateGravity && calculateMovement)
        {
            error = clSetKernelArg(kernelGV, 1, sizeof(int), &particleCount);
            error = clSetKernelArg(kernelGV, 3, sizeof(int), &gravitorCount);
            error = clSetKernelArg(kernelGV, 4, sizeof(float), &deltaTime);
            error = clEnqueueNDRangeKernel(commandQueue, kernelGV, 2, nullptr, globalWorkSize, nullptr, 0, nullptr, nullptr);
        }
        if (calculateMovement)
        {
            error = clSetKernelArg(kernelMV, 1, sizeof(int), &particleCount);
            error = clSetKernelArg(kernelMV, 2, sizeof(float), &deltaTime);
            error = clEnqueueNDRangeKernel(commandQueue, kernelMV, 2, nullptr, globalWorkSize, nullptr, 0, nullptr, nullptr);
        }

        error = clFinish(commandQueue);
        error = clEnqueueReleaseGLObjects(commandQueue, 1, &clParticleBuffer, 0, nullptr, nullptr);





        //Global uniform buffer data
        glBindBuffer(GL_UNIFORM_BUFFER, INPUT_UBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ShaderInput), &shaderInput);

        glBindBuffer(GL_UNIFORM_BUFFER, TRANS_UBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Transform), &transform);



        //Draw gravitors
        gravitorShader.use();
        glPointSize(6);
        glBindVertexArray(GRAV_VAO);
        glDrawArrays(GL_POINTS, 0, (GLsizei)gravitorCount);



        //Draw particles
        particleShader.use();
        glPointSize((GLfloat)particleSize);
        glBindVertexArray(PART_VAO);
        glDrawArrays(GL_POINTS, 0, (GLsizei)particleCount);





        glfwSwapBuffers(window);

        t1 = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(t1 - t0).count() / 1000.0f;
        totalTime += deltaTime;
        
        ++updateCount;
        frameTimes.push_back(deltaTime);
        if (benchmarkDuration && t1 > endTime) break;
    }

    auto now = std::chrono::high_resolution_clock::now();
    float timeElapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(now - startTime).count() / 1000.0f;

    float averageFrameTime{};
    for (auto frameTime : frameTimes) averageFrameTime += frameTime;
    averageFrameTime /= frameTimes.size();
    averageFrameTime *= 1000.0f;

    system("cls");
    std::cout << "  RESULTS\n" << "  ------------------------\n";
    std::cout << std::setw(24) << std::left << "| INITIAL PARTICLES" << initialParticles << '\n';
    std::cout << std::setw(24) << std::left << "| UPDATES" << updateCount << '\n';
    std::cout << std::setw(24) << std::left << "| TIME ELAPSED" << timeElapsed << '\n';
    std::cout << std::setw(24) << std::left << "| UPDATES/SECOND" << updateCount / timeElapsed << '\n';
    std::cout << std::setw(24) << std::left << "| AVG FRAME TIME" << averageFrameTime << "ms" << '\n';



    clReleaseMemObject(clGravitorBuffer);

    clReleaseKernel(kernelMV);
    clReleaseKernel(kernelGV);
    clReleaseKernel(kernelEN);
    clReleaseProgram(program);

    clReleaseCommandQueue(commandQueue);
    clReleaseContext(context);
    
    glfwTerminate();



    std::cin.get();
    return 0;
}