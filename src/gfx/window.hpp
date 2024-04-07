#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GLFW/glfw3.h>

GLFWwindow* createWindow(int SCR_WIDTH, int SCR_HEIGHT);
bool window_input_getkey(GLFWwindow* window, int KEYBIND);

#endif