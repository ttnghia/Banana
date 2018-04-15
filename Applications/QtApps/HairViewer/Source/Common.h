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
//    /                    Created: 2018 by Nghia Truong                     \
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
#define DEFAULT_CAMERA_POSITION Vec3f(0.0, 0.8, 3.0)
#define DEFAULT_CAMERA_FOCUS    Vec3f(0, -0.2, 0)

#define DEFAULT_LIGHT0_POSITION Vec4f(-10, 20, 10, 1.0)
#define DEFAULT_LIGHT1_POSITION Vec4f(10, -20, -10, 1.0)

#define DEFAULT_COLOR_DATA_MIN  Vec3f(0.7, 0.87, 0.88)
#define DEFAULT_COLOR_DATA_MAX  Vec3f(0.98, 0.27, 0.35)

#define CUSTOM_PARTICLE_MATERIAL     \
    {                                \
        Vec4f(0.2 * 0.2),            \
        Vec4f(1.0, 0.63, 0.3, 1.00), \
        Vec4f(  1),                  \
        250.0,                       \
        String("ParticleMaterial")   \
    }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct HairRenderMode
{
    enum
    {
        LineRender,
        VertexParticle,
        LineWithVertexParticle
    };
};

struct HairColorMode
{
    enum
    {
        UniformMaterial = 0,
        Random,
        Ramp,
        ObjectIndex,
        VelocityMagnitude,
        NumColorMode
    };
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/Setup.h>
#include <OpenGLHelpers/Lights.h>
namespace Banana {}
namespace Banana::ParticleSolvers {}
using namespace Banana;
using namespace Banana::ParticleSolvers;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct VisualizationData
{
    ////////////////////////////////////////////////////////////////////////////////
    // simulation system dimention
    Int systemDimension = 3;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // camera
    Vec3f cameraPosition = DEFAULT_CAMERA_POSITION;
    Vec3f cameraFocus    = DEFAULT_CAMERA_FOCUS;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // domain box
    Vec3f boxMin = Vec3f(-1.0);
    Vec3f boxMax = Vec3f(1.0);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // light
    Vector<PointLights::PointLightData> lights;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // particle data
    char* positions      = nullptr;
    char* velocities     = nullptr;
    char* aniKernel      = nullptr;
    char* objIndex       = nullptr;
    UInt  nObjects       = 1;
    UInt  nParticles     = 0;
    float particleRadius = 0;
    ////////////////////////////////////////////////////////////////////////////////

    VisualizationData() { resetData(); }
    void resetData()
    {
        ////////////////////////////////////////////////////////////////////////////////
        // camera
        cameraPosition = DEFAULT_CAMERA_POSITION;
        cameraFocus    = DEFAULT_CAMERA_FOCUS;
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // domain box
        boxMin = Vec3f(-1.0);
        boxMax = Vec3f(1.0);
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // light
        lights.resize(2);
        lights[0].position = DEFAULT_LIGHT0_POSITION;
        lights[1].position = DEFAULT_LIGHT1_POSITION;
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // particles
        particleRadius = 0;
        nParticles     = 0;
        ////////////////////////////////////////////////////////////////////////////////
    }
};
