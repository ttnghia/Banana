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

#include <ParticleTools/ParticleHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class ParticleObject
{
public:
    ParticleObject()  = default;
    ~ParticleObject() = default;

    std::string& name() { return m_ObjName; }
    std::string& meshFile() { return m_MeshFile; }
    std::string& particleCacheFile() { return m_CacheFile; }

    Vec3<RealType>& translation() { return m_Translation; }
    Vec3<RealType>& rotation() { return m_Rotation; }
    Vec3<RealType>& scale() { return m_Scale; }

    const Vec3<RealType>& aabbMin() const noexcept { return m_AABBMin; }
    const Vec3<RealType>& aabbMax() const noexcept { return m_AABBMax; }
    const Vec3<RealType>& objCenter() const noexcept { return m_ObjCenter; }

    void transformParticles(const Vec3<RealType>& translation, const Vec3<RealType>& rotation) { ParticleHelpers::transform(m_Particles, m_Translation, m_Rotation); }
    unsigned int getNumParticles() const noexcept { return static_cast<unsigned int>(m_Particles.size()); }
    const Vec_Vec3<RealType>& getParticles() const noexcept { return m_Particles; }

    virtual void makeObj(RealType particleRadius) = 0;

protected:
    std::string m_ObjName;
    std::string m_MeshFile;
    std::string m_CacheFile;

    Vec3<RealType> m_Translation;
    Vec3<RealType> m_Rotation;
    Vec3<RealType> m_Scale;

    Vec3<RealType> m_AABBMin;
    Vec3<RealType> m_AABBMax;
    Vec3<RealType> m_ObjCenter;

    Vec_Vec3<RealType> m_Particles;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana