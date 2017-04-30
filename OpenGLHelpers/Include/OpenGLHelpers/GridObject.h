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

#pragma once
#include <OpenGLHelpers/MeshObject.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class GridObject : public MeshObject
{
public:
    GridObject(int sizeX = 1, int sizeY = 1, float scaleTexX = 1.0f, float scaleTexY = 1.0f) :
        m_SizeX(0), m_SizeY(0)
    {
        generateGrid(sizeX, sizeY, scaleTexX, scaleTexY);
    }

    void generateGrid(int sizeX, int sizeY, float scaleTexX = 1.0f, float scaleTexY = 1.0f)
    {
        clearData();

        GLfloat vertex[3];
        vertex[1] = 0;

        GLfloat normal[3] = { 0.0f, 1.0f, 0.0f };

        GLfloat stepX    = 2.0f / static_cast<GLfloat>(sizeX);
        GLfloat stepY    = 2.0f / static_cast<GLfloat>(sizeY);
        GLfloat texStepX = 2.0f / static_cast<GLfloat>(scaleTexX);
        GLfloat texStepY = 2.0f / static_cast<GLfloat>(scaleTexY);
        m_NumVertices = 0;

        for(int i = 0; i <= sizeX; ++i)
        {
            for(int j = 0; j <= sizeY; ++j)
            {
                vertex[0] = -1.0f + static_cast<GLfloat>(i) * stepX;
                vertex[2] = -1.0f + static_cast<GLfloat>(j) * stepY;

                m_Vertices.push_back(vertex[0]);
                m_Vertices.push_back(vertex[1]);
                m_Vertices.push_back(vertex[2]);

                m_VertexNormals.push_back(normal[0]);
                m_VertexNormals.push_back(normal[1]);
                m_VertexNormals.push_back(normal[2]);

                m_VertexTexCoords.push_back(static_cast<GLfloat>(i) * texStepX);
                m_VertexTexCoords.push_back(static_cast<GLfloat>(j) * texStepY);

                ++m_NumVertices;
            }
        }

        GLuint indexSize = sizeX * sizeY * 6;
        m_IndexList.reserve(indexSize);

        for(int j = 0; j < sizeY; ++j)
        {
            for(int i = 0; i < sizeX; ++i)
            {
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

        for(int i = 0; i <= m_SizeX; ++i)
        {
            for(int j = 0; j <= m_SizeX; ++j)
            {
                m_VertexTexCoords.push_back(static_cast<GLfloat>(i) * texStepX);
                m_VertexTexCoords.push_back(static_cast<GLfloat>(j) * texStepY);
            }
        }
    }

private:
    int m_SizeX;
    int m_SizeY;
};
