#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "shader.hpp"

#define SCR_WIDTH 1280
#define SCR_HEIGHT 720

#define SQUARES_PER_SIDE 300
#define SCALE 16 / SQUARES_PER_SIDE

void processInput(GLFWwindow* window);
void renderScreenFBO(Shader screenShader, unsigned int textureToRender);

void getObjects();

void getPlaneIndices(unsigned int planeIndices[SQUARES_PER_SIDE * SQUARES_PER_SIDE * 6]);
void getPlaneTexCoords(glm::vec2 planeTexCoords[(SQUARES_PER_SIDE + 1) * (SQUARES_PER_SIDE + 1)]);
void getPlaneVertices(glm::vec3 planeVertices[(SQUARES_PER_SIDE + 1) * (SQUARES_PER_SIDE + 1)]);

std::string getBuildPath(std::string argv_0); 

void framebuffer_size_callback(GLFWwindow* window, int width, int size);
void mouse_callback(GLFWwindow* window, double xpos, double ypos); 
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset); 

int framebufferWidth, framebufferHeight;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
glm::vec3 cameraInitPos(0.0f, 3.0f, 0.0f);

Camera camera(cameraInitPos, cameraUp, SCR_WIDTH, SCR_HEIGHT);
glm::mat4 proj = glm::perspective(glm::radians(60.0f), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.0f);

unsigned int triangleVAO, triangleVBO;
unsigned int quadVAO, quadVBO;
unsigned int planeVAO, planeVertexVBO, planeTexCoordsVBO, planeEBO;
unsigned int noiseFBO, noiseTex;
unsigned int screenFBO, screenRBO, screenTexture;

void renderQuad() {
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

int main(int argc, char* argv[]) {

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

    const std::string buildPath = getBuildPath(argv[0]);

    getObjects();

    Shader noiseGenShader(buildPath, "noisegen");
    Shader screenShader(buildPath, "screen");
    Shader testShader(buildPath, "test");

    glm::vec2 posOffset      = glm::vec2(0.0f);
    glm::vec2 posOffsetDelta = glm::vec2(0.8f);

    screenShader.use();
    screenShader.setInt("tex", 0);

    glm::mat4 view = camera.GetViewMatrix();

    while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        view = camera.GetViewMatrix();
        
        glBindFramebuffer(GL_FRAMEBUFFER, noiseFBO);
        noiseGenShader.use();
        noiseGenShader.setVec2("posOffset", posOffset += posOffsetDelta * deltaTime);
        noiseGenShader.setFloat("timeOffset", 0);
        renderQuad();

        glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
        glClearColor(0.2f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, noiseTex);

        testShader.use();
        testShader.setMat4("projection", proj);
        testShader.setMat4("view", view);
        testShader.setInt("heightMap", 0);

        glBindVertexArray(planeVAO);
        glDrawElements(GL_TRIANGLES, SQUARES_PER_SIDE * SQUARES_PER_SIDE * 6, GL_UNSIGNED_INT, 0);

        renderScreenFBO(screenShader, noiseTex);
        renderScreenFBO(screenShader, screenTexture);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
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

void renderScreenFBO(Shader screenShader, unsigned int textureToRender) {

    int texWidth, texHeight;
    int miplevel = 0;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureToRender);

    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &texWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &texHeight);
    //glViewport(0, 0, texWidth, texHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.2f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    screenShader.use();
    renderQuad();
}

void getObjects() {

    // triangle //
    float vertices[18] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };

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

    // quad //

    float quadVertices[24] = {
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // plane //
    glm::vec3 planeVertices[(SQUARES_PER_SIDE + 1) * (SQUARES_PER_SIDE + 1)];
    glm::vec2 planeTexCoords[(SQUARES_PER_SIDE + 1) * (SQUARES_PER_SIDE + 1)];
    unsigned int planeIndices[SQUARES_PER_SIDE * SQUARES_PER_SIDE * 6];

    getPlaneVertices(planeVertices);
    getPlaneTexCoords(planeTexCoords);
    getPlaneIndices(planeIndices);

    glGenVertexArrays(1, &planeVAO);
    glBindVertexArray(planeVAO);

    glGenBuffers(1, &planeVertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVertexVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &planeTexCoordsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeTexCoordsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeTexCoords), planeTexCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &planeEBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);
    glBindVertexArray(0);

    glGenFramebuffers(1, &screenFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);

    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, framebufferWidth, framebufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &screenRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, screenRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, framebufferWidth, framebufferHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, screenRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Screen framebuffer is not complete!\n";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  

    glGenFramebuffers(1, &noiseFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, noiseFBO);

    glGenTextures(1, &noiseTex);
    glBindTexture(GL_TEXTURE_2D, noiseTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, noiseTex, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Noise framebuffer is not complete!\n";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void getPlaneTexCoords(glm::vec2 planeTexCoords[(SQUARES_PER_SIDE + 1) * (SQUARES_PER_SIDE + 1)]) {

    float ratio = 1 / float(SQUARES_PER_SIDE);

    for (int i = 0; i < SQUARES_PER_SIDE + 1; i++) {
        for (int j = 0; j < SQUARES_PER_SIDE + 1; j++) {
            planeTexCoords[i * (SQUARES_PER_SIDE + 1) + j] = glm::vec2(float(i) * ratio, float(j) * ratio);
        }
    }
}

void getPlaneVertices(glm::vec3 planeVertices[(SQUARES_PER_SIDE + 1) * (SQUARES_PER_SIDE + 1)]) {

    for (int i = 0; i < SQUARES_PER_SIDE + 1; i++) {
        for (int j = 0; j < SQUARES_PER_SIDE + 1; j++) {

            float xPos = (float(i) - float(SQUARES_PER_SIDE) / 2.0f) * SCALE;
            float zPos = (float(j) - float(SQUARES_PER_SIDE) / 2.0f) * SCALE;
            float yPos = 0.0f;

            planeVertices[i * (SQUARES_PER_SIDE + 1) + j] = glm::vec3(xPos, yPos, zPos);
        }
    }
}

std::string getBuildPath(std::string argv_0) {

    // hehehe this will let us find executable location
    std::string resPath = argv_0;
    std::string cwd = std::filesystem::current_path().string();

    std::string slash = "/";

# ifdef _WIN32
    slash = "\\";
# endif

    if (resPath.find(".") == 0) {
        resPath.erase(0,1);
    }

    size_t last_slash_pos = resPath.find_last_of(slash);

    if (last_slash_pos != std::string::npos) {
        resPath.erase(last_slash_pos);
    }

    size_t resPathContainsCWD = resPath.find(cwd);
    if (resPathContainsCWD != std::string::npos) {
            return resPath + slash;
    }

# ifdef _WIN32
    if (std::string(1, resPath[0]) != slash && std::string(1, cwd[cwd.length() - 1]) != slash) {
        cwd = cwd + slash;
    }
# endif

    std::string buildPath = cwd + resPath + slash;
    return buildPath;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    camera.ProcessMouse(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessScroll(yoffset);
}
