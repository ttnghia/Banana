//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/24/2017
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <OpenGLHelpers/MeshObject.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MeshObject::~MeshObject()
{
    clearData();
    clearBuffer();

    delete m_ArrayBuffer;
    delete m_IndexBuffer;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertexNormal(const std::vector<GLfloat>& normals)
{
    assert(normals.size() == m_NumVertices * 3);
    m_hasVertexNormal = true;

    m_VertexNormals.resize(m_NumVertices * 3);
    std::copy(normals.begin(), normals.end(), m_VertexNormals.begin());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::inverseVertexNormal()
{
    if(!m_hasVertexNormal)
        return;

    for(size_t i=0; i < m_VertexNormals.size(); ++i)
        m_VertexNormals[i] *= -1.0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::clearVertexNormal()
{
    m_VertexNormals.resize(0);
    m_hasVertexNormal = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertexTexCoord(const std::vector<float>& texcoords)
{
    assert(texcoords.size() == m_NumVertices * 2);
    m_hasVertexTexCoord = true;

    m_VertexTexCoords.resize(m_NumVertices * 2);
    std::copy(texcoords.begin(), texcoords.end(), m_VertexTexCoords.begin());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::scaleVertexTexCoord(GLfloat scaleX, GLfloat scaleY)
{
    if(!m_hasVertexTexCoord)
        return;

    for(size_t i=0; i < m_NumVertices; ++i)
    {
        m_VertexTexCoords[i * 2] *= scaleX;
        m_VertexTexCoords[i * 2 + 1] *= scaleY;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::clearVertexTexCoord()
{
    m_VertexTexCoords.resize(0);
    m_hasVertexTexCoord = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setVertexColor(const std::vector<float>& vcolors)
{
    assert(vcolors.size() == m_NumVertices * 3);
    m_hasVertexColor = true;

    m_VertexColors.resize(m_NumVertices * 3);
    std::copy(vcolors.begin(), vcolors.end(), m_VertexColors.begin());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::generateRandomVertexColor()
{
    if(!m_isDataReady)
        return;

    m_hasVertexColor = true;
    m_VertexColors.resize(m_Vertices.size());

    for(size_t i=0; i < m_VertexColors.size(); ++i)
        m_VertexColors[i] = (GLfloat)rand() / (GLfloat)RAND_MAX;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::clearVertexColor()
{
    m_VertexColors.resize(0);
    m_hasVertexColor = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::clearElementIndex()
{
    m_IndicesList.resize(0);
    m_hasIndexBuffer = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setElementIndex(const std::vector<GLuint>& indices)
{
    m_isMeshVeryLarge = true;
    m_hasIndexBuffer = true;

    m_IndicesListLong.resize(indices.size());
    std::copy(indices.begin(), indices.end(), m_IndicesListLong.begin());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::clearElementIndexLong()
{
    m_IndicesListLong.resize(0);
    m_hasIndexBuffer = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setCullFaceMode(GLenum cullFaceMode)
{
    m_CullFaceMode = cullFaceMode;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::bindAllBuffers()
{
    if(!m_isDataReady)
        return;

    m_ArrayBuffer->bind();
    if(m_hasIndexBuffer)
    {
        m_IndexBuffer->bind();
    }
}

void MeshObject::releaseAllBuffers()
{
    if(!m_isDataReady)
        return;

    m_ArrayBuffer->release();
    if(m_hasIndexBuffer)
    {
        m_IndexBuffer->release();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::draw()
{
    if(!m_isDataReady)
        return;

    if(!m_isGLDataReady)
    {
        createBuffers();
        uploadDataToGPU();
    }

    //bindAllBuffers();

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
            glCall(glDrawElements(m_DataTopology, m_IndicesListLong.size(), GL_UNSIGNED_INT, 0));
        }
        else
        {
            glCall(glDrawElements(m_DataTopology, m_IndicesList.size(), GL_UNSIGNED_SHORT, 0));
        }
    }
    else
    {
        glCall(glDrawArrays(m_DataTopology, 0, m_NumVertices));
    }

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::uploadDataToGPU()
{
    if(!m_isBufferCreated)
    {
        createBuffers();
    }

    size_t offset = 0;
    size_t dataSize = sizeof(GLfloat) * m_Vertices.size();
    m_ArrayBuffer->uploadData((GLvoid*)m_Vertices.data(), offset, dataSize);
    offset += dataSize;

    if(m_hasVertexNormal)
    {
        assert(m_VertexNormals.size() == m_NumVertices * 3);
        m_VNormalOffset = offset;
        dataSize = sizeof(GLfloat) * m_VertexNormals.size();
        m_ArrayBuffer->uploadData((GLvoid*)m_VertexNormals.data(), offset, dataSize);
        offset += dataSize;
    }

    if(m_hasVertexTexCoord)
    {
        assert(m_VertexTexCoords.size() == m_NumVertices * 2);
        m_VTexCoordOffset = offset;
        dataSize = sizeof(GLfloat) * m_VertexTexCoords.size();
        m_ArrayBuffer->uploadData((GLvoid*)m_VertexTexCoords.data(), offset, dataSize);
        offset += dataSize;
    }

    if(m_hasVertexColor)
    {
        assert(m_VertexColors.size() == m_NumVertices * 3);
        m_VColorOffset = offset;
        dataSize = sizeof(GLfloat) * m_VertexColors.size();
        m_ArrayBuffer->uploadData((GLvoid*)m_VertexColors.data(), offset, dataSize);
        offset += dataSize;
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(m_hasIndexBuffer)
    {
        assert(m_IndexBuffer != nullptr);

        offset = 0;
        dataSize = m_isMeshVeryLarge ? sizeof(GLuint) * m_IndicesListLong.size() :
            sizeof(GLushort) * m_IndicesList.size();
        m_IndexBuffer->uploadData(m_isMeshVeryLarge ? (GLvoid*)m_IndicesListLong.data() :
            (GLvoid*)m_IndicesList.data(), 0, dataSize);
    }

    ////////////////////////////////////////////////////////////////////////////////
    m_isGLDataReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t MeshObject::getVNormalOffset()
{
    return m_VNormalOffset;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t MeshObject::getVTexCoordOffset()
{
    return m_VTexCoordOffset;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t MeshObject::getVColorOffset()
{
    return m_VColorOffset;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MeshObject::hasVertexNormal()
{
    return m_hasVertexNormal;
}

bool MeshObject::hasVertexTexCoord()
{
    return m_hasVertexTexCoord;
}

bool MeshObject::hasVertexColor()
{
    return m_hasVertexColor;
}

bool MeshObject::hasIndexBuffer()
{
    return m_hasIndexBuffer;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::clearData()
{
    m_Vertices.resize(0);
    clearVertexNormal();
    clearVertexTexCoord();
    clearVertexColor();
    clearElementIndex();
    clearElementIndexLong();

    m_NumVertices  = 0;
    m_VNormalOffset  = 0;
    m_VTexCoordOffset  = 0;
    m_VColorOffset  = 0;
    m_DataTopology = GL_TRIANGLES;

    m_isMeshVeryLarge = false;
    m_isNoTransformation  = true;
    m_isDataReady     = false;

    m_ScaleX = 1.0;
    m_ScaleY = 1.0;
    m_ScaleZ = 1.0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::createBuffers()
{
    // create vertex array buffer
    if(!m_isDataReady)
        return;

    size_t vBufferSize = sizeof(GLfloat) * m_Vertices.size();

    if(m_hasVertexNormal)
        vBufferSize += sizeof(GLfloat) * m_VertexNormals.size();

    if(m_hasVertexTexCoord)
        vBufferSize += sizeof(GLfloat) * m_VertexTexCoords.size();

    if(m_hasVertexColor)
        vBufferSize += sizeof(GLfloat) * m_VertexColors.size();

    m_ArrayBuffer = new OpenGLBuffer;
    m_ArrayBuffer->createBuffer(GL_ARRAY_BUFFER, vBufferSize);

    // create element array buffer
    if(m_hasIndexBuffer)
    {
        size_t iBufferSize = m_isMeshVeryLarge ? sizeof(GLuint) * m_IndicesListLong.size() :
            sizeof(GLushort) * m_IndicesList.size();

        m_IndexBuffer = new OpenGLBuffer;
        m_IndexBuffer->createBuffer(GL_ELEMENT_ARRAY_BUFFER, iBufferSize);
    }

    m_isBufferCreated = true;

    // upload to GPU
    // uploadDataToGPU();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::clearBuffer()
{
    m_ArrayBuffer->deleteBuffer();
    m_IndexBuffer->deleteBuffer();

    m_isGLDataReady = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshObject::setElementIndex(const std::vector<GLushort>& indices)
{
    m_isMeshVeryLarge = false;
    m_hasIndexBuffer = true;

    m_IndicesList.resize(indices.size());
    std::copy(indices.begin(), indices.end(), m_IndicesList.begin());
}
