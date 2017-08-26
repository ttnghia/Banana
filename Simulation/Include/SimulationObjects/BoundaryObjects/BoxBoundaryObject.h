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

#include <SimulationObjects/BoundaryObjects/BoundaryObjectInterface.h>

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
    BoxBoundaryObject(const Vec3<RealType>& bMin, const Vec3<RealType>& bMax, RealType margin) :
        BoundaryObject(margin), m_BMin(bMin), m_BMax(bMax), m_MovingBMin(bMin), m_MovingBMax(bMax)
    {
        computeMovingBox();
    }

    BoxBoundaryObject(const Vec3<RealType>& bMin, const Vec3<RealType>& bMax, RealType margin, RealType restitution) :
        BoundaryObject(margin, restitution), m_BMin(bMin), m_BMax(bMax), m_MovingBMin(bMin), m_MovingBMax(bMax)
    {
        computeMovingBox();
    }

    void         setBox(const Vec3<RealType>& bMin, const Vec3<RealType>& bMax);
    virtual void generateBoundaryParticles(RealType spacing, int numBDLayers = 2) override;
    virtual bool constrainToBoundary(Vec3<RealType>& ppos, Vec3<RealType>& pvel) override;

private:
    void computeMovingBox();

    Vec3<RealType> m_BMin, m_BMax;
    Vec3<RealType> m_MovingBMin, m_MovingBMax;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::BoxBoundaryObject<RealType>::setBox(const Vec3<RealType>& bMin, const Vec3<RealType>& bMax)
{
    m_BMin = bMin;
    m_BMax = bMax;
    computeMovingBox();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::BoxBoundaryObject<RealType>::computeMovingBox()
{
    m_MovingBMin = m_BMin + Vec3<RealType>(m_Margin);
    m_MovingBMax = m_BMax - Vec3<RealType>(m_Margin);
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

    ////////////////////////////////////////////////////////////////////////////////
    // plane x < 0
    {
        Vec3<RealType> minLX = m_BMin - Vec3<RealType>(spacing * RealType(numBDLayers * 1.001));
        Vec3<RealType> maxLX = m_BMax + Vec3<RealType>(spacing * RealType(numBDLayers * 1.001));
        maxLX[0] = m_BMin[0];
        Vec3i gridLX = ParticleHelpers::createGrid(minLX, maxLX, spacing);

        for(int x = 0; x < gridLX[0]; ++x)
        {
            for(int y = 0; y < gridLX[1]; ++y)
            {
                for(int z = 0; z < gridLX[2]; ++z)
                {
                    const Vec3<RealType> gridPos = minLX + Vec3<RealType>(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3<RealType>       ppos    = gridPos + Vec3<RealType>(disSmall(gen), disLarge(gen), disLarge(gen));
                    m_BDParticles.push_back(ppos);
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane x > 0
    {
        Vec3<RealType> minUX = m_BMin - Vec3<RealType>(spacing * RealType(numBDLayers * 1.001));
        Vec3<RealType> maxUX = m_BMax + Vec3<RealType>(spacing * RealType(numBDLayers * 1.001));
        minUX[0] = m_BMax[0];
        Vec3i gridUX = ParticleHelpers::createGrid(minUX, maxUX, spacing);

        for(int x = 0; x < gridUX[0]; ++x)
        {
            for(int y = 0; y < gridUX[1]; ++y)
            {
                for(int z = 0; z < gridUX[2]; ++z)
                {
                    const Vec3<RealType> gridPos = minUX + Vec3<RealType>(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3<RealType>       ppos    = gridPos + Vec3<RealType>(disSmall(gen), disLarge(gen), disLarge(gen));
                    m_BDParticles.push_back(ppos);
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane y < 0
    {
        Vec3<RealType> minLY = m_BMin - Vec3<RealType>(spacing * RealType(numBDLayers) * 1.001);
        Vec3<RealType> maxLY = m_BMax + Vec3<RealType>(spacing * RealType(numBDLayers) * 1.001);
        minLY[0] = m_BMin[0];
        maxLY[0] = m_BMax[0];
        maxLY[1] = m_BMin[1];
        Vec3i gridLY = ParticleHelpers::createGrid(minLY, maxLY, spacing);

        for(int x = 0; x < gridLY[0]; ++x)
        {
            for(int y = 0; y < gridLY[1]; ++y)
            {
                for(int z = 0; z < gridLY[2]; ++z)
                {
                    const Vec3<RealType> gridPos = minLY + Vec3<RealType>(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3<RealType>       ppos    = gridPos + Vec3<RealType>(disLarge(gen), disSmall(gen), disLarge(gen));
                    m_BDParticles.push_back(ppos);
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane y > 0
    {
        Vec3<RealType> minUY = m_BMin - Vec3<RealType>(spacing * RealType(numBDLayers) * 1.001);
        Vec3<RealType> maxUY = m_BMax + Vec3<RealType>(spacing * RealType(numBDLayers) * 1.001);
        minUY[0] = m_BMin[0];
        maxUY[0] = m_BMax[0];
        minUY[1] = m_BMax[1];
        Vec3i gridUY = ParticleHelpers::createGrid(minUY, maxUY, spacing);

        for(int x = 0; x < gridUY[0]; ++x)
        {
            for(int y = 0; y < gridUY[1]; ++y)
            {
                for(int z = 0; z < gridUY[2]; ++z)
                {
                    const Vec3<RealType> gridPos = minUY + Vec3<RealType>(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3<RealType>       ppos    = gridPos + Vec3<RealType>(disLarge(gen), disSmall(gen), disLarge(gen));
                    m_BDParticles.push_back(ppos);
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane z < 0
    {
        Vec3<RealType> minLZ = m_BMin - Vec3<RealType>(spacing * RealType(numBDLayers) * 1.001);
        Vec3<RealType> maxLZ = m_BMax + Vec3<RealType>(spacing * RealType(numBDLayers) * 1.001);
        minLZ[0] = m_BMin[0];
        maxLZ[0] = m_BMax[0];
        minLZ[1] = m_BMin[1];
        maxLZ[1] = m_BMax[1];
        maxLZ[2] = m_BMin[2];
        Vec3i gridLZ = ParticleHelpers::createGrid(minLZ, maxLZ, spacing);

        for(int x = 0; x < gridLZ[0]; ++x)
        {
            for(int y = 0; y < gridLZ[1]; ++y)
            {
                for(int z = 0; z < gridLZ[2]; ++z)
                {
                    const Vec3<RealType> gridPos = minLZ + Vec3<RealType>(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3<RealType>       ppos    = gridPos + Vec3<RealType>(disLarge(gen), disLarge(gen), disSmall(gen));
                    m_BDParticles.push_back(ppos);
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane z > 0
    {
        Vec3<RealType> minUZ = m_BMin - Vec3<RealType>(spacing * RealType(numBDLayers) * 1.001);
        Vec3<RealType> maxUZ = m_BMax + Vec3<RealType>(spacing * RealType(numBDLayers) * 1.001);
        minUZ[0] = m_BMin[0];
        maxUZ[0] = m_BMax[0];
        minUZ[1] = m_BMin[1];
        maxUZ[1] = m_BMax[1];
        minUZ[2] = m_BMax[2];
        Vec3i gridUX = ParticleHelpers::createGrid(minUZ, maxUZ, spacing);

        for(int x = 0; x < gridUX[0]; ++x)
        {
            for(int y = 0; y < gridUX[1]; ++y)
            {
                for(int z = 0; z < gridUX[2]; ++z)
                {
                    const Vec3<RealType> gridPos = minUZ + Vec3<RealType>(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3<RealType>       ppos    = gridPos + Vec3<RealType>(disLarge(gen), disLarge(gen), disSmall(gen));
                    m_BDParticles.push_back(ppos);
                }
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
bool Banana::BoxBoundaryObject<RealType>::constrainToBoundary(Vec3<RealType>& pPos, Vec3<RealType>& pVel)
{
    bool velChanged = false;

    for(int l = 0; l < 3; ++l)
    {
        if(pPos[l] < m_MovingBMin[l] || pPos[l] > m_MovingBMax[l])
        {
            pPos[l]    = MathHelpers::min(MathHelpers::max(pPos[l], m_MovingBMin[l]), m_MovingBMax[l]);
            pVel[l]   *= -m_RestitutionCoeff;
            velChanged = true;
        }
    }

    return velChanged;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana