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
template<class Real>
class BoxBoundaryObject : public BoundaryObject<Real>
{
public:
    BoxBoundaryObject() = default;
    BoxBoundaryObject(const Vec3r& bMin, const Vec3r& bMax) : m_BMin(bMin), m_BMax(bMax) {}
    BoxBoundaryObject(const Vec3r& bMin, const Vec3r& bMax, Real margin) :
        BoundaryObject(margin), m_BMin(bMin), m_BMax(bMax), m_MovingBMin(bMin), m_MovingBMax(bMax)
    {
        computeMovingBox();
    }

    BoxBoundaryObject(const Vec3r& bMin, const Vec3r& bMax, Real margin, Real restitution) :
        BoundaryObject(margin, restitution), m_BMin(bMin), m_BMax(bMax), m_MovingBMin(bMin), m_MovingBMax(bMax)
    {
        computeMovingBox();
    }

    void         setBox(const Vec3r& bMin, const Vec3r& bMax);
    virtual void generateBoundaryParticles(Real spacing, int numBDLayers = 2) override;
    virtual bool constrainToBoundary(Vec3r& ppos, Vec3r& pvel) override;

private:
    void computeMovingBox();

    ////////////////////////////////////////////////////////////////////////////////
    Vec3r m_BMin, m_BMax;
    Vec3r m_MovingBMin, m_MovingBMax;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
void Banana::BoxBoundaryObject<Real>::setBox(const Vec3r& bMin, const Vec3r& bMax)
{
    m_BMin = bMin;
    m_BMax = bMax;
    computeMovingBox();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
void Banana::BoxBoundaryObject<Real>::computeMovingBox()
{
    m_MovingBMin = m_BMin + Vec3r(m_Margin);
    m_MovingBMax = m_BMax - Vec3r(m_Margin);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
void Banana::BoxBoundaryObject<Real>::generateBoundaryParticles(Real spacing, int numBDLayers /*= 2*/)
{
    m_BDParticles.resize(0);
    std::random_device rd;
    std::mt19937       gen(rd());

    std::normal_distribution<Real> disSmall(0, Real(0.02) * spacing);
    std::normal_distribution<Real> disLarge(0, Real(0.1) * spacing);

    ////////////////////////////////////////////////////////////////////////////////
    // plane x < 0
    {
        Vec3r minLX = m_BMin - Vec3r(spacing * Real(numBDLayers * 1.001));
        Vec3r maxLX = m_BMax + Vec3r(spacing * Real(numBDLayers * 1.001));
        maxLX[0] = m_BMin[0];
        Vec3i gridLX = ParticleHelpers::createGrid(minLX, maxLX, spacing);

        for(int x = 0; x < gridLX[0]; ++x)
        {
            for(int y = 0; y < gridLX[1]; ++y)
            {
                for(int z = 0; z < gridLX[2]; ++z)
                {
                    const Vec3r gridPos = minLX + Vec3r(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disSmall(gen), disLarge(gen), disLarge(gen));
                    m_BDParticles.push_back(ppos);
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane x > 0
    {
        Vec3r minUX = m_BMin - Vec3r(spacing * Real(numBDLayers * 1.001));
        Vec3r maxUX = m_BMax + Vec3r(spacing * Real(numBDLayers * 1.001));
        minUX[0] = m_BMax[0];
        Vec3i gridUX = ParticleHelpers::createGrid(minUX, maxUX, spacing);

        for(int x = 0; x < gridUX[0]; ++x)
        {
            for(int y = 0; y < gridUX[1]; ++y)
            {
                for(int z = 0; z < gridUX[2]; ++z)
                {
                    const Vec3r gridPos = minUX + Vec3r(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disSmall(gen), disLarge(gen), disLarge(gen));
                    m_BDParticles.push_back(ppos);
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane y < 0
    {
        Vec3r minLY = m_BMin - Vec3r(spacing * Real(numBDLayers) * 1.001);
        Vec3r maxLY = m_BMax + Vec3r(spacing * Real(numBDLayers) * 1.001);
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
                    const Vec3r gridPos = minLY + Vec3r(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disLarge(gen), disSmall(gen), disLarge(gen));
                    m_BDParticles.push_back(ppos);
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane y > 0
    {
        Vec3r minUY = m_BMin - Vec3r(spacing * Real(numBDLayers) * 1.001);
        Vec3r maxUY = m_BMax + Vec3r(spacing * Real(numBDLayers) * 1.001);
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
                    const Vec3r gridPos = minUY + Vec3r(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disLarge(gen), disSmall(gen), disLarge(gen));
                    m_BDParticles.push_back(ppos);
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane z < 0
    {
        Vec3r minLZ = m_BMin - Vec3r(spacing * Real(numBDLayers) * 1.001);
        Vec3r maxLZ = m_BMax + Vec3r(spacing * Real(numBDLayers) * 1.001);
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
                    const Vec3r gridPos = minLZ + Vec3r(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disLarge(gen), disLarge(gen), disSmall(gen));
                    m_BDParticles.push_back(ppos);
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane z > 0
    {
        Vec3r minUZ = m_BMin - Vec3r(spacing * Real(numBDLayers) * 1.001);
        Vec3r maxUZ = m_BMax + Vec3r(spacing * Real(numBDLayers) * 1.001);
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
                    const Vec3r gridPos = minUZ + Vec3r(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disLarge(gen), disLarge(gen), disSmall(gen));
                    m_BDParticles.push_back(ppos);
                }
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
bool Banana::BoxBoundaryObject<Real>::constrainToBoundary(Vec3r& pPos, Vec3r& pVel)
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