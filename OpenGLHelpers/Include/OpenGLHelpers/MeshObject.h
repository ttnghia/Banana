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
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    MeshObject(GLenum _topology = GL_TRIANGLES,
               bool _veryLargeMesh = false) :
        dataTopology(_topology),
        isMeshVeryLarge(_veryLargeMesh),
        isDataReady(false),
        isBufferReady(false),
        hasIndexBuffer(false),
        isUniformScale(true),
        scaleX(1.0),
        scaleY(1.0),
        scaleZ(1.0),
        numVertices(0)
    {
#ifdef __Banana_Qt__
        initializeOpenGLFunctions();
#endif
    }

    ~MeshObject()
    {
        clearBuffer();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setScale(float _scaleX = 1.0, float _scaleY = 1.0, float _scaleZ = 1.0)
    {
        scaleX = _scaleX;
        scaleY = _scaleY;
        scaleZ = _scaleZ;

        isUniformScale = false;

        ////////////////////////////////////////////////////////////////////////////////
        // rescale
        if(isDataReady)
        {
            for(size_t i = 0; i < (vertices.size() / 3); ++i)
            {
                vertices[i * 3] *= scaleX;
                vertices[i * 3 + 1] *= scaleY;
                vertices[i * 3 + 2] *= scaleZ;
            }
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void render()
    {
        if(!isDataReady)
            return;

        if(!isBufferReady)
        {
            createBuffer();
            uploadDataToGPU();
        }

        if(hasIndexBuffer)
        {
            glCall(glDrawElements(dataTopology, numVertices));
        }
        else
        {
            glCall(glDrawArrays(dataTopology, 0, numVertices));
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void uploadDataToGPU()
    {

    }

protected:
    void createBuffer()
    {
        // create vertex array buffer
        if(!isDataReady)
            return;

        arrayBuffer->createBuffer(GL_ARRAY_BUFFER);



        // create element array buffer
        if(hasIndexBuffer)
        {
            indexBuffer->createBuffer(GL_ELEMENT_ARRAY_BUFFER);
        }

        // upload to GPU
        arrayBuffer->uploadData();
        indexBuffer->uploadData();
    }

    void clearBuffer()
    {
        arrayBuffer->deleteBuffer();
        indexBuffer->deleteBuffer();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    std::vector<GLushort> indicesList;
    std::vector<GLuint> indicesListLong;
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> texCoord;
    std::vector<GLfloat> normals;

    OpenGLBuffer* arrayBuffer;
    OpenGLBuffer* indexBuffer;
    size_t numVertices;

protected:
    GLenum dataTopology;
    bool isMeshVeryLarge;
    bool isUniformScale;
    bool isDataReady;
    bool isBufferReady;
    bool hasVertexColor;
    bool hasVertexTexCoord;
    bool hasVertexNormal;
    bool hasIndexBuffer;

    float scaleX;
    float scaleY;
    float scaleZ;
};
