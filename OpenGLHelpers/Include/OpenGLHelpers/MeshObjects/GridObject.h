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
#include <OpenGLHelpers/MeshObjects/MeshObject.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class GridObject : public MeshObject
{
public:
    GridObject(int sizeX = 1, int sizeY = 1, float scaleTexX = 1.0f, float scaleTexY = 1.0f) : m_SizeX(0), m_SizeY(0)
    {
        generateGrid(sizeX, sizeY, scaleTexX, scaleTexY);
    }

    void generateGrid(int sizeX, int sizeY, float scaleTexX = 1.0f, float scaleTexY = 1.0f)
    {
        clearData();

        Vec3f vertex;
        vertex[1] = 0;

        Vec3f normal(0.0f, 1.0f, 0.0f);

        GLfloat stepX    = 2.0f / static_cast<GLfloat>(sizeX);
        GLfloat stepY    = 2.0f / static_cast<GLfloat>(sizeY);
        GLfloat texStepX = 2.0f / static_cast<GLfloat>(scaleTexX);
        GLfloat texStepY = 2.0f / static_cast<GLfloat>(scaleTexY);
        m_NVertices = 0;

        for(int i = 0; i <= sizeX; ++i) {
            for(int j = 0; j <= sizeY; ++j) {
                vertex[0] = -1.0f + static_cast<GLfloat>(i) * stepX;
                vertex[2] = -1.0f + static_cast<GLfloat>(j) * stepY;

                m_Vertices.push_back(vertex);
                m_VertexNormals.push_back(normal);
                m_VertexTexCoords.push_back(Vec2f(static_cast<GLfloat>(i) * texStepX, static_cast<GLfloat>(j) * texStepY));
                ++m_NVertices;
            }
        }

        GLuint indexSize = sizeX * sizeY * 6;
        m_IndexList.reserve(indexSize);

        for(int j = 0; j < sizeY; ++j) {
            for(int i = 0; i < sizeX; ++i) {
                m_IndexList.push_back(i * (sizeY + 1) + j);
                m_IndexList.push_back((i + 1) * (sizeY + 1) + j);
                m_IndexList.push_back(i * (sizeY + 1) + (j + 1));

                m_IndexList.push_back((i + 1) * (sizeY + 1) + j);
                m_IndexList.push_back((i + 1) * (sizeY + 1) + (j + 1));
                m_IndexList.push_back(i * (sizeY + 1) + (j + 1));
            }
        }

        ////////////////////////////////////////////////////////////////////////////////
        m_isDataReady       = true;
        m_hasVertexNormal   = true;
        m_hasVertexTexCoord = true;
        m_hasIndexBuffer    = true;

        m_SizeX = sizeX;
        m_SizeY = sizeY;
    }

    void scaleTexCoord(int scaleTexX, int scaleTexY)
    {
        m_VertexTexCoords.resize(0);
        GLfloat texStepX = 2.0f / static_cast<GLfloat>(m_SizeX) * static_cast<GLfloat>(scaleTexX);
        GLfloat texStepY = 2.0f / static_cast<GLfloat>(m_SizeX) * static_cast<GLfloat>(scaleTexY);

        for(int i = 0; i <= m_SizeX; ++i) {
            for(int j = 0; j <= m_SizeX; ++j) {
                m_VertexTexCoords.push_back(Vec2f(static_cast<GLfloat>(i) * texStepX, static_cast<GLfloat>(j) * texStepY));
            }
        }
    }

private:
    int m_SizeX;
    int m_SizeY;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana