#include "OpenGL.hpp"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

GLFWwindow* setup_gl(glm::uvec2 windowDimensions)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(windowDimensions.x, windowDimensions.y, "Particles", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(1);
    }

    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Other blending options. Does not work with every fragment shader option, so you need to mix & match a bit
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    //glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
    //glBlendFunc(GL_SRC_COLOR, GL_DST_ALPHA);
    //glBlendFunc(GL_SRC_COLOR, GL_ONE);
    //glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
    
    //glBlendFuncSeparate(GL_ONE, GL_ONE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSwapInterval(0);

    return window;
}
