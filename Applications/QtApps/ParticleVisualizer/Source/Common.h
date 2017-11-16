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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define DEFAULT_CLEAR_COLOR     Vec4f(1.0, 1.0, 1.0, 1.0)
#define DEFAULT_CAMERA_POSITION Vec3f(4.0, 4.0, 3.0)
#define DEFAULT_CAMERA_FOCUS    Vec3f(0, 1, 0)
#define DEFAULT_LIGHT_POSITION  Vec3f(-10, 10, 10)

#define CUSTOM_PARTICLE_MATERIAL         \
    {                                    \
        Vec4f(0.2 * 0.2),                \
        Vec4f(0.69, 0.957, 0.259, 1.00), \
        Vec4f(1),                        \
        250.0,                           \
        String("ParticleMaterial")       \
    }


#define CUSTOM_SURFACE_MATERIAL                   \
    {                                             \
        Vec4f(0.135,    0.2225,   0.1575,   1.0), \
        Vec4f(0.54,     0.89,     0.63,     1.0), \
        Vec4f(0.316228, 0.316228, 0.316228, 1.0), \
        250.0,                                    \
        String("ParticleMaterial")                \
    }

#define DEFAULT_FLUID_VOLUME_MATERIAL Material::MT_Jade
#define DEFAULT_MESH_MATERIAL         Material::MT_Brass

#define PARTICLE_COLOR_RAMP   \
    {                         \
        Vec3f(1.0, 0.0, 0.0), \
        Vec3f(1.0, 0.5, 0.0), \
        Vec3f(1.0, 1.0, 0.0), \
        Vec3f(1.0, 0.0, 1.0), \
        Vec3f(0.0, 1.0, 0.0), \
        Vec3f(0.0, 1.0, 1.0), \
        Vec3f(0.0, 0.0, 1.0)  \
    }

#define DEFAULT_CLIP_PLANE           Vec4f(0.0, 0.0, -1.0, 0.0)
#define DEFAULT_DELAY_TIME           50

#define MAX_FILTER_SIZE              100
#define SHADOWMAP_TEXTURE_SIZE       2048
#define CLEAR_DEPTH_VALUE            -1000000.0f

#define THICKNESS_TEXTURE_SIZE_RATIO 0.25
#define THICKNESS_FILTER_SIZE        5

#define MAX_NUM_MESHES               8

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class ParticleColorMode
{
public:
    enum
    {
        Uniform = 0,
        Random,
        Ramp,
        FromData,
        NumColorMode
    };
};

class GLSLPrograms
{
public:
    enum
    {
        RenderFloor = 0,
        RenderSkyBox,
        RenderLight,
        RenderDataDomain,
        RenderPointSphere,
        DepthPass,
        ThicknessPass,
        BilateralGaussianFilter,
        PlaneFittingFilter,
        CurvatureFlowFilter,
        ModifiedGaussianFilter,
        NormalPass,
        CompositingPass,
        RenderMesh,
        RenderMeshDepth,
        RenderMeshShadow,
        NumPrograms
    };
};

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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/Setup.h>
namespace Banana {}
using namespace Banana;