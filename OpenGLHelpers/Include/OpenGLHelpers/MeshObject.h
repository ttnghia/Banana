//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/19/2017
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

#include <vector>
#include <cassert>

#include <OpenGLHelpers/OpenGLMacros.h>
#include <OpenGLHelpers/OpenGLBuffer.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __Banana_Qt__
class MeshObject : public OpenGLFunctions
{
#else
class MeshObject
{
#endif

public:
    MeshObject(GLenum _topology = GL_TRIANGLES,
               bool _veryLargeMesh = false) :
        m_DataTopology(_topology),
        m_isMeshVeryLarge(_veryLargeMesh),
        m_isDataReady(false),
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
        m_NumIndices(0)
    {
#ifdef __Banana_Qt__
        initializeOpenGLFunctions();
#endif
    }

    ~MeshObject()
    {
        clearData();
        clearBuffer();

        delete m_ArrayBuffer;
        delete m_IndexBuffer;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void transformObject(GLfloat scaleX = 1.0, GLfloat scaleY = 1.0, GLfloat scaleZ = 1.0,
                         GLfloat translateX = 0.0, GLfloat translateY = 0.0,
                         GLfloat translateZ = 0.0)
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

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setVertices(const std::vector<GLfloat>& vertices)
    {
        m_NumVertices = vertices.size() / 3;
        m_Vertices.resize(m_NumVertices);
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

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setVertexNormal(const std::vector<GLfloat>& normals)
    {
        assert(normals.size() == m_NumVertices * 3);
        m_hasVertexNormal = true;

        m_VertexNormals.resize(m_NumVertices * 3);
        std::copy(normals.begin(), normals.end(), m_VertexNormals.begin());
    }

    void inverseVertexNormal()
    {
        if(!m_hasVertexNormal)
            return;

        for(size_t i=0; i < m_VertexNormals.size(); ++i)
            m_VertexNormals[i] *= -1.0;
    }

    void clearVertexNormal()
    {
        m_VertexNormals.resize(0);
        m_hasVertexNormal = false;
    }
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setVertexTexCoord(const std::vector<float>& texcoords)
    {
        assert(texcoords.size() == m_NumVertices * 2);
        m_hasVertexTexCoord = true;

        m_VertexTexCoords.resize(m_NumVertices * 2);
        std::copy(texcoords.begin(), texcoords.end(), m_VertexTexCoords.begin());
    }

    void scaleVertexTexCoord(GLfloat scaleX, GLfloat scaleY)
    {
        if(!m_hasVertexTexCoord)
            return;

        for(size_t i=0; i < m_NumVertices; ++i)
        {
            m_VertexTexCoords[i * 2] *= scaleX;
            m_VertexTexCoords[i * 2 + 1] *= scaleY;
        }
    }

    void clearVertexTexCoord()
    {
        m_VertexTexCoords.resize(0);
        m_hasVertexTexCoord = false;
    }
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setVertexColor(const std::vector<float>& vcolors)
    {
        assert(vcolors.size() == m_NumVertices * 3);
        m_hasVertexColor = true;

        m_VertexColors.resize(m_NumVertices * 3);
        std::copy(vcolors.begin(), vcolors.end(), m_VertexColors.begin());
    }

    void generateRandomVertexColor()
    {
        if(!m_isDataReady)
            return;

        m_hasVertexColor = true;
        m_VertexColors.resize(m_Vertices.size());

        for(size_t i=0; i < m_VertexColors.size(); ++i)
            m_VertexColors[i] = (GLfloat)rand() / (GLfloat)RAND_MAX;
    }

    void clearVertexColor()
    {
        m_VertexColors.resize(0);
        m_hasVertexColor = false;
    }
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setElementIndex(const std::vector<GLushort>& indices)
    {
        m_isMeshVeryLarge = false;
        m_hasIndexBuffer = true;

        m_NumIndices = indices.size();
        m_IndicesList.resize(m_NumIndices);
        std::copy(indices.begin(), indices.end(), m_IndicesList.begin());
    }

    void clearElementIndex()
    {
        m_IndicesList.resize(0);
        m_hasIndexBuffer = false;
    }
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setElementIndex(const std::vector<GLuint>& indices)
    {
        m_isMeshVeryLarge = true;
        m_hasIndexBuffer = true;

        m_NumIndices = indices.size();
        m_IndicesListLong.resize(m_NumIndices);
        std::copy(indices.begin(), indices.end(), m_IndicesListLong.begin());
    }

    void clearElementIndexLong()
    {
        m_IndicesListLong.resize(0);
        m_hasIndexBuffer = false;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void render()
    {
        if(!m_isDataReady)
            return;

        if(!m_isGLDataReady)
        {
            createBuffers();
            uploadDataToGPU();
        }

        if(m_hasIndexBuffer)
        {
            if(m_isMeshVeryLarge)
            {
                glCall(glDrawElements(m_DataTopology, m_NumIndices, GL_UNSIGNED_INT, 0));
            }
            else
            {
                glCall(glDrawElements(m_DataTopology, m_NumIndices, GL_UNSIGNED_SHORT, 0));
            }
        }
        else
        {
            glCall(glDrawArrays(m_DataTopology, 0, m_NumVertices));
        }

    }
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void uploadDataToGPU()
    {
        size_t offset = 0;
        size_t dataSize = sizeof(GLfloat) * m_Vertices.size();
        m_ArrayBuffer->uploadData((GLvoid*)m_Vertices.data(), offset, dataSize);
        offset += dataSize;

        if(m_hasVertexNormal)
        {
            dataSize = sizeof(GLfloat) * m_VertexNormals.size();
            m_ArrayBuffer->uploadData((GLvoid*)m_VertexNormals.data(), offset, dataSize);
            offset += dataSize;
        }

        if(m_hasVertexTexCoord)
        {
            dataSize = sizeof(GLfloat) * m_VertexTexCoords.size();
            m_ArrayBuffer->uploadData((GLvoid*)m_VertexTexCoords.data(), offset, dataSize);
            offset += dataSize;
        }

        if(m_hasVertexColor)
        {
            dataSize = sizeof(GLfloat) * m_VertexColors.size();
            m_ArrayBuffer->uploadData((GLvoid*)m_VertexColors.data(), offset, dataSize);
            offset += dataSize;
        }

        ////////////////////////////////////////////////////////////////////////////////
        if(m_hasIndexBuffer)
        {
            offset = 0;
            dataSize = m_isMeshVeryLarge ? sizeof(GLuint) * m_IndicesListLong.size() :
                sizeof(GLushort) * m_IndicesList.size();
            m_IndexBuffer->uploadData(m_isMeshVeryLarge ? (GLvoid*)m_IndicesListLong.data() :
                (GLvoid*)m_IndicesList.data(), 0, dataSize);
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    OpenGLBuffer*         m_ArrayBuffer;
    OpenGLBuffer*         m_IndexBuffer;

protected:
    void clearData()
    {
        m_Vertices.resize(0);
        clearVertexNormal();
        clearVertexTexCoord();
        clearVertexColor();
        clearElementIndex();
        clearElementIndexLong();

        m_NumVertices  = 0;
        m_NumIndices   = 0;
        m_DataTopology = GL_TRIANGLES;

        m_isMeshVeryLarge = false;
        m_isNoTransformation  = true;
        m_isDataReady     = false;

        m_ScaleX = 1.0;
        m_ScaleY = 1.0;
        m_ScaleZ = 1.0;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void createBuffers()
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

        m_ArrayBuffer->createBuffer(GL_ARRAY_BUFFER, vBufferSize);



        // create element array buffer
        if(m_hasIndexBuffer)
        {
            size_t iBufferSize = m_isMeshVeryLarge ? sizeof(GLuint) * m_IndicesListLong.size() :
                sizeof(GLushort) * m_IndicesList.size();
            m_IndexBuffer->createBuffer(GL_ELEMENT_ARRAY_BUFFER, iBufferSize);
        }

        // upload to GPU
        uploadDataToGPU();
    }

    void clearBuffer()
    {
        m_ArrayBuffer->deleteBuffer();
        m_IndexBuffer->deleteBuffer();

        m_isGLDataReady = false;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    std::vector<GLushort> m_IndicesList;
    std::vector<GLuint>   m_IndicesListLong;
    std::vector<GLfloat>  m_Vertices;
    std::vector<GLfloat>  m_VertexNormals;
    std::vector<GLfloat>  m_VertexTexCoords;
    std::vector<GLfloat>  m_VertexColors;

    size_t m_NumVertices;
    size_t m_NumIndices;
    GLenum m_DataTopology;

    bool m_isMeshVeryLarge;
    bool m_isNoTransformation;
    bool m_isDataReady;
    bool m_isGLDataReady;

    bool m_hasVertexTexCoord;
    bool m_hasVertexNormal;
    bool m_hasVertexColor;
    bool m_hasIndexBuffer;

    GLfloat m_ScaleX;
    GLfloat m_ScaleY;
    GLfloat m_ScaleZ;
    GLfloat m_TranslateX;
    GLfloat m_TranslateY;
    GLfloat m_TranslateZ;
};
