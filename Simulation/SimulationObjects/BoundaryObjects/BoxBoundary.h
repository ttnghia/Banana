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

#include <SimulationObjects/BoundaryObjects/BoundaryObject.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class BoxBoundaryInterface : public BoundaryObject<N, RealType>
{
public:
    BoxBoundaryInterface() : BoundaryObject<N, RealType>("Box") {}
    BoxBoundaryInterface(const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax) : BoundaryObject<N, RealType>("Box") { setBox(bMin, bMax); }

    virtual bool constrainToBoundary(VecX<N, RealType>& ppos, VecX<N, RealType>& pvel) override
    {
        bool velChanged = false;

        for(Int l = 0; l < ppos.length(); ++l)
        {
            if(ppos[l] < boxMin()[l] || ppos[l] > boxMax()[l])
            {
                ppos[l]    = MathHelpers::min(MathHelpers::max(ppos[l], boxMin()[l]), boxMax()[l]);
                pvel[l]   *= -m_RestitutionCoeff;
                velChanged = true;
            }
        }

        return velChanged;
    }

    void setBox(const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax)
    {
        auto box = std::static_pointer_cast<GeometryObjects::BoxObject<N, RealType> >(m_GeometryObj);
        __BNN_ASSERT(box != nullptr);
        box->setBox(bMin, bMax);
    }

    const VecX<N, RealType>& boxMin() const noexcept { return m_GeometryObj->aabbBoxMin(); }
    const VecX<N, RealType>& boxMax() const noexcept { return m_GeometryObj->aabbBoxMax(); }

protected:
    void parseParameters()
    {
        __BNN_ASSERT(m_jParams.find("BoxMin") != m_jParams.end());
        __BNN_ASSERT(m_jParams.find("BoxMax") != m_jParams.end());

        VecX<N, RealType> bMin, bMax;
        RealType          offset = RealType(1e-5);
        JSONHelpers::readVector(m_jParams, bMin, "BoxMin");
        JSONHelpers::readVector(m_jParams, bMax, "BoxMax");
        JSONHelpers::readValue(m_jParams, offset, "Offset");

        bMin += VecX<N, RealType>(offset);
        bMax -= VecX<N, RealType>(offset);

        setBox(bMin, bMax);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class BoxBoundary : public BoxBoundaryInterface<N, RealType>
{};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class BoxBoundary<2, RealType> : public BoxBoundaryInterface<2, RealType>
{
public:
    BoxBoundary() {}
    BoxBoundary(const Vec2<RealType>& bMin, const Vec2<RealType>& bMax) : BoxBoundaryInterface<2, RealType>(bMin, bMax) {}
    void generateBoundaryParticles(RealType spacing, Int numBDLayers /*= 2*/, bool saveCache /*= false*/)
    {
        m_BDParticles.resize(0);
        std::random_device rd;
        std::mt19937       gen(rd());

        std::normal_distribution<RealType> disSmall(0, RealType(0.02) * spacing);
        std::normal_distribution<RealType> disLarge(0, RealType(0.1) * spacing);

        ////////////////////////////////////////////////////////////////////////////////
        // plane x < 0
        {
            Vec2r minLX = boxMin() - Vec2r(spacing * RealType(numBDLayers * RealType(1.001)));
            Vec2r maxLX = boxMax() + Vec2r(spacing * RealType(numBDLayers * RealType(1.001)));
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
            Vec2r minUX = boxMin() - Vec2r(spacing * RealType(numBDLayers * RealType(1.001)));
            Vec2r maxUX = boxMax() + Vec2r(spacing * RealType(numBDLayers * RealType(1.001)));
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
            Vec2r minLY = boxMin() - Vec2r(spacing * RealType(numBDLayers) * RealType(1.001));
            Vec2r maxLY = boxMax() + Vec2r(spacing * RealType(numBDLayers) * RealType(1.001));
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
            Vec2r minUY = boxMin() - Vec2r(spacing * RealType(numBDLayers) * RealType(1.001));
            Vec2r maxUY = boxMax() + Vec2r(spacing * RealType(numBDLayers) * RealType(1.001));
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
            ParticleHelpers::saveBinary(m_ParticleFile, m_BDParticles, spacing * RealType(0.5));
        }
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class BoxBoundary<3, RealType> : public BoxBoundaryInterface<3, RealType>
{
public:
    BoxBoundary() {}
    BoxBoundary(const Vec3<RealType>& bMin, const Vec3<RealType>& bMax) : BoxBoundaryInterface<3, RealType>(bMin, bMax) {}
    void generateBoundaryParticles(RealType spacing, Int numBDLayers /*= 2*/, bool saveCache /*= false*/)
    {
        m_BDParticles.resize(0);
        std::random_device rd;
        std::mt19937       gen(rd());

        std::normal_distribution<RealType> disSmall(0, RealType(0.02) * spacing);
        std::normal_distribution<RealType> disLarge(0, RealType(0.1) * spacing);

        ////////////////////////////////////////////////////////////////////////////////
        // plane x < 0
        {
            Vec3r minLX = boxMin() - Vec3r(spacing * RealType(numBDLayers * RealType(1.001)));
            Vec3r maxLX = boxMax() + Vec3r(spacing * RealType(numBDLayers * RealType(1.001)));
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
            Vec3r minUX = boxMin() - Vec3r(spacing * RealType(numBDLayers * RealType(1.001)));
            Vec3r maxUX = boxMax() + Vec3r(spacing * RealType(numBDLayers * RealType(1.001)));
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
            Vec3r minLY = boxMin() - Vec3r(spacing * RealType(numBDLayers) * RealType(1.001));
            Vec3r maxLY = boxMax() + Vec3r(spacing * RealType(numBDLayers) * RealType(1.001));
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
            Vec3r minUY = boxMin() - Vec3r(spacing * RealType(numBDLayers) * RealType(1.001));
            Vec3r maxUY = boxMax() + Vec3r(spacing * RealType(numBDLayers) * RealType(1.001));
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
            Vec3r minLZ = boxMin() - Vec3r(spacing * RealType(numBDLayers) * RealType(1.001));
            Vec3r maxLZ = boxMax() + Vec3r(spacing * RealType(numBDLayers) * RealType(1.001));
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
            Vec3r minUZ = boxMin() - Vec3r(spacing * RealType(numBDLayers) * RealType(1.001));
            Vec3r maxUZ = boxMax() + Vec3r(spacing * RealType(numBDLayers) * RealType(1.001));
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
            ParticleHelpers::saveBinary(m_ParticleFile, m_BDParticles, spacing * RealType(0.5));
        }
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana