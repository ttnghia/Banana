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
    void setVertices(const Vector<GLfloat>& vertices);
    void setVertices(void* vertexData, size_t dataSize);

    void setVertexNormal(const Vector<GLfloat>& normals);
    void setVertexNormal(void* normalData, size_t dataSize);
    void inverseVertexNormal();
    void clearVertexNormal();

    void setVertexTexCoord(const Vector<float>& texcoords);
    void setVertexTexCoord(void* texData, size_t dataSize);
    void scaleVertexTexCoord(GLfloat scaleX, GLfloat scaleY);
    void clearVertexTexCoord();

    void setVertexColor(const Vector<float>& vcolors);
    void setVertexColor(void* colorData, size_t dataSize);
    void generateRandomVertexColor();
    void clearVertexColor();

    void setElementIndex(const Vector<GLushort>& indices);
    void clearElementIndex();
    void setElementIndex(const Vector<GLuint>& indices);
    void clearElementIndexLong();

    ////////////////////////////////////////////////////////////////////////////////
    void setCullFaceMode(GLenum cullFaceMode);
    void draw();
    void uploadDataToGPU();

    void   clearData();
    bool   isEmpty() const;
    size_t getNumVertices() const;

    const Vector<GLushort>& getIndexList() const;
    const Vector<GLuint>&   getIndexListLong() const;
    const Vector<GLfloat>&  getVertices() const;
    const Vector<GLfloat>&  getVertexNormals() const;
    const Vector<GLfloat>&  getVertexTexCoords() const;
    const Vector<GLfloat>&  getVertexColors() const;

    const SharedPtr<OpenGLBuffer>& getIndexBuffer() const;
    const SharedPtr<OpenGLBuffer>& getVertexBuffer() const;
    const SharedPtr<OpenGLBuffer>& getNormalBuffer() const;
    const SharedPtr<OpenGLBuffer>& getTexCoordBuffer() const;
    const SharedPtr<OpenGLBuffer>& getVertexColorBuffer() const;

    bool hasVertexNormal() const;
    bool hasVertexTexCoord() const;
    bool hasVertexColor() const;
    bool hasIndexBuffer() const;

protected:
    void createBuffers();
    void clearBuffer();

    ////////////////////////////////////////////////////////////////////////////////
    SharedPtr<OpenGLBuffer> m_VertexBuffer;
    SharedPtr<OpenGLBuffer> m_NormalBuffer;
    SharedPtr<OpenGLBuffer> m_TexCoordBuffer;
    SharedPtr<OpenGLBuffer> m_VertexColorBuffer;
    SharedPtr<OpenGLBuffer> m_IndexBuffer;

    Vector<GLushort> m_IndexList;
    Vector<GLuint>   m_IndexListLong;
    Vector<GLfloat>  m_Vertices;
    Vector<GLfloat>  m_VertexNormals;
    Vector<GLfloat>  m_VertexTexCoords;
    Vector<GLfloat>  m_VertexColors;

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