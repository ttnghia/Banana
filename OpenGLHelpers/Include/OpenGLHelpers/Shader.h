//------------------------------------------------------------------------------------------
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 10/15/2016
//            `)  ('                                                    Author: Nghia Truong
//         ,  /::::\  ,
//         |'.\::::/.'|
//        _|  ';::;'  |_
//       (::)   ||   (::)                       _.
//        "|    ||    |"                      _(:)
//         '.   ||   .'                       /::\
//           '._||_.'                         \::/
//            /::::\                         /:::\
//            \::::/                        _\:::/
//             /::::\_.._  _.._  _.._  _.._/::::\
//             \::::/::::\/::::\/::::\/::::\::::/
//               `""`\::::/\::::/\::::/\::::/`""`
//                    `""`  `""`  `""`  `""`
//------------------------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#if defined(QT_GUI_LIB) || defined(QT_CORE_LIB)
#include <QtAppHelpers/QtAppMacros.h>

//------------------------------------------------------------------------------------------
class Shader : public OpenGLFunctions
{
public:
    Shader() : isLink(false)
    {
        initializeOpenGLFunctions();
    }
#else
#define GLEW_STATIC
#include <GL/glew.h>

//------------------------------------------------------------------------------------------
class Shader
{
public:
    Shader() : isLink(false)
    {}
#endif



    bool isValid()
    {
        return isLink;
    }

    ////////////////////////////////////////////////////////////////////////////////
    void addVertexShader(const GLchar* shaderSource)
    {
        addShader(GL_VERTEX_SHADER, shaderSource);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void addGeometryShader(const GLchar* shaderSource)
    {
        addShader(GL_GEOMETRY_SHADER, shaderSource);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void addFragmentShader(const GLchar* shaderSource)
    {
        addShader(GL_FRAGMENT_SHADER, shaderSource);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void addVertexShaderFromFile(const char* fileName)
    {
        std::string shaderSouce;
        loadFile(shaderSouce, fileName);

        addVertexShader(shaderSouce.c_str());
    }

    ////////////////////////////////////////////////////////////////////////////////
    void addGeometryShaderFromFile(const char* fileName)
    {
        std::string shaderSouce;
        loadFile(shaderSouce, fileName);

        addGeometryShader(shaderSouce.c_str());
    }

    ////////////////////////////////////////////////////////////////////////////////
    void addFragmentShaderFromFile(const char* fileName)
    {
        std::string shaderSouce;
        loadFile(shaderSouce, fileName);

        addFragmentShader(shaderSouce.c_str());
    }

    ////////////////////////////////////////////////////////////////////////////////
    void link()
    {
        programID = glCreateProgram();

        for(GLuint shader : shaders)
        {
            glAttachShader(programID, shader);
        }

        glLinkProgram(programID);

        if(checkLinkError(programID))
        {
            isLink = true;
        }

        for(GLuint shader : shaders)
        {
            glDeleteShader(shader);
        }

        shaders.clear();
    }

    ////////////////////////////////////////////////////////////////////////////////
    void bind()
    {
        glUseProgram(programID);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void release()
    {
        glUseProgram(0);
    }

    ////////////////////////////////////////////////////////////////////////////////
    GLint getAtributeLocation(const char* atributeName)
    {
        GLint location = glGetAttribLocation(programID, atributeName);

        if(location < 0)
        {
            fprintf(stderr, "Attribute %s not found!\n", atributeName);
            exit(EXIT_FAILURE);
        }

        return location;
    }

    ////////////////////////////////////////////////////////////////////////////////
    GLint getUniformLocation(const char* uniformName)
    {
        GLint location = glGetUniformLocation(programID, uniformName);

        if(location < 0)
        {
            fprintf(stderr, "Uniform location %s not found!\n", uniformName);
            exit(EXIT_FAILURE);
        }

        return location;
    }

    ////////////////////////////////////////////////////////////////////////////////
    void setUniformValue(const char* uniformName, glm::mat4 mat)
    {
        GLint location = getUniformLocation(uniformName);
        glUniformMatrix4fv(location,
                           1 /*only setting 1 matrix*/,
                           false /*transpose?*/,
                           glm::value_ptr(mat));
    }

    ////////////////////////////////////////////////////////////////////////////////
    void setUniformValue(const char* uniformName, glm::vec4 vec)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform4fv(location, 1, glm::value_ptr(vec));
    }

    ////////////////////////////////////////////////////////////////////////////////
    void setUniformValue(const char* uniformName, glm::vec3 vec)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform3fv(location, 1, glm::value_ptr(vec));
    }

    ////////////////////////////////////////////////////////////////////////////////
    void setUniformValue(const char* uniformName, glm::vec2 vec)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform2fv(location, 1, glm::value_ptr(vec));
    }

    ////////////////////////////////////////////////////////////////////////////////
    void setUniformValue(const char* uniformName, GLfloat value)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform1f(location, value);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void setUniformValue(const char* uniformName, GLint value)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform1i(location, value);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void setUniformValue(const char* uniformName, GLuint value)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform1ui(location, value);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void setUniformValue(const char* uniformName, GLboolean value)
    {
        GLint location = getUniformLocation(uniformName);
        glUniform1i(location, value ? GL_TRUE : GL_FALSE);
    }

    ////////////////////////////////////////////////////////////////////////////////

    GLuint programID;

protected:
    ////////////////////////////////////////////////////////////////////////////////
    void addShader(GLenum shaderType, const GLchar* shaderSource)
    {
        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &shaderSource, NULL);
        glCompileShader(shader);

        // => add shader into list
        if(checkCompileError(shader, shaderType))
        {
            shaders.push_back(shader);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    bool checkCompileError(GLuint shader, GLenum shaderType)
    {
        GLint success;
        GLchar infoLog[1024];

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if(!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);

            std::string shaderName = "Unknown";

            if(shaderType == GL_VERTEX_SHADER)
            {
                shaderName = "VertexShader";
            }
            else if(shaderType == GL_GEOMETRY_SHADER)
            {
                shaderName = "GeometryShader";
            }
            else if(shaderType == GL_FRAGMENT_SHADER)
            {
                shaderName = "FragmentShader";
            }
            else
            {
                shaderName = "InvalidShader";
            }


            std::cout << "ERROR: " << shaderName << " failed to compile!" << std::endl
                << "  => : " << infoLog << std::endl;
        }

        return static_cast<bool>(success);
    }

    ////////////////////////////////////////////////////////////////////////////////
    bool checkLinkError(GLuint program)
    {
        GLint success;
        GLchar infoLog[1024];
        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if(!success)
        {
            glGetProgramInfoLog(program, 1024, NULL, infoLog);
            std::cout << "ERROR: Program failed to link!" << std::endl
                << "  => : " << infoLog << std::endl;
        }

        return static_cast<bool>(success);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void loadFile(std::string& fileContent, const char* fileName)
    {
        std::ifstream file(fileName);
        if(!file.is_open())
        {
            printf("Error: Cannot open file for reading!\n");
            exit(EXIT_FAILURE);
        }

        // => allocate memory up front
        file.seekg(0, std::ios::end);
        fileContent.reserve(file.tellg());
        file.seekg(0, std::ios::beg);

        // => read file
        fileContent.assign((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
    }

    ////////////////////////////////////////////////////////////////////////////////


    std::vector<GLuint> shaders;
    bool isLink;
};