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

#include <Banana/Utils/NumberHelpers.h>
#include <ParticleTools/ParticleHelpers.h>
#include <SimulationObjects/BoundaryObjects/BoxBoundary2D.h>

#include <random>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void BoxBoundary2D::setBox(const Vec2r& bMin, const Vec2r& bMax)
{
    SharedPtr<GeometryObject2D::BoxObject> box = std::static_pointer_cast<GeometryObject2D::BoxObject>(m_GeometryObj);
    __BNN_ASSERT(box != nullptr);
    box->setBox(bMin, bMax);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void BoxBoundary2D::generateBoundaryParticles(Real spacing, Int numBDLayers /*= 2*/, bool saveCache /*= false*/)
{
    m_BDParticles.resize(0);
    std::random_device rd;
    std::mt19937       gen(rd());

    std::normal_distribution<Real> disSmall(0, Real(0.02) * spacing);
    std::normal_distribution<Real> disLarge(0, Real(0.1) * spacing);

    ////////////////////////////////////////////////////////////////////////////////
    // plane x < 0
    {
        Vec2r minLX = boxMin() - Vec2r(spacing * Real(numBDLayers * Real(1.001)));
        Vec2r maxLX = boxMax() + Vec2r(spacing * Real(numBDLayers * Real(1.001)));
        maxLX[0] = boxMin()[0];
        Vec2i gridLX = NumberHelpers::createGrid<Int>(minLX, maxLX, spacing);

        for(Int x = 0; x < gridLX[0]; ++x)
        {
            for(Int y = 0; y < gridLX[1]; ++y)
            {
                const Vec2r gridPos = minLX + Vec2r(x + 0.5, y + 0.5) * spacing;
                Vec2r       ppos    = gridPos + Vec2r(disSmall(gen), disLarge(gen));
                m_BDParticles.push_back(ppos);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane x > 0
    {
        Vec2r minUX = boxMin() - Vec2r(spacing * Real(numBDLayers * Real(1.001)));
        Vec2r maxUX = boxMax() + Vec2r(spacing * Real(numBDLayers * Real(1.001)));
        minUX[0] = boxMax()[0];
        Vec2i gridUX = NumberHelpers::createGrid<Int>(minUX, maxUX, spacing);

        for(Int x = 0; x < gridUX[0]; ++x)
        {
            for(Int y = 0; y < gridUX[1]; ++y)
            {
                const Vec2r gridPos = minUX + Vec2r(x + 0.5, y + 0.5) * spacing;
                Vec2r       ppos    = gridPos + Vec2r(disSmall(gen), disLarge(gen));
                m_BDParticles.push_back(ppos);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane y < 0
    {
        Vec2r minLY = boxMin() - Vec2r(spacing * Real(numBDLayers) * Real(1.001));
        Vec2r maxLY = boxMax() + Vec2r(spacing * Real(numBDLayers) * Real(1.001));
        minLY[0] = boxMin()[0];
        maxLY[0] = boxMax()[0];
        maxLY[1] = boxMin()[1];
        Vec2i gridLY = NumberHelpers::createGrid<Int>(minLY, maxLY, spacing);

        for(Int x = 0; x < gridLY[0]; ++x)
        {
            for(Int y = 0; y < gridLY[1]; ++y)
            {
                const Vec2r gridPos = minLY + Vec2r(x + 0.5, y + 0.5) * spacing;
                Vec2r       ppos    = gridPos + Vec2r(disLarge(gen), disSmall(gen));
                m_BDParticles.push_back(ppos);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane y > 0
    {
        Vec2r minUY = boxMin() - Vec2r(spacing * Real(numBDLayers) * Real(1.001));
        Vec2r maxUY = boxMax() + Vec2r(spacing * Real(numBDLayers) * Real(1.001));
        minUY[0] = boxMin()[0];
        maxUY[0] = boxMax()[0];
        minUY[1] = boxMax()[1];
        Vec2i gridUY = NumberHelpers::createGrid<Int>(minUY, maxUY, spacing);

        for(Int x = 0; x < gridUY[0]; ++x)
        {
            for(Int y = 0; y < gridUY[1]; ++y)
            {
                const Vec2r gridPos = minUY + Vec2r(x + 0.5, y + 0.5) * spacing;
                Vec2r       ppos    = gridPos + Vec2r(disLarge(gen), disSmall(gen));
                m_BDParticles.push_back(ppos);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // save file
    if(saveCache)
    {
        __BNN_ASSERT(!m_ParticleFile.empty());
        ParticleHelpers::saveBinary(m_ParticleFile, m_BDParticles, spacing * Real(0.5));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool BoxBoundary2D::constrainToBoundary(Vec2r& pPos, Vec2r& pVel)
{
    bool velChanged = false;

    for(Int l = 0; l < pPos.length(); ++l)
    {
        if(pPos[l] < boxMin()[l] || pPos[l] > boxMax()[l])
        {
            pPos[l]    = MathHelpers::min(MathHelpers::max(pPos[l], boxMin()[l]), boxMax()[l]);
            pVel[l]   *= -m_RestitutionCoeff;
            velChanged = true;
        }
    }

    return velChanged;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana