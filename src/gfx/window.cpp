#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

GLFWwindow* createWindow(int SCR_WIDTH, int SCR_HEIGHT){
    // Init
    glfwInit();
    // Disable double buffer to enable 144+ fps on windows, remember to use glFlush() instead of glfwSwapBuffers() in main loop.
    //glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GYARB", NULL, NULL);
    // Full screen
    //GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GYARB", glfwGetPrimaryMonitor(), NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, keyCallback);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

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
    glViewport(0, 0, width, height);
}

