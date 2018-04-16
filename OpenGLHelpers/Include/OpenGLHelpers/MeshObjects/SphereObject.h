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
class SphereObject : public MeshObject
{
public:
    SphereObject() { generateSphere(25, 50); }

    void generateSphere(int numStacks, int numSlices)
    {
        clearData();
        Vec3f vertex;
        Vec2f tex;
        m_NVertices = 0;

        for(int j = 0; j <= numStacks; ++j) {
            GLfloat theta    = (GLfloat)j * M_PI / (GLfloat)numStacks;
            GLfloat sinTheta = static_cast<GLfloat>(sin(theta));
            GLfloat cosTheta = static_cast<GLfloat>(cos(theta));

            for(int i = 0; i <= numSlices; ++i) {
                GLfloat phi    = static_cast<GLfloat>(i * 2.0f * M_PI / static_cast<GLfloat>(numSlices));
                GLfloat sinPhi = static_cast<GLfloat>(sin(phi));
                GLfloat cosPhi = static_cast<GLfloat>(cos(phi));

                vertex[0] = cosPhi * sinTheta;
                vertex[1] = cosTheta;
                vertex[2] = sinPhi * sinTheta;

                m_Vertices.push_back(vertex);

                // normal at this point is the same value with coordinate
                m_VertexNormals.push_back(vertex);

                // texture coordinate
                tex[0] = 2.0 * (1.0 - (GLfloat)i / (GLfloat)numSlices);
                tex[1] = 1.0 - (GLfloat)j / (GLfloat)numStacks;
                m_VertexTexCoords.push_back(tex);
                ////////////////////////////////////////////////////////////////////////////////
                ++m_NVertices;
            }
        }

        for(int j = 0; j < numStacks; ++j) {
            for(int i = 0; i < numSlices; ++i) {
                int first  = (j * (numSlices + 1)) + i;
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana