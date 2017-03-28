//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <OpenGLHelpers/OpenGLMacros.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <memory>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ShaderProgram : public OpenGLCallable
{
public:
    ShaderProgram() : m_ProgramName("Noname"), m_isProgramLinked(false)
    {}

    ShaderProgram(std::string programName) : m_ProgramName(programName), m_isProgramLinked(false)
    {}

    ShaderProgram(const char* vsFile, const char* fsFile, const char* programName) : m_ProgramName(programName), m_isProgramLinked(false)
    {
        addVertexShaderFromFile(vsFile);
        addFragmentShaderFromFile(fsFile);
        link();
    }

    bool isValid() const
    {
        return m_isProgramLinked;
    }

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
    void   bindUniformBlock(GLuint blockIndex, GLuint bindingPoint);

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
    GLuint getProgramID() const
    {
        return m_ProgramID;
    }

    std::string getProgramName() const
    {
        return m_ProgramName;
    }

protected:
    bool addShader(GLenum shaderType, const GLchar* shaderSource);
    bool checkCompileError(GLuint shader, GLenum shaderType);
    bool checkLinkError(GLuint program);

    void loadFile(std::string& fileContent, const char* fileName);

    ////////////////////////////////////////////////////////////////////////////////
    bool                          m_isProgramLinked;
    GLuint                        m_ProgramID;
    std::string                   m_ProgramName;
    std::vector<GLuint>           m_ShaderIDs;
    std::map<GLenum, std::string> m_ShaderSourceCodes;
    std::map<GLenum, std::string> m_ShaderSourceFiles;

public:
    static std::shared_ptr<ShaderProgram> getSimpleVertexColorShader(std::string programName = std::string("SimpleVertexColorShader"));
    static std::shared_ptr<ShaderProgram> getSimpleUniformColorShader(std::string programName = std::string("SimpleUniformColorShader"));
    static std::shared_ptr<ShaderProgram> getSimpleTextureShader(std::string programName = std::string("SimpleTextureShader"));
    static std::shared_ptr<ShaderProgram> getSimpleLightSpaceDepthShader(std::string programName = std::string("SimpleLightSpaceDepthShader"));
    static std::shared_ptr<ShaderProgram> getSimpleCameraSpaceDepthShader(std::string programName = std::string("SimpleCameraSpaceDepthShader"));
    static std::shared_ptr<ShaderProgram> getObjUniformColorShader(std::string programName = std::string("ObjUniformColorShader"));
    static std::shared_ptr<ShaderProgram> getObjTextureShader(std::string programName = std::string("ObjTextureShader"));
    static std::shared_ptr<ShaderProgram> getPhongShader(std::string programName = std::string("PhongShader"));
    static std::shared_ptr<ShaderProgram> getSkyBoxShader(std::string programName = std::string("SkyBoxShader"));
    static std::shared_ptr<ShaderProgram> getScreenQuadShader(const char* fragmentShaderSource, std::string programName = std::string("ScreenQuadShader"));
    static std::shared_ptr<ShaderProgram> getScreenQuadShaderFromFile(const char* fragmentShaderFile, std::string programName = std::string("ScreenQuadShader"));
};