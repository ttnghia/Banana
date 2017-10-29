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
#include <OpenGLHelpers/OpenGLBuffer.h>

#include <vector>
#include <cassert>
#include <memory>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class MeshObject : public OpenGLCallable
{
public:
    MeshObject(GLenum dataTopology    = GL_TRIANGLES,
               bool   isMeshVeryLarge = false) :
        m_DataTopology(dataTopology),
        m_CullFaceMode(GL_NONE),
        m_isMeshVeryLarge(isMeshVeryLarge),
        m_isDataReady(false),
        m_isBufferCreated(false),
        m_isGLDataReady(false),
        m_hasVertexTexCoord(false),
        m_hasVertexNormal(false),
        m_hasVertexColor(false),
        m_isNoTransformation(true),
        m_hasIndexBuffer(false),
        m_ScaleX(1.0),
        m_ScaleY(1.0),
        m_ScaleZ(1.0),
        m_TranslateX(0.0),
        m_TranslateY(0.0),
        m_TranslateZ(0.0),
        m_NumVertices(0),
        m_VertexBuffer(nullptr),
        m_NormalBuffer(nullptr),
        m_TexCoordBuffer(nullptr),
        m_VertexColorBuffer(nullptr),
        m_IndexBuffer(nullptr) {}

    ~MeshObject();

    void transformObject(GLfloat scaleX = 1.0, GLfloat scaleY = 1.0, GLfloat scaleZ = 1.0, GLfloat translateX = 0.0, GLfloat translateY = 0.0, GLfloat translateZ = 0.0);
    void setVertices(const std::vector<GLfloat>& vertices);
    void setVertices(void* vertexData, size_t dataSize);

    void setVertexNormal(const std::vector<GLfloat>& normals);
    void setVertexNormal(void* normalData, size_t dataSize);
    void inverseVertexNormal();
    void clearVertexNormal();

    void setVertexTexCoord(const std::vector<float>& texcoords);
    void setVertexTexCoord(void* texData, size_t dataSize);
    void scaleVertexTexCoord(GLfloat scaleX, GLfloat scaleY);
    void clearVertexTexCoord();

    void setVertexColor(const std::vector<float>& vcolors);
    void setVertexColor(void* colorData, size_t dataSize);
    void generateRandomVertexColor();
    void clearVertexColor();

    void setElementIndex(const std::vector<GLushort>& indices);
    void clearElementIndex();
    void setElementIndex(const std::vector<GLuint>& indices);
    void clearElementIndexLong();

    ////////////////////////////////////////////////////////////////////////////////
    void setCullFaceMode(GLenum cullFaceMode);
    void draw();
    void uploadDataToGPU();

    void   clearData();
    bool   isEmpty() const;
    size_t getNumVertices() const;

    const std::vector<GLushort>& getIndexList() const;
    const std::vector<GLuint>&   getIndexListLong() const;
    const std::vector<GLfloat>&  getVertices() const;
    const std::vector<GLfloat>&  getVertexNormals() const;
    const std::vector<GLfloat>&  getVertexTexCoords() const;
    const std::vector<GLfloat>&  getVertexColors() const;

    const std::shared_ptr<OpenGLBuffer>& getIndexBuffer() const;
    const std::shared_ptr<OpenGLBuffer>& getVertexBuffer() const;
    const std::shared_ptr<OpenGLBuffer>& getNormalBuffer() const;
    const std::shared_ptr<OpenGLBuffer>& getTexCoordBuffer() const;
    const std::shared_ptr<OpenGLBuffer>& getVertexColorBuffer() const;

    bool hasVertexNormal() const;
    bool hasVertexTexCoord() const;
    bool hasVertexColor() const;
    bool hasIndexBuffer() const;

protected:
    void createBuffers();
    void clearBuffer();

    ////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<OpenGLBuffer> m_VertexBuffer;
    std::shared_ptr<OpenGLBuffer> m_NormalBuffer;
    std::shared_ptr<OpenGLBuffer> m_TexCoordBuffer;
    std::shared_ptr<OpenGLBuffer> m_VertexColorBuffer;
    std::shared_ptr<OpenGLBuffer> m_IndexBuffer;

    std::vector<GLushort> m_IndexList;
    std::vector<GLuint>   m_IndexListLong;
    std::vector<GLfloat>  m_Vertices;
    std::vector<GLfloat>  m_VertexNormals;
    std::vector<GLfloat>  m_VertexTexCoords;
    std::vector<GLfloat>  m_VertexColors;

    size_t m_NumVertices;
    GLenum m_DataTopology;
    GLenum m_CullFaceMode;

    bool m_isMeshVeryLarge;
    bool m_isNoTransformation;
    bool m_isDataReady;
    bool m_isBufferCreated;
    bool m_isGLDataReady;

    bool m_hasVertexNormal;
    bool m_hasVertexTexCoord;
    bool m_hasVertexColor;
    bool m_hasIndexBuffer;

    GLfloat m_ScaleX;
    GLfloat m_ScaleY;
    GLfloat m_ScaleZ;
    GLfloat m_TranslateX;
    GLfloat m_TranslateY;
    GLfloat m_TranslateZ;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana