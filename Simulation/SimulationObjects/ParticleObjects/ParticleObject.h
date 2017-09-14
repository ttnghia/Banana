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

#include <Banana/Setup.h>
#include <ParticleTools/ParticleHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleObject2D
{};

class ParticleObject3D
{
public:
    ParticleObject3D() = default;

    String& name() { return m_ObjName; }
    String& meshFile() { return m_MeshFile; }
    String& particleCacheFile() { return m_CacheFile; }
    Real&   particleRadius() { return m_ParticleRadius; }
    Real&   samplingJitter() { return m_SamplingJitter; }
    bool&   relaxPosition() { return m_bRelaxPosition; }

    ParticleHelpers::RelaxationMethod& relaxPositionMethod() { return m_RelaxMethod; }

    Vec3r& translation() { return m_Translation; }
    Vec3r& rotation() { return m_Rotation; }
    Vec3r& scale() { return m_Scale; }

    const Vec3r& aabbMin() const noexcept { return m_AABBMin; }
    const Vec3r& aabbMax() const noexcept { return m_AABBMax; }
    const Vec3r& objCenter() const noexcept { return m_ObjCenter; }

    void             transformParticles(const Vec3r& translation, const Vec3r& rotation) { ParticleHelpers::transform(m_Particles, translation, rotation); }
    UInt             getNumParticles() const noexcept { return static_cast<UInt>(m_Particles.size()); }
    const Vec_Vec3r& getParticles() const noexcept { return m_Particles; }

    void makeObj();

protected:
    void         computeAABB();
    virtual void generateParticles() = 0;

    String m_ObjName   = "NoName";
    String m_MeshFile  = "";
    String m_CacheFile = "";

    Vec3r m_Translation = Vec3r(0);
    Vec3r m_Rotation    = Vec3r(0);
    Vec3r m_Scale       = Vec3r(1);

    Vec3r m_AABBMin   = Vec3r(Huge);
    Vec3r m_AABBMax   = Vec3r(Tiny);
    Vec3r m_ObjCenter = Vec3r(0);

    Vec_Vec3r                         m_Particles;
    Real                              m_ParticleRadius = 0;
    Real                              m_SamplingJitter = Real(0.1);
    bool                              m_bRelaxPosition = false;
    ParticleHelpers::RelaxationMethod m_RelaxMethod    = ParticleHelpers::RelaxationMethod::SPHBasedRelaxationMethod;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana