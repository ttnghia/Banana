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

#include <ParticleTools/BoundaryObjects/BoundaryObjectInterface.h>

#include <random>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class BoxBoundaryObject : public BoundaryObject<RealType>
{
public:
    BoxBoundaryObject() = default;
    BoxBoundaryObject(const Vec3<RealType>& bMin, const Vec3<RealType>& bMax) : m_BMin(bMin), m_BMax(bMax) {}

    void         setBox(const Vec3<RealType>& bMin, const Vec3<RealType>& bMax);
    virtual void generateBoundaryParticles(RealType spacing, int numBDLayers = 2) override;
    virtual bool constrainToBoundary(Vec3<RealType>& ppos, Vec3<RealType>& pvel, RealType restitution = RealType(0.1)) override;

private:
    Vec3<RealType> m_BMin, m_BMax;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Implementation
template<class RealType>
void Banana::BoxBoundaryObject<RealType>::setBox(const Vec3<RealType>& bMin, const Vec3<RealType>& bMax)
{
    m_BMin = bMin;
    m_BMax = bMax;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::BoxBoundaryObject<RealType>::generateBoundaryParticles(RealType spacing, int numBDLayers /*= 2*/)
{
    m_BDParticles.resize(0);
    std::random_device rd;
    std::mt19937       gen(rd());

    std::normal_distribution<RealType> disSmall(0, RealType(0.02) * spacing);
    std::normal_distribution<RealType> disLarge(0, RealType(0.1) * spacing);

    const Vec3<RealType> boundaryBMin = m_BMin - Vec3<RealType>(spacing * (RealType(numBDLayers) - RealType(0.5)));
    const Vec3<RealType> boundaryBMax = m_BMax + Vec3<RealType>(spacing * (RealType(numBDLayers) - RealType(0.501)));

    ////////////////////////////////////////////////////////////////////////////////
    for(RealType x = boundaryBMin[0]; x <= boundaryBMax[0]; x += spacing)
    {
        if(x > m_BMin[0] && x < m_BMax[0])
            x = m_BMax[0] + spacing * RealType(0.5);

        for(RealType y = boundaryBMin[1]; y <= boundaryBMax[1]; y += spacing)
        {
            if(y > m_BMin[1] && y < m_BMax[1])
                y = m_BMax[1] + spacing * RealType(0.5);

            for(RealType z = boundaryBMin[2]; z <= boundaryBMax[2]; z += spacing)
            {
                if(z > m_BMin[2] && z < m_BMax[2])
                    z = m_BMax[2] + spacing * RealType(0.5);

                const Vec3<RealType> gridPos(x, y, z);
                Vec3<RealType>       ppos = gridPos + Vec3<RealType>(disLarge(gen), disLarge(gen), disLarge(gen));

                if(gridPos[0] < m_BMin[0] || gridPos[0] > m_BMax[0])
                    ppos[0] = gridPos[0] + disSmall(gen);

                if(gridPos[1] < m_BMin[1] || gridPos[1] > m_BMax[1])
                    ppos[1] = gridPos[1] + disSmall(gen);

                if(gridPos[2] < m_BMin[2] || gridPos[2] > m_BMax[2])
                    ppos[2] = gridPos[2] + disSmall(gen);

                m_BDParticles.push_back(ppos);
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
bool Banana::BoxBoundaryObject<RealType>::constrainToBoundary(Vec3<RealType>& pPos, Vec3<RealType>& pVel, RealType restitution /*= RealType(0.1)*/)
{
    bool velChanged = false;

    for(int l = 0; l < 3; ++l)
    {
        if(pPos[l] < m_BMin[l] || pPos[l] > m_BMax[l])
        {
            pPos[l]    = MathHelpers::min(MathHelpers::max(pPos[l], m_BMin[l]), m_BMax[l]);
            pVel[l]   *= -restitution;
            velChanged = true;
        }
    }

    return velChanged;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana