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

#include <array>

#include <OpenGLHelpers/MeshObjects/MeshObject.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class WireFrameBoxObject : public MeshObject
{
public:
    WireFrameBoxObject()
    {
        generateBox();
    }

    void generateBox()
    {
        clearData();

        m_Vertices = {
            // top
            -1.0,  1.0,  1.0,
            1.0,   1.0,  1.0,
            1.0,   1.0, -1.0,
            -1.0,  1.0, -1.0,
            // bottom
            -1.0, -1.0,  1.0,
            1.0,  -1.0,  1.0,
            1.0,  -1.0, -1.0,
            -1.0, -1.0, -1.0,
        };

        m_IndexList = {
            // top
            0, 1,
            1, 2,
            2, 3,
            3, 0,
            // bottom
            4, 5,
            5, 6,
            6, 7,
            7, 4,
            // sides
            0, 4,
            1, 5,
            2, 6,
            3, 7
        };


        ////////////////////////////////////////////////////////////////////////////////
        m_NumVertices    = 8;
        m_isDataReady    = true;
        m_hasIndexBuffer = true;
        m_DataTopology   = GL_LINES;
    }

    void setBox(const Vec3f& boxMin, const Vec3f& boxMax)
    {
        m_Vertices = {
            // top
            boxMin[0], boxMax[1], boxMax[2],
            boxMax[0], boxMax[1], boxMax[2],
            boxMax[0], boxMax[1], boxMin[2],
            boxMin[0], boxMax[1], boxMin[2],
            // bottom
            boxMin[0], boxMin[1], boxMax[2],
            boxMax[0], boxMin[1], boxMax[2],
            boxMax[0], boxMin[1], boxMin[2],
            boxMin[0], boxMin[1], boxMin[2]
        };
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana