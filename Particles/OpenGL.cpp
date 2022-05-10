#include "OpenGL.hpp"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

GLFWwindow* setup_gl(glm::uvec2 windowDimensions)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(windowDimensions.x, windowDimensions.y, "Particles", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(1);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    return window;
}
