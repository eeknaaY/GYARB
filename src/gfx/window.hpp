#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GLFW/glfw3.h>

GLFWwindow* createWindow(int SCR_WIDTH, int SCR_HEIGHT);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

#endif