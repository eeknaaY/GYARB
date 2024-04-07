#include <glad/glad.h>
#include "src/gfx/window.hpp"
#include "src/gfx/voxel.hpp"
#include "src/shaders/shaders.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main(){
    GLFWwindow* window = createWindow(800, 600);

    Shader testShader("src/shaders/vertexshader.vs", "src/shaders/fragmentshader.fs");

    Voxel cube = Voxel(0, 0, 0, 0);
    glm::vec4 position = cube.position;
    std::cout << position.x;

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VoxelVertices), VoxelVertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float testInt = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        if(window_input_getkey(window, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 trans = glm::mat4(1.0f);

        if(window_input_getkey(window, GLFW_KEY_D)){
            testInt += 1.0f;
        }

        if(window_input_getkey(window, GLFW_KEY_A)){
            testInt -= 1.0f;
        }

        trans = glm::rotate(trans, glm::radians(testInt), glm::vec3(0.0, 1.0, 0.0));
        trans = glm::scale(trans, glm::vec3(1.0, 1.0, 1.0));  

        testShader.use();
        unsigned int transformLoc = glGetUniformLocation(testShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}