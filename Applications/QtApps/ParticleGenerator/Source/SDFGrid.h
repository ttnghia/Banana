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

#include "Common.h"

#include <Banana/Data/ParticleSystemData.h>
#include <Banana/Geometry/GeometryObjects.h>
#include <Banana/ParallelHelpers/Scheduler.h>
#include <Banana/Utils/Timer.h>
#include <QObject>

#include <Banana/Utils/Timer.h>

#include <memory>
#include <future>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class SDFGrid : public QObject
{
    Q_OBJECT

public:
    SDFGrid(const SharedPtr<ParticleSystemData>& particleData) : m_ParticleData(particleData) {}

public slots:
    void setSDFObjectType(SDFObjectTypes SDFObjType);
    void setResolution(int resolution);
    void setTransformation(const Vec3f& translation, const Vec4f& rotation, float uniformScale);

signals:
    void dataReady();
    void generationTimeChanged(float time);

private:
    void generateParticles();

    SDFObjectTypes                m_SDFObjectType = Box;
    int                           m_Resolution    = 32;
    SharedPtr<ParticleSystemData> m_ParticleData;
    std::future<void>             m_SDFGenerationFutureObj;
    Vec3f                         m_Translation  = Vec3f(0);
    Vec4f                         m_Rotation     = Vec4f(Vec3f(1), 0);
    float                         m_UniformScale = 1.0f;
    Timer                         m_Timer;

    SharedPtr<GeometryObjects::GeometryObject<3, float> > m_SDFObject;
};
