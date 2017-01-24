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

    ~MeshObject();

    void transformObject(GLfloat scaleX = 1.0, GLfloat scaleY = 1.0, GLfloat scaleZ = 1.0,
                         GLfloat translateX = 0.0, GLfloat translateY = 0.0,
                         GLfloat translateZ = 0.0);
    void setVertices(const std::vector<GLfloat>& vertices);

    void setVertexNormal(const std::vector<GLfloat>& normals);
    void inverseVertexNormal();
    void clearVertexNormal();

    void setVertexTexCoord(const std::vector<float>& texcoords);
    void scaleVertexTexCoord(GLfloat scaleX, GLfloat scaleY);
    void clearVertexTexCoord();

    void setVertexColor(const std::vector<float>& vcolors);
    void generateRandomVertexColor();
    void clearVertexColor();

    void setElementIndex(const std::vector<GLushort>& indices);
    void clearElementIndex();
    void setElementIndex(const std::vector<GLuint>& indices);
    void clearElementIndexLong();

    ////////////////////////////////////////////////////////////////////////////////
    void render();
    void uploadDataToGPU();

    ////////////////////////////////////////////////////////////////////////////////
    OpenGLBuffer* m_ArrayBuffer;
    OpenGLBuffer* m_IndexBuffer;

protected:
    void clearData();

    void createBuffers();
    void clearBuffer();

    ////////////////////////////////////////////////////////////////////////////////
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
