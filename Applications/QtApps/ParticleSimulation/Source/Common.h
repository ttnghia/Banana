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
#define DEFAULT_CAMERA_POSITION     Vec3f(3.0, 0.8, 0)
#define DEFAULT_CAMERA_FOCUS        Vec3f(0, -0.2, 0)
#define DEFAULT_LIGHT_POSITION      Vec4f(10, 10, -10, 1.0)
#define DEFAULT_CLIP_PLANE          Vec4f(-1.0f, 0.0f, 0.0f, 0.0f)

#define DEFAULT_CHECKERBOARD_COLOR1 Vec3f(0.9)
#define DEFAULT_CHECKERBOARD_COLOR2 Vec3f(0.5)
#define DEFAULT_BOX_COLOR           Vec3f(0, 1, 0.5)
#define DEFAULT_COLOR_DATA_MIN      Vec3f(0, 0, 0)
#define DEFAULT_COLOR_DATA_MAX      Vec3f(1, 1, 1)

#define CUSTOM_PARTICLE_MATERIAL         \
    {                                    \
        glm::vec4(0.2 * 0.2),            \
        glm::vec4(1.0, 0.63, 0.3, 1.00), \
        glm::vec4(1),                    \
        250.0,                           \
        std::string("ParticleMaterial")  \
    }


#define DEFAULT_CHECKERBOARD_GRID_SIZE 20
#define DEFAULT_FLOOR_SIZE             10

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
        UniformMaterial       = 0,
        Random                = 1,
        Ramp                  = 2,
        ObjectIndex           = 3,
        FromVelocityMagnitude = 4,
        NumColorMode
    };
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/Setup.h>
namespace Banana {}
namespace Banana::ParticleSolvers {}
using namespace Banana;
using namespace Banana::ParticleSolvers;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <QString>
#include <QDir>
#include <Banana/Utils/AppConfigReader.h>
inline QString getTexPath()
{
    AppConfigReader config("config.ini");
    if(config.isFileLoaded() && config.hasParam("TexturePath")) {
        return QString::fromStdString(config.getStringValue("TexturePath"));
    } else {
        return QDir::currentPath() + "/Textures";
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct LightData
{
    LightData(const Vec3f& position_ = DEFAULT_LIGHT_POSITION, const Vec3f& color_ = Vec3f(1.0)) : position(position_), color(color_) {}
    Vec3f position;
    Vec3f color;
};

struct VisualizationData
{
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
    Vector<LightData> lights;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // particle data
    Vec_Vec3f*   positions      = nullptr;
    Vec_Mat3x3f* aniKernel      = nullptr;
    char*        colorData      = nullptr;
    UInt         nParticles     = 0;
    float        particleRadius = 0;
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
        lights.resize(0);
        lights.push_back(LightData());
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // particles
        particleRadius = 0;
        nParticles     = 0;
        ////////////////////////////////////////////////////////////////////////////////
    }
};