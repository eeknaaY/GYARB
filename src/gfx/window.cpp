#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

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
    glfwSwapInterval(0);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    return window;
}

bool gl_LineEnabled;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
        if(gl_LineEnabled){
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            gl_LineEnabled = false;
        }

        else{
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            gl_LineEnabled = true;
        }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Update screen dimensions, dimensions might be fucked on retina displays?
    glViewport(0, 0, width, height);
}

