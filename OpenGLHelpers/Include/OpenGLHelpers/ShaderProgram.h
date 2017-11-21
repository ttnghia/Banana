//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
//    /                    Created: 2017 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
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
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ShaderProgram : public OpenGLCallable
{
public:
    ShaderProgram() : m_ProgramName("NoName"), m_isProgramLinked(false) {}
    ShaderProgram(const String& programName) : m_ProgramName(programName), m_isProgramLinked(false) {}
    ShaderProgram(const char* vsFile, const char* fsFile, const char* programName) : m_ProgramName(programName), m_isProgramLinked(false)
    {
        addVertexShaderFromFile(vsFile);
        addFragmentShaderFromFile(fsFile);
        link();
    }

    bool isValid() const { return m_isProgramLinked; }

    void addVertexShaderFromSource(const GLchar* shaderSource);
    void addGeometryShaderFromSource(const GLchar* shaderSource);
    void addFragmentShaderFromSource(const GLchar* shaderSource);

    void addVertexShaderFromFile(const char* fileName);
    void addGeometryShaderFromFile(const char* fileName);
    void addFragmentShaderFromFile(const char* fileName);

    bool link();
    bool reloadShaders();
    void clearCachedSource();

    void bind() { glCall(glUseProgram(m_ProgramID)); }
    void release() { glCall(glUseProgram(0)); }

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

    void setUniformValue(GLint location, const Mat4x4f& mat);
    void setUniformValue(GLint location, const Vec4f& vec);
    void setUniformValue(GLint location, const Vec3f& vec);
    void setUniformValue(GLint location, const Vec2f& vec);
    void setUniformValue(GLint location, GLfloat value);
    void setUniformValue(GLint location, GLint value);
    void setUniformValue(GLint location, GLuint value);
    void setUniformValue(GLint location, GLboolean value);

    ////////////////////////////////////////////////////////////////////////////////
    GLuint getProgramID() const { return m_ProgramID; }
    String getProgramName() const { return m_ProgramName; }

protected:
    bool addShader(GLenum shaderType, const GLchar* shaderSource);
    bool checkCompileError(GLuint shader, GLenum shaderType);
    bool checkLinkError(GLuint program);

    void loadFile(String& fileContent, const char* fileName);

    ////////////////////////////////////////////////////////////////////////////////
    bool                m_isProgramLinked;
    GLuint              m_ProgramID;
    String              m_ProgramName;
    Vector<GLuint>      m_ShaderIDs;
    Map<GLenum, String> m_ShaderSourceCodes;
    Map<GLenum, String> m_ShaderSourceFiles;

public:
    static SharedPtr<ShaderProgram> getSimpleVertexColorShader(String programName = String("SimpleVertexColorShader"));
    static SharedPtr<ShaderProgram> getSimpleUniformColorShader(String programName = String("SimpleUniformColorShader"));
    static SharedPtr<ShaderProgram> getSimpleTextureShader(String programName = String("SimpleTextureShader"));
    static SharedPtr<ShaderProgram> getSimpleLightSpaceDepthShader(String programName = String("SimpleLightSpaceDepthShader"));
    static SharedPtr<ShaderProgram> getSimpleCameraSpaceDepthShader(String programName = String("SimpleCameraSpaceDepthShader"));
    static SharedPtr<ShaderProgram> getObjUniformColorShader(String programName = String("ObjUniformColorShader"));
    static SharedPtr<ShaderProgram> getObjTextureShader(String programName = String("ObjTextureShader"));
    static SharedPtr<ShaderProgram> getPhongShader(String programName = String("PhongShader"));
    static SharedPtr<ShaderProgram> getSkyBoxShader(String programName = String("SkyBoxShader"));
    static SharedPtr<ShaderProgram> getScreenQuadShader(const char* fragmentShaderSource, String programName = String("ScreenQuadShader"));
    static SharedPtr<ShaderProgram> getScreenQuadShaderFromFile(const char* fragmentShaderFile, String programName = String("ScreenQuadShader"));
    static SharedPtr<ShaderProgram> getCheckerboardBackgroundShader(String programName = String("CheckerboardShader"));
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana