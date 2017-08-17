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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define DEFAULT_CAMERA_POSITION glm::vec3(3.0, 0.8, 0.0)
#define DEFAULT_CAMERA_FOCUS    glm::vec3(0, -0.2, 0)
#define DEFAULT_CLIP_PLANE      glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f)

#define CUSTOM_PARTICLE_MATERIAL_OUTSIDE \
    {                                    \
        glm::vec4(0.2 * 0.2),            \
        glm::vec4(1.0, 0.63, 0.3, 1.00), \
        glm::vec4(1),                    \
        250.0,                           \
        std::string("ParticleMaterial")  \
    }

#define CUSTOM_PARTICLE_MATERIAL_INSIDE  \
    {                                    \
        glm::vec4(0.2 * 0.2),            \
        glm::vec4(0.0, 0.3, 0.77, 1.00), \
        glm::vec4(1),                    \
        250.0,                           \
        std::string("ParticleMaterial")  \
    }


#define PARTICLE_COLOR_RAMP       \
    {                             \
        glm::vec3(1.0, 0.0, 0.0), \
        glm::vec3(1.0, 0.5, 0.0), \
        glm::vec3(1.0, 1.0, 0.0), \
        glm::vec3(1.0, 0.0, 1.0), \
        glm::vec3(0.0, 1.0, 0.0), \
        glm::vec3(0.0, 1.0, 1.0), \
        glm::vec3(0.0, 0.0, 1.0)  \
    }


#define DEFAULT_FLOOR_SIZE 10

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define GridResolutions    { QString("32"), QString("64"), QString("128"), QString("256"), QString("512") }

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
#include <QStringList>
#include <QString>
#include <QDir>
////////////////////////////////////////////////////////////////////////////////
inline QStringList getTextureFolders(QString texType)
{
    QDir dataDir(QDir::currentPath() + "/Textures/" + texType);
    dataDir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);

    return dataDir.entryList();
}

inline QStringList getTextureFiles(QString texType)
{
    QDir dataDir(QDir::currentPath() + "/Textures/" + texType);
    dataDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);

    return dataDir.entryList();
}

inline QStringList getSceneFiles()
{
    QDir dataDir(QDir::currentPath() + "/Scenes");
    dataDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);

    return dataDir.entryList();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/TypeNames.h>
#include <Banana/Data/ParticleSystemData.h>
#include <memory>
#include <QVector3D>
#include <QDebug>
using namespace Banana;
