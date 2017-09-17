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

#include <SimulationObjects/BoundaryObjects/BoxBoundary3D.h>

#include <random>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void BoxBoundary3D::setParameters(const nlohmann::json& jParams)
{
    __BNN_ASSERT(jParams.find("BoxMin") != jParams.end());
    __BNN_ASSERT(jParams.find("BoxMax") != jParams.end());

    Vec3r bMin, bMax;
    Real  offset = 1e-5;
    JSONHelpers::readVector(jParams, bMin, "BoxMin");
    JSONHelpers::readVector(jParams, bMax, "BoxMax");
    JSONHelpers::readValue(jParams, offset, "Offset");

    bMin += Vec3r(offset);
    bMax -= Vec3r(offset);

    setBox(bMin, bMax);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void BoxBoundary3D::generateBoundaryParticles(Real spacing, Int numBDLayers /*= 2*/, bool saveCache /*= false*/)
{
    m_BDParticles.resize(0);
    std::random_device rd;
    std::mt19937       gen(rd());

    std::normal_distribution<Real> disSmall(0, Real(0.02) * spacing);
    std::normal_distribution<Real> disLarge(0, Real(0.1) * spacing);

    ////////////////////////////////////////////////////////////////////////////////
    // plane x < 0
    {
        Vec3r minLX = boxMin() - Vec3r(spacing * Real(numBDLayers * Real(1.001)));
        Vec3r maxLX = boxMax() + Vec3r(spacing * Real(numBDLayers * Real(1.001)));
        maxLX[0] = boxMin()[0];
        Vec3i gridLX = NumberHelpers::createGrid<Int>(minLX, maxLX, spacing);

        for(Int x = 0; x < gridLX[0]; ++x)
        {
            for(Int y = 0; y < gridLX[1]; ++y)
            {
                for(Int z = 0; z < gridLX[2]; ++z)
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
        Vec3r minUX = boxMin() - Vec3r(spacing * Real(numBDLayers * Real(1.001)));
        Vec3r maxUX = boxMax() + Vec3r(spacing * Real(numBDLayers * Real(1.001)));
        minUX[0] = boxMax()[0];
        Vec3i gridUX = NumberHelpers::createGrid<Int>(minUX, maxUX, spacing);

        for(Int x = 0; x < gridUX[0]; ++x)
        {
            for(Int y = 0; y < gridUX[1]; ++y)
            {
                for(Int z = 0; z < gridUX[2]; ++z)
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
        Vec3r minLY = boxMin() - Vec3r(spacing * Real(numBDLayers) * Real(1.001));
        Vec3r maxLY = boxMax() + Vec3r(spacing * Real(numBDLayers) * Real(1.001));
        minLY[0] = boxMin()[0];
        maxLY[0] = boxMax()[0];
        maxLY[1] = boxMin()[1];
        Vec3i gridLY = NumberHelpers::createGrid<Int>(minLY, maxLY, spacing);

        for(Int x = 0; x < gridLY[0]; ++x)
        {
            for(Int y = 0; y < gridLY[1]; ++y)
            {
                for(Int z = 0; z < gridLY[2]; ++z)
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
        Vec3r minUY = boxMin() - Vec3r(spacing * Real(numBDLayers) * Real(1.001));
        Vec3r maxUY = boxMax() + Vec3r(spacing * Real(numBDLayers) * Real(1.001));
        minUY[0] = boxMin()[0];
        maxUY[0] = boxMax()[0];
        minUY[1] = boxMax()[1];
        Vec3i gridUY = NumberHelpers::createGrid<Int>(minUY, maxUY, spacing);

        for(Int x = 0; x < gridUY[0]; ++x)
        {
            for(Int y = 0; y < gridUY[1]; ++y)
            {
                for(Int z = 0; z < gridUY[2]; ++z)
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
        Vec3r minLZ = boxMin() - Vec3r(spacing * Real(numBDLayers) * Real(1.001));
        Vec3r maxLZ = boxMax() + Vec3r(spacing * Real(numBDLayers) * Real(1.001));
        minLZ[0] = boxMin()[0];
        maxLZ[0] = boxMax()[0];
        minLZ[1] = boxMin()[1];
        maxLZ[1] = boxMax()[1];
        maxLZ[2] = boxMin()[2];
        Vec3i gridLZ = NumberHelpers::createGrid<Int>(minLZ, maxLZ, spacing);

        for(Int x = 0; x < gridLZ[0]; ++x)
        {
            for(Int y = 0; y < gridLZ[1]; ++y)
            {
                for(Int z = 0; z < gridLZ[2]; ++z)
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
        Vec3r minUZ = boxMin() - Vec3r(spacing * Real(numBDLayers) * Real(1.001));
        Vec3r maxUZ = boxMax() + Vec3r(spacing * Real(numBDLayers) * Real(1.001));
        minUZ[0] = boxMin()[0];
        maxUZ[0] = boxMax()[0];
        minUZ[1] = boxMin()[1];
        maxUZ[1] = boxMax()[1];
        minUZ[2] = boxMax()[2];
        Vec3i gridUX = NumberHelpers::createGrid<Int>(minUZ, maxUZ, spacing);

        for(Int x = 0; x < gridUX[0]; ++x)
        {
            for(Int y = 0; y < gridUX[1]; ++y)
            {
                for(Int z = 0; z < gridUX[2]; ++z)
                {
                    const Vec3r gridPos = minUZ + Vec3r(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disLarge(gen), disLarge(gen), disSmall(gen));
                    m_BDParticles.push_back(ppos);
                }
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
bool BoxBoundary3D::constrainToBoundary(Vec3r& pPos, Vec3r& pVel)
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
void BoxBoundary3D::setBox(const Vec3r& bMin, const Vec3r& bMax)
{
    SharedPtr<GeometryObject3D::BoxObject> box = std::static_pointer_cast<GeometryObject3D::BoxObject>(m_GeometryObj);
    __BNN_ASSERT(box != nullptr);
    box->setBox(bMin, bMax);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana