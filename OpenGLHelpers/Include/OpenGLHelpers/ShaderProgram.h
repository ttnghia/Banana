//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

#include <OpenGLHelpers/OpenGLMacros.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ShaderProgram : public OpenGLCallable
{
public:
    ShaderProgram() :
        m_ProgramName("Noname"),
        m_isProgramLinked(false)
    {}

    ShaderProgram(std::string programName) :
        m_ProgramName(programName),
        m_isProgramLinked(false)
    {}

    ShaderProgram(const char* programName,
                  const char* vsFile, const char* fsFile) :
        m_ProgramName(programName),
        m_isProgramLinked(false)
    {
        addVertexShaderFromFile(vsFile);
        addFragmentShaderFromFile(fsFile);
        link();
    }

    bool isValid();

    void addVertexShaderFromSource(const GLchar* shaderSource);
    void addGeometryShaderFromSource(const GLchar* shaderSource);
    void addFragmentShaderFromSource(const GLchar* shaderSource);

    void addVertexShaderFromFile(const char* fileName);
    void addGeometryShaderFromFile(const char* fileName);
    void addFragmentShaderFromFile(const char* fileName);

    bool link();
    bool reloadShaders();
    void clearCachedSource();

    void bind()
    {
        glCall(glUseProgram(m_ProgramID));
    }
    void release()
    {
        glCall(glUseProgram(0));
    }

    GLuint getAtributeLocation(const char* atributeName, bool dieOnError = true);
    GLuint getUniformLocation(const char* uniformName, bool dieOnError = true);
    GLuint getUniformBlockIndex(const char* uniformBlockName, bool dieOnError = true);
    void bindUniformBlock(GLuint blockIndex, GLuint bindingPoint);

    template<class T>
    void setUniformValue(const char* uniformName, T value, bool dieOnError = true)
    {
        GLint location = getUniformLocation(uniformName, dieOnError);
        setUniformValue(location, value);
    }

    void setUniformValue(GLint location, const glm::mat4& mat);
    void setUniformValue(GLint location, const glm::vec4& vec);
    void setUniformValue(GLint location, const glm::vec3& vec);
    void setUniformValue(GLint location, const glm::vec2& vec);
    void setUniformValue(GLint location, GLfloat value);
    void setUniformValue(GLint location, GLint value);
    void setUniformValue(GLint location, GLuint value);
    void setUniformValue(GLint location, GLboolean value);

    ////////////////////////////////////////////////////////////////////////////////
    GLuint getProgramID()
    {
        return m_ProgramID;
    }

    std::string getProgramName()
    {
        return m_ProgramName;
    }

protected:
    bool addShader(GLenum shaderType, const GLchar* shaderSource);
    bool checkCompileError(GLuint shader, GLenum shaderType);
    bool checkLinkError(GLuint program);

    void loadFile(std::string& fileContent, const char* fileName);

    ////////////////////////////////////////////////////////////////////////////////
    GLuint                        m_ProgramID;
    std::string                   m_ProgramName;
    std::vector<GLuint>           m_ShaderIDs;
    bool                          m_isProgramLinked;
    std::map<GLenum, std::string> m_ShaderSourceCodes;
    std::map<GLenum, std::string> m_ShaderSourceFiles;

public:
    static ShaderProgram* getSimpleVertexColorShader(std::string programName = std::string("SimpleVertexColorShader"));
    static ShaderProgram* getSimpleUniformColorShader(std::string programName = std::string("SimpleUniformColorShader"));
    static ShaderProgram* getSimpleTextureShader(std::string programName = std::string("SimpleTextureShader"));
    static ShaderProgram* getObjUniformColorShader(std::string programName = std::string("ObjUniformColorShader"));
    static ShaderProgram* getObjTextureShader(std::string programName = std::string("ObjTextureShader"));
    static ShaderProgram* getPhongShader(std::string programName = std::string("PhongShader"));
    static ShaderProgram* getSkyBoxShader(std::string programName = std::string("SkyBoxShader"));
    static ShaderProgram* getScreenQuadShader(const char* fragmentShaderSource, std::string programName =std::string("ScreenQuadShader"));
    static ShaderProgram* getScreenQuadShaderFromFile(const char* fragmentShaderFile, std::string programName =std::string("ScreenQuadShader"));
};