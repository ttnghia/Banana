//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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

#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/ParallelHelpers/Scheduler.h>
#include <OpenGLHelpers/MeshObjects/MeshObject.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MeshObject::~MeshObject()
{
    clearData();
    clearBuffer();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::transform(const Vec3f& translation, const Vec3f& scale)
{
    m_Translation  = translation;
    m_Scale        = scale;
    m_bTranslation = true;
    m_bScale       = true;
    ////////////////////////////////////////////////////////////////////////////////
    if(m_isDataReady) {
        NumberHelpers::transform(m_Vertices, translation, scale);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::transform(const Vec3f& translation, const Vec3f& scale, const Vec3f& rotation)
{
    m_Translation  = translation;
    m_Scale        = scale;
    m_Rotation     = rotation;
    m_bTranslation = true;
    m_bScale       = true;
    m_bRotate      = true;
    ////////////////////////////////////////////////////////////////////////////////
    if(m_isDataReady) {
        NumberHelpers::transform(m_Vertices, translation, scale, rotation);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::translate(const Vec3f& translation)
{
    m_Translation  = translation;
    m_bTranslation = true;
    ////////////////////////////////////////////////////////////////////////////////
    if(m_isDataReady) {
        NumberHelpers::translate(m_Vertices, translation);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::scale(const Vec3f& scale)
{
    m_Scale  = scale;
    m_bScale = true;
    ////////////////////////////////////////////////////////////////////////////////
    if(m_isDataReady) {
        NumberHelpers::scale(m_Vertices, scale);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::rotate(const Vec3f& rotation)
{
    m_Rotation = rotation;
    m_bRotate  = true;
    ////////////////////////////////////////////////////////////////////////////////
    if(m_isDataReady) {
        NumberHelpers::rotate(m_Vertices, rotation);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertices(const Vector<Vec3f>& vertices)
{
    setVertices((void*)vertices.data(), vertices.size() * sizeof(Vec3f));
}

void MeshObject::setVertices(const Vector<GLfloat>& vertices)
{
    setVertices((void*)vertices.data(), vertices.size() * sizeof(GLfloat));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertices(void* vertexData, size_t dataSize)
{
    m_NVertices = dataSize / 3 / sizeof(GLfloat);
    __BNN_REQUIRE(m_NVertices * 3 * sizeof(GLfloat) == dataSize);
    m_Vertices.resize(m_NVertices);
    std::memcpy((void*)m_Vertices.data(), vertexData, dataSize);

    if(m_bTranslation && m_bScale && m_bRotate) {
        NumberHelpers::transform(m_Vertices, m_Translation, m_Scale, m_Rotation);
    } else if(m_bTranslation && m_bScale) {
        NumberHelpers::transform(m_Vertices, m_Translation, m_Scale);
    } else if(m_bTranslation) {
        NumberHelpers::translate(m_Vertices, m_Translation);
    }

    m_isDataReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertexNormal(const Vector<Vec3f>& normals)
{
    setVertexNormal((void*)normals.data(), normals.size() * sizeof(Vec3f));
}

void MeshObject::setVertexNormal(const Vector<GLfloat>& normals)
{
    setVertexNormal((void*)normals.data(), normals.size() * sizeof(GLfloat));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertexNormal(void* normalData, size_t dataSize)
{
    assert(dataSize == m_NVertices * 3 * sizeof(GLfloat));
    m_hasVertexNormal = true;

    m_VertexNormals.resize(m_NVertices);
    std::memcpy((void*)m_VertexNormals.data(), normalData, dataSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::inverseVertexNormal()
{
    if(!m_hasVertexNormal) {
        return;
    }
    Scheduler::parallel_for(m_NVertices, [&](size_t i) { m_VertexNormals[i] *= -1.0f; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::clearVertexNormal()
{
    m_VertexNormals.resize(0);
    m_hasVertexNormal = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertexTexCoord(const Vector<Vec2f>& texcoords)
{
    setVertexTexCoord((void*)texcoords.data(), texcoords.size() * sizeof(Vec2f));
}

void MeshObject::setVertexTexCoord(const Vector<GLfloat>& texcoords)
{
    setVertexTexCoord((void*)texcoords.data(), texcoords.size() * sizeof(GLfloat));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertexTexCoord(void* texData, size_t dataSize)
{
    assert(dataSize == m_NVertices * 2 * sizeof(GLfloat));
    m_hasVertexTexCoord = true;

    m_VertexTexCoords.resize(m_NVertices);
    std::memcpy((void*)m_VertexTexCoords.data(), texData, dataSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::scaleVertexTexCoord(GLfloat scaleX, GLfloat scaleY)
{
    if(!m_hasVertexTexCoord) {
        return;
    }
    Scheduler::parallel_for(m_NVertices, [&](size_t i) { m_VertexTexCoords[i] *= Vec2f(scaleX, scaleY); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::clearVertexTexCoord()
{
    m_VertexTexCoords.resize(0);
    m_hasVertexTexCoord = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertexColor(const Vector<Vec3f>& vcolors)
{
    setVertexColor((void*)vcolors.data(), vcolors.size() * sizeof(Vec3f));
}

void MeshObject::setVertexColor(const Vector<GLfloat>& vcolors)
{
    setVertexColor((void*)vcolors.data(), vcolors.size() * sizeof(GLfloat));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertexColor(void* colorData, size_t dataSize)
{
    assert(dataSize == m_NVertices * 3 * sizeof(GLfloat));
    m_hasVertexColor = true;

    m_VertexColors.resize(m_NVertices);
    std::memcpy((void*)m_VertexColors.data(), colorData, dataSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::generateRandomVertexColor()
{
    if(!m_isDataReady) {
        return;
    }

    m_hasVertexColor = true;
    m_VertexColors.resize(m_NVertices);

    // cannot run in parallel: rand() is not thread safe
    for(size_t i = 0; i < m_VertexColors.size(); ++i) {
        m_VertexColors[i] = NumberHelpers::fRand<float>::vrnd<Vec3f>();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::clearVertexColor()
{
    m_VertexColors.resize(0);
    m_hasVertexColor = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::clearElementIndex()
{
    m_IndexList.resize(0);
    m_hasIndexBuffer = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setElementIndex(const Vector<GLuint>& indices)
{
    m_isMeshVeryLarge = true;
    m_hasIndexBuffer  = true;

    m_IndexListLong.resize(indices.size());
    std::copy(indices.begin(), indices.end(), m_IndexListLong.begin());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::clearElementIndexLong()
{
    m_IndexListLong.resize(0);
    m_hasIndexBuffer = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::draw()
{
    if(!m_isDataReady) {
        return;
    }

    if(!m_isGLDataReady) {
        createBuffers();
        uploadDataToGPU();
    }

    if(m_CullFaceMode == GL_NONE) {
        glCall(glDisable(GL_CULL_FACE));
    } else {
        glCall(glEnable(GL_CULL_FACE));
        glCall(glCullFace(m_CullFaceMode));
    }

    if(m_hasIndexBuffer) {
        if(m_isMeshVeryLarge) {
            glCall(glDrawElements(m_DataTopology, static_cast<GLsizei>(m_IndexListLong.size()), GL_UNSIGNED_INT, 0));
        } else {
            glCall(glDrawElements(m_DataTopology, static_cast<GLsizei>(m_IndexList.size()), GL_UNSIGNED_SHORT, 0));
        }
    } else {
        glCall(glDrawArrays(m_DataTopology, 0, static_cast<GLsizei>(m_NVertices)));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::uploadDataToGPU()
{
    if(!m_isDataReady) {
        return;
    }

    if(!m_isBufferCreated) {
        createBuffers();
    }

    //size_t offset = 0;
    size_t dataSize = sizeof(Vec3f) * m_NVertices;
    assert(m_Vertices.size() == m_NVertices);
    m_VertexBuffer->uploadDataAsync((GLvoid*)m_Vertices.data(), 0, dataSize);

    if(m_hasVertexNormal) {
        assert(m_VertexNormals.size() == m_NVertices);
        m_NormalBuffer->uploadDataAsync((GLvoid*)m_VertexNormals.data(), 0, dataSize);
    }

    if(m_hasVertexColor) {
        assert(m_VertexColors.size() == m_NVertices);
        m_VertexColorBuffer->uploadDataAsync((GLvoid*)m_VertexColors.data(), 0, dataSize);
    }

    if(m_hasVertexTexCoord) {
        assert(m_VertexTexCoords.size() == m_NVertices);
        dataSize = sizeof(Vec2f) * m_VertexTexCoords.size();
        m_TexCoordBuffer->uploadDataAsync((GLvoid*)m_VertexTexCoords.data(), 0, dataSize);
    }
    ////////////////////////////////////////////////////////////////////////////////
    if(m_hasIndexBuffer) {
        assert(m_IndexBuffer != nullptr);

        dataSize = m_isMeshVeryLarge ? sizeof(GLuint) * m_IndexListLong.size() : sizeof(GLushort) * m_IndexList.size();
        m_IndexBuffer->uploadDataAsync(m_isMeshVeryLarge ? (GLvoid*)m_IndexListLong.data() : (GLvoid*)m_IndexList.data(), 0, dataSize);
    }
    ////////////////////////////////////////////////////////////////////////////////
    m_isGLDataReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::clearData()
{
    m_Vertices.resize(0);
    clearVertexNormal();
    clearVertexTexCoord();
    clearVertexColor();
    clearElementIndex();
    clearElementIndexLong();

    m_NVertices    = 0;
    m_DataTopology = GL_TRIANGLES;

    m_isMeshVeryLarge = false;
    m_isDataReady     = false;
    m_bTranslation    = false;
    m_bScale          = false;
    m_bRotate         = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::createBuffers()
{
    if(!m_isDataReady) {
        return;
    }

    size_t vBufferSize = sizeof(Vec3f) * m_NVertices;
    m_VertexBuffer = std::make_shared<OpenGLBuffer>();
    m_VertexBuffer->createBuffer(GL_ARRAY_BUFFER, vBufferSize);

    if(m_hasVertexNormal) {
        m_NormalBuffer = std::make_shared<OpenGLBuffer>();
        m_NormalBuffer->createBuffer(GL_ARRAY_BUFFER, vBufferSize);
    }
    if(m_hasVertexColor) {
        m_VertexColorBuffer = std::make_shared<OpenGLBuffer>();
        m_VertexColorBuffer->createBuffer(GL_ARRAY_BUFFER, vBufferSize);
    }

    if(m_hasVertexTexCoord) {
        vBufferSize      = sizeof(Vec2f) * m_VertexTexCoords.size();
        m_TexCoordBuffer = std::make_shared<OpenGLBuffer>();
        m_TexCoordBuffer->createBuffer(GL_ARRAY_BUFFER, vBufferSize);
    }

    // create element array buffer
    if(m_hasIndexBuffer) {
        size_t iBufferSize = m_isMeshVeryLarge ? sizeof(GLuint) * m_IndexListLong.size() : sizeof(GLushort) * m_IndexList.size();
        m_IndexBuffer = std::make_shared<OpenGLBuffer>();
        m_IndexBuffer->createBuffer(GL_ELEMENT_ARRAY_BUFFER, iBufferSize);
    }

    m_isBufferCreated = true;

    // upload to GPU
    // uploadDataToGPU();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::clearBuffer()
{
    m_VertexBuffer->deleteBuffer();
    if(m_NormalBuffer != nullptr) { m_NormalBuffer->deleteBuffer(); }
    if(m_VertexColorBuffer != nullptr) { m_VertexColorBuffer->deleteBuffer(); }
    if(m_TexCoordBuffer != nullptr) { m_TexCoordBuffer->deleteBuffer(); }
    if(m_IndexBuffer != nullptr) { m_IndexBuffer->deleteBuffer(); }
    ////////////////////////////////////////////////////////////////////////////////
    m_isGLDataReady = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setElementIndex(const Vector<GLushort>& indices)
{
    m_isMeshVeryLarge = false;
    m_hasIndexBuffer  = true;

    m_IndexList.resize(indices.size());
    std::copy(indices.begin(), indices.end(), m_IndexList.begin());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana