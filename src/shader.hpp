#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    Shader(const char* vsPath, const char* fsPath)
    {
        std::string vShaderCode;
        std::string fShaderCode;
        std::ifstream vsFile;
        std::ifstream fsFile;
        vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            vsFile.open(vsPath);
            fsFile.open(fsPath);
            std::stringstream vsStream;
            std::stringstream fsStream;
            vsStream << vsFile.rdbuf();
            fsStream << fsFile.rdbuf();
            vsFile.close();
            fsFile.close();
            vShaderCode = vsStream.str();
            fShaderCode = fsStream.str();
        }
        catch(std::ifstream::failure e)
        {
            std::cout << "ERROR::" << ID << "::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* vsCode = vShaderCode.c_str();
        const char* fsCode = fShaderCode.c_str();

        unsigned int vs;
        vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vsCode, NULL);
        glCompileShader(vs);
#if DEBUG
        int vsSuccess;
        char vsInfoLog[512];
        glGetShaderiv(vs, GL_COMPILE_STATUS, &vsSuccess);
        if (!vsSuccess)
        {
            glGetShaderInfoLog(vs, 512, NULL, vsInfoLog);
            std::cout << "ERROR::" << ID << "::SHADER::VERTEX::COMPILATION_FAILED\n" << vsInfoLog << std::endl;
        }
#endif

        unsigned int fs;

        fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fsCode, NULL);
        glCompileShader(fs);
#if DEBUG
        int fsSuccess;
        char fsInfoLog[512];
        glGetShaderiv(fs, GL_COMPILE_STATUS, &fsSuccess);
        if (!fsSuccess)
        {
            glGetShaderInfoLog(fs, 512, NULL, fsInfoLog);
            std::cout << "ERROR::" << ID << "::SHADER::FRAGMENT::COMPILATION_FAILED\n" << fsInfoLog << std::endl;
        }
#endif

        ID = glCreateProgram();
        glAttachShader(ID, vs);
        glAttachShader(ID, fs);
        glLinkProgram(ID);
#if DEBUG
        int programSuccess;
        char programInfoLog[512];
        glGetProgramiv(ID, GL_LINK_STATUS, &programSuccess);
        if (!programSuccess)
        {
            glGetProgramInfoLog(ID, 512, NULL, programInfoLog);
            std::cout << "ERROR::" << ID << "::SHADER::PROGRAM::LINKING_FAILED\n" << programInfoLog << std::endl;
        }
#endif

        glDeleteShader(fs);
        glDeleteShader(vs);
    }
    
    unsigned int ID;
    void Use() 
    {
        glUseProgram(ID);
    };
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), (float)value);
    }
    void setMat4(const std::string &name, glm::mat4 value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }
    void setVec(const std::string &name, glm::vec4 value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
    }
    void setVec(const std::string &name, glm::vec3 value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
    }
};