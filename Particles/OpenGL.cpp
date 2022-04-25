#include "OpenGL.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
float MODELSCALE = 0.05f;

double mouseX = 0;
double mouseY = 0;

bool mousePressedOnce = false;
bool isMousePressed = false;
bool wasMousePressed = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        mousePressedOnce = true;
    }
}
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    mouseX = (xpos * 2 / SCREEN_WIDTH - 1.0f) / MODELSCALE;
    mouseY = (- ypos * 2 / SCREEN_HEIGHT + 1.0f) / MODELSCALE;
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    MODELSCALE += (float)yoffset / 100.0f;

    if (MODELSCALE < 0.001f) MODELSCALE = 0.001f;
    if (MODELSCALE > 1.0f) MODELSCALE = 1.0f;
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
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(1);
    }

    glfwSwapInterval(0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glPointSize(5);

    return window;
}

bool isMousePressedOnce()
{
    return mousePressedOnce;
}