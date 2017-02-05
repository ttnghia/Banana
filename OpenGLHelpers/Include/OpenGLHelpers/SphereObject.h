//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <OpenGLHelpers/MeshObject.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class SphereObject : public MeshObject
{
public:
    SphereObject()
    {
        generateSphere(25, 50);
    }

    void generateSphere(int numStacks, int numSlices)
    {
        clearData();
        GLfloat vertex[3];
        GLfloat tex[2];
        m_NumVertices = 0;

        for(int j = 0; j <= numStacks; ++j)
        {
            GLfloat theta = (GLfloat)j * M_PI / (GLfloat)numStacks;
            GLfloat sinTheta = sin(theta);
            GLfloat cosTheta = cos(theta);

            for(int i = 0; i <= numSlices; ++i)
            {
                GLfloat phi = (GLfloat)i * 2.0 * M_PI / (GLfloat)numSlices;
                GLfloat sinPhi = (GLfloat)sin(phi);
                GLfloat cosPhi = (GLfloat)cos(phi);

                vertex[0] = cosPhi * sinTheta;
                vertex[1] = cosTheta;
                vertex[2] = sinPhi * sinTheta;

                m_Vertices.push_back(vertex[0]);
                m_Vertices.push_back(vertex[1]);
                m_Vertices.push_back(vertex[2]);

                // normal at this point is the same value with coordinate
                m_VertexNormals.push_back(vertex[0]);
                m_VertexNormals.push_back(vertex[1]);
                m_VertexNormals.push_back(vertex[2]);

                // texture coordinate
                tex[0] = 2.0*(1.0 - (GLfloat)i / (GLfloat)numSlices);
                tex[1] = 1.0 - (GLfloat)j / (GLfloat)numStacks;
                m_VertexTexCoords.push_back(tex[0]);
                m_VertexTexCoords.push_back(tex[1]);

                ++m_NumVertices;
            }
        }

        for(int j = 0; j < numStacks; ++j)
        {
            for(int i = 0; i < numSlices; ++i)
            {
                int first = (j * (numSlices + 1)) + i;
                int second = first + numSlices + 1;

                m_IndexList.push_back(first);
                m_IndexList.push_back(first + 1);
                m_IndexList.push_back(second);

                m_IndexList.push_back(second);
                m_IndexList.push_back(first + 1);
                m_IndexList.push_back(second + 1);
            }
        }

        ////////////////////////////////////////////////////////////////////////////////
        m_isDataReady       = true;
        m_hasIndexBuffer    = true;
        m_hasVertexNormal   = true;
        m_hasVertexTexCoord = true;
    }

};