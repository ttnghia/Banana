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
#include <map>
#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <OpenGLHelpers/OpenGLMacros.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __Banana_Qt__
class ShaderProgram : public OpenGLFunctions
{
public:
    ShaderProgram() :
        m_ProgramName("Noname"),
        m_isProgramLinked(false)
    {
        initializeOpenGLFunctions();
    }

    ShaderProgram(const char* programName, const char* vsFile, const char* fsFile) :
        m_ProgramName(programName),
        m_isProgramLinked(false)
    {
        initializeOpenGLFunctions();

        addVertexShaderFromFile(vsFile);
        addFragmentShaderFromFile(fsFile);
        link();
    }
#else
class ShaderProgram
{
public:
    ShaderProgram() : m_isProgramLinked(false)
    {}

    ShaderProgram(const char* vsFile, const char* fsFile) : m_isProgramLinked(false)
    {
        addVertexShaderFromFile(vsFile);
        addFragmentShaderFromFile(fsFile);
        link();
    }
#endif

    bool isValid();

    void addVertexShader(const GLchar* shaderSource);
    void addGeometryShader(const GLchar* shaderSource);
    void addFragmentShader(const GLchar* shaderSource);

    void addVertexShaderFromFile(const char* fileName);
    void addGeometryShaderFromFile(const char* fileName);
    void addFragmentShaderFromFile(const char* fileName);

    bool link();
    bool reloadShaders();

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
    std::string                   m_ProgramName;
    std::vector<GLuint>           m_ShaderIDs;
    bool                          m_isProgramLinked;
    std::map<GLenum, std::string> m_ShaderSourceFiles;
};