//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <OpenGLHelpers/OpenGLMacros.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __Banana_Qt__
class Shader : public OpenGLFunctions
{
public:
    Shader() : m_isProgramLinked(false)
    {
        initializeOpenGLFunctions();
    }
#else
class Shader
{
public:
    Shader() : m_isProgramLinked(false)
    {}
#endif

    bool isValid();

    void addVertexShader(const GLchar* shaderSource);
    void addGeometryShader(const GLchar* shaderSource);
    void addFragmentShader(const GLchar* shaderSource);

    void addVertexShaderFromFile(const char* fileName);
    void addGeometryShaderFromFile(const char* fileName);
    void addFragmentShaderFromFile(const char* fileName);

    void link();

    void bind()
    {
        glCall(glUseProgram(programID));
    }
    void release()
    {
        glCall(glUseProgram(0));
    }

    GLint getAtributeLocation(const char* atributeName);
    GLint getUniformLocation(const char* uniformName);

    void setUniformValue(const char* uniformName, glm::mat4 mat);
    void setUniformValue(const char* uniformName, glm::vec4 vec);
    void setUniformValue(const char* uniformName, glm::vec3 vec);
    void setUniformValue(const char* uniformName, glm::vec2 vec);
    void setUniformValue(const char* uniformName, GLfloat value);
    void setUniformValue(const char* uniformName, GLint value);
    void setUniformValue(const char* uniformName, GLuint value);
    void setUniformValue(const char* uniformName, GLboolean value);

    ////////////////////////////////////////////////////////////////////////////////
    GLuint programID;

protected:
    void addShader(GLenum shaderType, const GLchar* shaderSource);
    bool checkCompileError(GLuint shader, GLenum shaderType);
    bool checkLinkError(GLuint program);

    void loadFile(std::string& fileContent, const char* fileName);

    ////////////////////////////////////////////////////////////////////////////////
    std::vector<GLuint> m_ShaderIDs;
    bool                m_isProgramLinked;
};