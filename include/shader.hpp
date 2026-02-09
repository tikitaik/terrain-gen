#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Shader {

    public:
        unsigned int ID;

        Shader(std::string buildPath, const std::string shaderName);
        void use(); 
};
