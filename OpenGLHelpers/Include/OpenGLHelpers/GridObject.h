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
#include <OpenGLHelpers/MeshObject.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class GridObject : public MeshObject
{
public:
    GridObject()
    {
        generateGrid(1, 1);
    }

    void generateGrid(int sizeX, int sizeY)
    {
        clearData();

        GLfloat vertex[3];
        vertex[1] = 0;

        GLfloat normal[3] ={0.0, 1.0, 0.0};

        GLfloat stepX = 1.0 / (GLfloat)sizeX;
        GLfloat stepY = 1.0 / (GLfloat)sizeY;

        for(int i = 0; i <= sizeX; ++i)
        {
            for(int j = 0; j <= sizeY; ++j)
            {
                vertex[0] = -0.5 + (GLfloat)i * stepX;
                vertex[2] = -0.5 + (GLfloat)j * stepY;

                m_Vertices.push_back(vertex[0]);
                m_Vertices.push_back(vertex[1]);
                m_Vertices.push_back(vertex[2]);

                m_VertexNormals.push_back(normal[0]);
                m_VertexNormals.push_back(normal[1]);
                m_VertexNormals.push_back(normal[2]);

                m_VertexTexCoords.push_back(vertex[0]);
                m_VertexTexCoords.push_back(vertex[2]);
            }
        }

        ////////////////////////////////////////////////////////////////////////////////
        m_isDataReady       = true;
        m_hasVertexNormal   = true;
        m_hasVertexTexCoord = true;
    }
};
