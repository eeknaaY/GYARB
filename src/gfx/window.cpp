#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool window_input_getkey(GLFWwindow* window, int KEYBIND);

GLFWwindow* createWindow(int SCR_WIDTH, int SCR_HEIGHT){
    // Init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GYARB", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    return window;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Update screen dimensions, dimensions might be fucked on retina displays?
    glViewport(0, 0, width, height);
}

bool window_input_getkey(GLFWwindow* window, int KEYBIND){
    if(glfwGetKey(window, KEYBIND) == GLFW_PRESS)
        return true;
    else
        return false;
}