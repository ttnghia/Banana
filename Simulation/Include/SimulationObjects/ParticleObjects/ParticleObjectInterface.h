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
template<class Real>
class ParticleObject
{
protected:
    __BNN_SETUP_DATA_TYPE(Real)
public:
    ParticleObject()  = default;
    ~ParticleObject() = default;

    std::string& name() { return m_ObjName; }
    std::string& meshFile() { return m_MeshFile; }
    std::string& particleCacheFile() { return m_CacheFile; }
    Real& particleRadius() { return m_ParticleRadius; }
    Real& samplingJitter() { return m_SamplingJitter; }
    bool& relaxPosition() { return m_bRelaxPosition; }
    ParticleHelpers::RelaxationMethod& relaxPositionMethod() { return m_RelaxMethod; }

    Vec3r& translation() { return m_Translation; }
    Vec3r& rotation() { return m_Rotation; }
    Vec3r& scale() { return m_Scale; }

    const Vec3r& aabbMin() const noexcept { return m_AABBMin; }
    const Vec3r& aabbMax() const noexcept { return m_AABBMax; }
    const Vec3r& objCenter() const noexcept { return m_ObjCenter; }

    void transformParticles(const Vec3r& translation, const Vec3r& rotation) { ParticleHelpers::transform(m_Particles, m_Translation, m_Rotation); }
    unsigned int getNumParticles() const noexcept { return static_cast<unsigned int>(m_Particles.size()); }
    const Vec_Vec3r& getParticles() const noexcept { return m_Particles; }

    void makeObj();

protected:
    void         computeAABB();
    virtual void generateParticles() = 0;

    std::string m_ObjName   = "NoName";
    std::string m_MeshFile  = "";
    std::string m_CacheFile = "";

    Vec3r m_Translation = Vec3r(0);
    Vec3r m_Rotation    = Vec3r(0);
    Vec3r m_Scale       = Vec3r(1);

    Vec3r m_AABBMin   = Vec3r(Huge);
    Vec3r m_AABBMax   = Vec3r(Tiny);
    Vec3r m_ObjCenter = Vec3r(0);

    Vec_Vec3r                         m_Particles;
    Real                              m_ParticleRadius = 0;
    Real                              m_SamplingJitter = 0.1;
    bool                              m_bRelaxPosition = false;
    ParticleHelpers::RelaxationMethod m_RelaxMethod    = ParticleHelpers::RelaxationMethod::SPHBasedRelaxationMethod;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Implementation
template<class Real>
void Banana::ParticleObject<Real>::makeObj()
{
    if(!m_CacheFile.empty() && FileHelpers::fileExisted(m_CacheFile))
    {
        m_Particles.resize(0);
        ParticleHelpers::loadBinary(m_CacheFile, m_Particles, m_ParticleRadius);
    }
    else
    {
        __BNN_ASSERT(m_ParticleRadius > 0);
        generateParticles();

        if(m_bRelaxPosition)
            ParticleHelpers::relaxPosition(m_Particles, m_RelaxMethod);

        if(!m_CacheFile.empty())
            ParticleHelpers::saveBinary(m_CacheFile, m_Particles, m_ParticleRadius);
    }

    computeAABB();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
void Banana::ParticleObject<Real>::computeAABB()
{
    for(auto& ppos : m_Particles)
    {
        for(int l = 0; l < 3; ++l)
        {
            if(m_AABBMin[l] > ppos[l])
                m_AABBMin[l] = ppos[l];

            if(m_AABBMax[l] < ppos[l])
                m_AABBMax[l] = ppos[l];
        }
    }

    m_ObjCenter = (m_AABBMin + m_AABBMax) * Real(0.5);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana