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
#define DEFAULT_CAMERA_POSITION     Vec3f(3.5, 0.5, 0)
#define DEFAULT_CAMERA_FOCUS        Vec3f(0, 0, 0)
#define DEFAULT_LIGHT_POSITION      Vec4f(10, 10, -10, 1.0)

#define DEFAULT_CHECKERBOARD_COLOR1 Vec3f(0.9)
#define DEFAULT_CHECKERBOARD_COLOR2 Vec3f(0.5)
#define DEFAULT_BOX_COLOR           Vec3f(0, 1, 0.5)

#define CUSTOM_PARTICLE_MATERIAL         \
    {                                    \
        Vec4f(0.2 * 0.2),                \
        Vec4f(0.69, 0.957, 0.259, 1.00), \
        Vec4f(1),                        \
        250.0,                           \
        String("ParticleMaterial")       \
    }

#define DEFAULT_CHECKERBOARD_GRID_SIZE 20
#define DEFAULT_MESH_MATERIAL          Material::MT_Brass
#define DEFAULT_CLIP_PLANE             Vec4f(0.0, 0.0, -1.0, 0.0)
#define DEFAULT_DELAY_TIME             40
#define NUM_MESHES                     8

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct BackgroundMode
{
    enum
    {
        SkyBox = 0,
        Color,
        Checkerboard,
        Grid,
        NumBackgroundMode
    };
};

struct ParticleColorMode
{
    enum
    {
        UniformMaterial = 0,
        Random          = 1,
        Ramp            = 2,
        FromData        = 3,
        NumColorMode
    };
};

struct GLSLPrograms
{
    enum
    {
        RenderFloor = 0,
        RenderSkyBox,
        RenderLight,
        RenderDataDomain,
        RenderPointSphere,
        RenderMesh,
        RenderMeshDepth,
        RenderMeshShadow,
        NumPrograms
    };
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/Setup.h>
namespace Banana {}
using namespace Banana;