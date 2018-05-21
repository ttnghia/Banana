//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define DEFAULT_CAMERA_POSITION Vec3f(3.0, 0.8, 0.0)
#define DEFAULT_CAMERA_FOCUS    Vec3f(0, -0.2, 0)

#define CUSTOM_PARTICLE_MATERIAL_OUTSIDE  \
    {                                     \
        Vec4f(0.2 * 0.2),                 \
        Vec4f(1.0, 0.63, 0.3, 1.00),      \
        Vec4f(1),                         \
        250.0,                            \
        String("ParticleMaterialOutside") \
    }

#define CUSTOM_PARTICLE_MATERIAL_INSIDE  \
    {                                    \
        Vec4f(0.2 * 0.2),                \
        Vec4f(0.0, 0.3, 0.77, 1.00),     \
        Vec4f(1),                        \
        250.0,                           \
        String("ParticleMaterialInside") \
    }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define GridResolutions { QString("32"), QString("64"), QString("128"), QString("256"), QString("512") }

enum SDFObjectTypes
{
    Box = 0,
    Sphere,
    Torus,
    Torus28,
    Torus2Inf,
    Torus88,
    TorusInfInf,
    Cylinder,
    Cone,
    Plane,
    TriangularPrism,
    HexagonalPrism,
    Capsule,
    Ellipsoid,
    BoxSphereUnion,
    BoxSubtractSphere,
    BoxSphereIntersection,
    BoxSphereBlendExp,
    BoxSphereBlendPoly,
    TorusTwist,
    BoxBend,
    TriMeshObj
};

#define SDFObjectNames { QString("Box"), QString("Sphere"), QString("Torus"), QString("Torus28"), QString("Torus2Inf"), QString("Torus88"), QString("TorusInfInf"),     \
                         QString("Cylinder"), QString("Cone"), QString("Plane"), QString("TriangularPrism"), QString("HexagonalPrism"), QString("Capsule"),             \
                         QString("Ellipsoid"), QString("BoxSphereUnion"), QString("BoxSubtractSphere"), QString("BoxSphereIntersection"), QString("BoxSphereBlendExp"), \
                         QString("BoxSphereBlendPoly"), QString("TorusTwist"), QString("BoxBend"), QString("TriMeshObj")                                                \
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/Setup.h>
#include <Banana/Data/ParticleSystemData.h>
#include <QVector3D>
#include <QDebug>
using namespace Banana;
