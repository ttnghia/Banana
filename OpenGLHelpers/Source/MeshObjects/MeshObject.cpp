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
void MeshObject::transformObject(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ,
                                 GLfloat translateX, GLfloat translateY,
                                 GLfloat translateZ)
{
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
    m_ScaleZ = scaleZ;

    m_TranslateX = translateX;
    m_TranslateY = translateY;
    m_TranslateZ = translateZ;

    m_isNoTransformation = false;

    ////////////////////////////////////////////////////////////////////////////////
    // rescale
    if(m_isDataReady)
    {
        for(size_t i = 0; i < m_NumVertices; ++i)
        {
            m_Vertices[i * 3]     = m_Vertices[i * 3] * m_ScaleX + m_TranslateX;
            m_Vertices[i * 3 + 1] = m_Vertices[i * 3 + 1] * m_ScaleX + m_TranslateY;
            m_Vertices[i * 3 + 2] = m_Vertices[i * 3 + 2] * m_ScaleX + m_TranslateZ;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertices(const std::vector<GLfloat>& vertices)
{
    m_NumVertices = vertices.size() / 3;
    m_Vertices.resize(vertices.size());
    std::copy(vertices.begin(), vertices.end(), m_Vertices.begin());

    if(!m_isNoTransformation)
    {
        for(size_t i = 0; i < m_NumVertices; ++i)
        {
            m_Vertices[i * 3]     = m_Vertices[i * 3] * m_ScaleX + m_TranslateX;
            m_Vertices[i * 3 + 1] = m_Vertices[i * 3 + 1] * m_ScaleX + m_TranslateY;
            m_Vertices[i * 3 + 2] = m_Vertices[i * 3 + 2] * m_ScaleX + m_TranslateZ;
        }
    }

    m_isDataReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertices(void* vertexData, size_t dataSize)
{
    m_NumVertices = dataSize / 3 / sizeof(GLfloat);
    m_Vertices.resize(m_NumVertices * 3);
    memcpy(m_Vertices.data(), vertexData, dataSize);

    if(!m_isNoTransformation)
    {
        for(size_t i = 0; i < m_NumVertices; ++i)
        {
            m_Vertices[i * 3]     = m_Vertices[i * 3] * m_ScaleX + m_TranslateX;
            m_Vertices[i * 3 + 1] = m_Vertices[i * 3 + 1] * m_ScaleX + m_TranslateY;
            m_Vertices[i * 3 + 2] = m_Vertices[i * 3 + 2] * m_ScaleX + m_TranslateZ;
        }
    }

    m_isDataReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertexNormal(const std::vector<GLfloat>& normals)
{
    assert(normals.size() == m_NumVertices * 3);
    m_hasVertexNormal = true;

    m_VertexNormals.resize(m_NumVertices * 3);
    std::copy(normals.begin(), normals.end(), m_VertexNormals.begin());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertexNormal(void* normalData, size_t dataSize)
{
    assert(dataSize == m_NumVertices * 3 * sizeof(GLfloat));
    m_hasVertexNormal = true;

    m_VertexNormals.resize(m_NumVertices * 3);
    memcpy(m_VertexNormals.data(), normalData, dataSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::inverseVertexNormal()
{
    if(!m_hasVertexNormal)
        return;

    for(size_t i = 0; i < m_VertexNormals.size(); ++i)
        m_VertexNormals[i] *= -1.0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::clearVertexNormal()
{
    m_VertexNormals.resize(0);
    m_hasVertexNormal = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertexTexCoord(const std::vector<float>& texcoords)
{
    assert(texcoords.size() == m_NumVertices * 2);
    m_hasVertexTexCoord = true;

    m_VertexTexCoords.resize(m_NumVertices * 2);
    std::copy(texcoords.begin(), texcoords.end(), m_VertexTexCoords.begin());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertexTexCoord(void* texData, size_t dataSize)
{
    assert(dataSize == m_NumVertices * 2 * sizeof(GLfloat));
    m_hasVertexTexCoord = true;

    m_VertexTexCoords.resize(m_NumVertices * 2);
    memcpy(m_VertexTexCoords.data(), texData, dataSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::scaleVertexTexCoord(GLfloat scaleX, GLfloat scaleY)
{
    if(!m_hasVertexTexCoord)
        return;

    for(size_t i = 0; i < m_NumVertices; ++i)
    {
        m_VertexTexCoords[i * 2]     *= scaleX;
        m_VertexTexCoords[i * 2 + 1] *= scaleY;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::clearVertexTexCoord()
{
    m_VertexTexCoords.resize(0);
    m_hasVertexTexCoord = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertexColor(const std::vector<float>& vcolors)
{
    assert(vcolors.size() == m_NumVertices * 3);
    m_hasVertexColor = true;

    m_VertexColors.resize(m_NumVertices * 3);
    std::copy(vcolors.begin(), vcolors.end(), m_VertexColors.begin());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertexColor(void* colorData, size_t dataSize)
{
    assert(dataSize == m_NumVertices * 3 * sizeof(GLfloat));
    m_hasVertexColor = true;

    m_VertexColors.resize(m_NumVertices * 3);
    memcpy(m_VertexColors.data(), colorData, dataSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::generateRandomVertexColor()
{
    if(!m_isDataReady)
        return;

    m_hasVertexColor = true;
    m_VertexColors.resize(m_Vertices.size());

    for(size_t i = 0; i < m_VertexColors.size(); ++i)
        m_VertexColors[i] = (GLfloat)rand() / (GLfloat)RAND_MAX;
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
void MeshObject::setElementIndex(const std::vector<GLuint>& indices)
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
void MeshObject::setCullFaceMode(GLenum cullFaceMode)
{
    m_CullFaceMode = cullFaceMode;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::draw()
{
    if(!m_isDataReady)
        return;

    if(!m_isGLDataReady)
    {
        createBuffers();
        uploadDataToGPU();
    }

    if(m_CullFaceMode == GL_NONE)
    {
        glCall(glDisable(GL_CULL_FACE));
    }
    else
    {
        glCall(glEnable(GL_CULL_FACE));
        glCall(glCullFace(m_CullFaceMode));
    }

    if(m_hasIndexBuffer)
    {
        if(m_isMeshVeryLarge)
        {
            glCall(glDrawElements(m_DataTopology, static_cast<GLsizei>(m_IndexListLong.size()), GL_UNSIGNED_INT, 0));
        }
        else
        {
            glCall(glDrawElements(m_DataTopology, static_cast<GLsizei>(m_IndexList.size()), GL_UNSIGNED_SHORT, 0));
        }
    }
    else
    {
        glCall(glDrawArrays(m_DataTopology, 0, static_cast<GLsizei>(m_NumVertices)));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::uploadDataToGPU()
{
    if(!m_isDataReady)
    {
        return;
    }

    if(!m_isBufferCreated)
    {
        createBuffers();
    }

    //size_t offset = 0;
    size_t dataSize = sizeof(GLfloat) * m_Vertices.size();
    m_VertexBuffer->uploadDataAsync((GLvoid*)m_Vertices.data(), 0, dataSize);

    if(m_hasVertexNormal)
    {
        assert(m_VertexNormals.size() == m_NumVertices * 3);
        dataSize = sizeof(GLfloat) * m_VertexNormals.size();
        m_NormalBuffer->uploadDataAsync((GLvoid*)m_VertexNormals.data(), 0, dataSize);
    }

    if(m_hasVertexTexCoord)
    {
        assert(m_VertexTexCoords.size() == m_NumVertices * 2);
        dataSize = sizeof(GLfloat) * m_VertexTexCoords.size();
        m_TexCoordBuffer->uploadDataAsync((GLvoid*)m_VertexTexCoords.data(), 0, dataSize);
    }

    if(m_hasVertexColor)
    {
        assert(m_VertexColors.size() == m_NumVertices * 3);
        dataSize = sizeof(GLfloat) * m_VertexColors.size();
        m_VertexColorBuffer->uploadDataAsync((GLvoid*)m_VertexColors.data(), 0, dataSize);
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(m_hasIndexBuffer)
    {
        assert(m_IndexBuffer != nullptr);

        dataSize = m_isMeshVeryLarge ? sizeof(GLuint) * m_IndexListLong.size() :
                   sizeof(GLushort) * m_IndexList.size();
        m_IndexBuffer->uploadDataAsync(m_isMeshVeryLarge ? (GLvoid*)m_IndexListLong.data() :
                                       (GLvoid*)m_IndexList.data(), 0, dataSize);
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

    m_NumVertices  = 0;
    m_DataTopology = GL_TRIANGLES;

    m_isMeshVeryLarge    = false;
    m_isNoTransformation = true;
    m_isDataReady        = false;

    m_ScaleX = 1.0;
    m_ScaleY = 1.0;
    m_ScaleZ = 1.0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MeshObject::isEmpty() const
{
    return !m_isDataReady;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t MeshObject::getNumVertices() const
{
    return m_NumVertices;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const std::vector<GLushort>& MeshObject::getIndexList() const
{
    return m_IndexList;
}

const std::vector<GLuint>& MeshObject::getIndexListLong() const
{
    return m_IndexListLong;
}

const std::vector<GLfloat>& MeshObject::getVertices() const
{
    return m_Vertices;
}

const std::vector<GLfloat>& MeshObject::getVertexNormals() const
{
    return m_VertexNormals;
}

const std::vector<GLfloat>& MeshObject::getVertexTexCoords() const
{
    return m_VertexTexCoords;
}

const std::vector<GLfloat>& MeshObject::getVertexColors() const
{
    return m_VertexColors;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const std::shared_ptr<OpenGLBuffer>& MeshObject::getIndexBuffer() const
{
    return m_IndexBuffer;
}

const std::shared_ptr<OpenGLBuffer>& MeshObject::getVertexBuffer() const
{
    return m_VertexBuffer;
}

const std::shared_ptr<OpenGLBuffer>& MeshObject::getNormalBuffer() const
{
    return m_NormalBuffer;
}

const std::shared_ptr<OpenGLBuffer>& MeshObject::getTexCoordBuffer() const
{
    return m_TexCoordBuffer;
}

const std::shared_ptr<OpenGLBuffer>& MeshObject::getVertexColorBuffer() const
{
    return m_VertexColorBuffer;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MeshObject::hasVertexNormal() const
{
    return m_hasVertexNormal;
}

bool MeshObject::hasVertexTexCoord() const
{
    return m_hasVertexTexCoord;
}

bool MeshObject::hasVertexColor() const
{
    return m_hasVertexColor;
}

bool MeshObject::hasIndexBuffer() const
{
    return m_hasIndexBuffer;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::createBuffers()
{
    // create vertex array buffer
    if(!m_isDataReady)
        return;

    size_t vBufferSize = sizeof(GLfloat) * m_Vertices.size();
    m_VertexBuffer = std::make_shared<OpenGLBuffer>();
    m_VertexBuffer->createBuffer(GL_ARRAY_BUFFER, vBufferSize);


    if(m_hasVertexNormal)
    {
        vBufferSize    = sizeof(GLfloat) * m_VertexNormals.size();
        m_NormalBuffer = std::make_shared<OpenGLBuffer>();
        m_NormalBuffer->createBuffer(GL_ARRAY_BUFFER, vBufferSize);
    }

    if(m_hasVertexTexCoord)
    {
        vBufferSize      = sizeof(GLfloat) * m_VertexTexCoords.size();
        m_TexCoordBuffer = std::make_shared<OpenGLBuffer>();
        m_TexCoordBuffer->createBuffer(GL_ARRAY_BUFFER, vBufferSize);
    }

    if(m_hasVertexColor)
    {
        vBufferSize         = sizeof(GLfloat) * m_VertexColors.size();
        m_VertexColorBuffer = std::make_shared<OpenGLBuffer>();
        m_VertexColorBuffer->createBuffer(GL_ARRAY_BUFFER, vBufferSize);
    }

    // create element array buffer
    if(m_hasIndexBuffer)
    {
        size_t iBufferSize = m_isMeshVeryLarge ? sizeof(GLuint) * m_IndexListLong.size() :
                             sizeof(GLushort) * m_IndexList.size();

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
    m_IndexBuffer->deleteBuffer();

    m_isGLDataReady = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setElementIndex(const std::vector<GLushort>& indices)
{
    m_isMeshVeryLarge = false;
    m_hasIndexBuffer  = true;

    m_IndexList.resize(indices.size());
    std::copy(indices.begin(), indices.end(), m_IndexList.begin());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana