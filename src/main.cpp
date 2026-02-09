#include <cstddef>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "shader.hpp"

#define SQUARES_PER_SIDE 100

void framebuffer_size_callback(GLFWwindow* window, int width, int size);
void mouse_callback(GLFWwindow* window, double xpos, double ypos); 
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset); 

void processInput(GLFWwindow* window);

void getPlaneVertices(glm::vec3 planeVertices[(SQUARES_PER_SIDE + 1) * (SQUARES_PER_SIDE + 1)]);
void getPlaneIndices(unsigned int planeIndices[SQUARES_PER_SIDE * SQUARES_PER_SIDE * 6]);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
int framebufferWidth, framebufferHeight;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
glm::vec3 cameraInitPos(0.0f, 1.0f, 5.0f);

Camera camera(cameraInitPos, cameraUp, SCR_WIDTH, SCR_HEIGHT);
glm::mat4 proj = glm::perspective(glm::radians(60.0f), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.0f);

int main() {

    // Window boilerplate
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "terrain-gen", NULL, NULL);
    
    if (window == NULL) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialise GLAD\n";
        return -1;
    }

    // GL config
    glEnable(GL_DEPTH_TEST);

    float vertices[18] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };

    unsigned int triangleVAO, triangleVBO;
    glGenVertexArrays(1, &triangleVAO);
    glGenBuffers(1, &triangleVBO);
    glBindVertexArray(triangleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glm::vec3 planeVertices[(SQUARES_PER_SIDE + 1) * (SQUARES_PER_SIDE + 1)];
    unsigned int planeIndices[SQUARES_PER_SIDE * SQUARES_PER_SIDE * 6];

    getPlaneVertices(planeVertices);
    getPlaneIndices(planeIndices);

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int planeEBO;
    glGenBuffers(1, &planeEBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    std::string buildPath = "/home/edthi/Projects/terrain-gen/build/";
    Shader testShader(buildPath, "test");
    testShader.use();

    glm::mat4 view = camera.GetViewMatrix();

    while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        view = camera.GetViewMatrix();

        glClearColor(0.2f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        testShader.use();
        testShader.setVec3("color", glm::vec3(0.5f, 0.5f, 0.2f));
        testShader.setMat4("projection", proj);
        testShader.setMat4("view", view);

        glBindVertexArray(planeVAO);
        glDrawElements(GL_TRIANGLES, SQUARES_PER_SIDE * SQUARES_PER_SIDE * 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    camera.ProcessMouse(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessScroll(yoffset);
}

void getPlaneVertices(glm::vec3 planeVertices[(SQUARES_PER_SIDE + 1) * (SQUARES_PER_SIDE + 1)]) {

    for (int i = 0; i < SQUARES_PER_SIDE + 1; i++) {
        for (int j = 0; j < SQUARES_PER_SIDE + 1; j++) {

            float scale = 0.125f;

            float xPos = (float(i) - float(SQUARES_PER_SIDE) / 2.0f) * scale;
            float zPos = (float(j) - float(SQUARES_PER_SIDE) / 2.0f) * scale;
            float yPos = xPos * xPos + zPos * zPos;

            planeVertices[i * (SQUARES_PER_SIDE + 1) + j] = glm::vec3(xPos, yPos, zPos);
        }
    }
}

void getPlaneIndices(unsigned int planeIndices[SQUARES_PER_SIDE * SQUARES_PER_SIDE * 6]) {
    for (unsigned int i = 0; i < SQUARES_PER_SIDE; i++) {
        for (unsigned int j = 0; j < SQUARES_PER_SIDE; j++) {

            unsigned int vertexIndex = i * (SQUARES_PER_SIDE + 1) + j;

            unsigned int index = (i * SQUARES_PER_SIDE + j) * 6;

            planeIndices[index + 0] = vertexIndex;
            planeIndices[index + 1] = vertexIndex + SQUARES_PER_SIDE + 1;
            planeIndices[index + 2] = vertexIndex + SQUARES_PER_SIDE + 2;

            planeIndices[index + 3] = planeIndices[index + 2];
            planeIndices[index + 4] = vertexIndex + 1;
            planeIndices[index + 5] = planeIndices[index + 0];
        }
    }
}
