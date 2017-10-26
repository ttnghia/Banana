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
template<Int N, class RealType>
UInt BoundaryObjectInterface<N, RealType >::generateBoundaryParticles(Vec_VecX<N, RealType>& PDPositions,
                                                                      RealType particleRadius, Int numBDLayers /*= 2*/, bool bUseCache /*= true*/)
{
    Vec_VecX<N, RealType> tmpPositions;
    if(bUseCache && !m_ParticleFile.empty() && FileHelpers::fileExisted(m_ParticleFile)) {
        ParticleSerialization::loadParticle(m_ParticleFile, tmpPositions, particleRadius);
    } else {
        generateBoundaryParticles_Impl(tmpPositions, particleRadius, numBDLayers);
        if(bUseCache && !m_ParticleFile.empty()) {
            ParticleSerialization::saveParticle(m_ParticleFile, tmpPositions, particleRadius);
        }
    }

    PDPositions.insert(PDPositions.end(), tmpPositions.begin(), tmpPositions.end());
    return static_cast<UInt>(tmpPositions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType BoundaryObjectInterface<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bUseCache /*= true*/)
{
    if(bUseCache && m_bSDFGenerated) {
        return ArrayHelpers::interpolateValueLinear(m_Grid.getGridCoordinate(ppos), m_SDF);
    } else {
        return m_GeometryObj->signedDistance(ppos, false);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
VecX<N, RealType> BoundaryObjectInterface<N, RealType >::gradSignedDistance(const VecX<N, RealType>& ppos, RealType dxyz, bool bUseCache /*= true*/)
{
    if(bUseCache && m_bSDFGenerated) {
        return ArrayHelpers::interpolateGradient(m_Grid.getGridCoordinate(ppos), m_SDF);
    } else {
        return m_GeometryObj->gradSignedDistance(ppos, dxyz);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void BoundaryObjectInterface<N, RealType >::generateSDF(const VecX<N, RealType>& domainBMin, const VecX<N, RealType>& domainBMax, RealType sdfCellSize, bool bUseCache /*= true*/)
{
    m_Grid.setGrid(domainBMin, domainBMax, sdfCellSize);

    ////////////////////////////////////////////////////////////////////////////////
    // load sdf from file
    if(bUseCache && !m_SDFFile.empty() && FileHelpers::fileExisted(m_SDFFile)) {
        if(m_SDF.loadFromFile(m_SDFFile)) {
            __BNN_ASSERT(m_SDF.equalSize(m_Grid.getNNodes()));
            m_bSDFGenerated = true;
            return;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    generateSDF_Impl();
    m_bSDFGenerated = true;

    ////////////////////////////////////////////////////////////////////////////////
    // save cache sdf
    if(bUseCache && !m_SDFFile.empty()) {
        m_SDF.saveToFile(m_SDFFile);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void BoundaryObjectInterface<N, RealType >::constrainToBoundary(VecX<N, RealType>& ppos)
{
    const RealType phiVal = signedDistance(ppos) - m_Margin;
    if(phiVal < 0) {
        VecX<N, RealType> grad     = gradSignedDistance(ppos);
        RealType          mag2Grad = glm::length2(grad);

        if(mag2Grad > Tiny) {
            grad /= sqrt(mag2Grad);
            ppos -= phiVal * grad;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void BoundaryObject<2, RealType >::generateSDF_Impl()
{
    m_SDF.resize(m_Grid.getNNodes());
    ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
                                      [&](UInt i, UInt j)
                                      {
                                          m_SDF(i, j) = signedDistance(m_Grid.getWorldCoordinate(i, j), false);
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void BoundaryObject<3, RealType >::generateSDF_Impl()
{
    m_SDF.resize(m_Grid.getNNodes());
    ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          m_SDF(i, j, k) = signedDistance(m_Grid.getWorldCoordinate(i, j, k), false);
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void BoxBoundary<2, RealType >::generateBoundaryParticles_Impl(Vec_Vec2<RealType>& PDPositions, RealType particleRadius, Int numBDLayers /*= 2*/)
{
    RealType spacing = RealType(2.0) * particleRadius;
    PDPositions.resize(0);

    std::random_device                 rd;
    std::mt19937                       gen(rd());
    std::normal_distribution<RealType> disSmall(0, RealType(0.02) * spacing);
    std::normal_distribution<RealType> disLarge(0, RealType(0.1) * spacing);

    ////////////////////////////////////////////////////////////////////////////////
    // plane x < 0
    {
        Vec2r minLX = boxMin() - Vec2r(spacing * RealType(numBDLayers * RealType(1.001)));
        Vec2r maxLX = boxMax() + Vec2r(spacing * RealType(numBDLayers * RealType(1.001)));
        maxLX[0] = boxMin()[0];
        Vec2i gridLX = NumberHelpers::createGrid<Int>(minLX, maxLX, spacing);

        for(Int x = 0; x < gridLX[0]; ++x) {
            for(Int y = 0; y < gridLX[1]; ++y) {
                const Vec2r gridPos = minLX + Vec2r(x + 0.5, y + 0.5) * spacing;
                Vec2r       ppos    = gridPos + Vec2r(disSmall(gen), disLarge(gen));
                PDPositions.push_back(ppos);
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

        for(Int x = 0; x < gridUX[0]; ++x) {
            for(Int y = 0; y < gridUX[1]; ++y) {
                const Vec2r gridPos = minUX + Vec2r(x + 0.5, y + 0.5) * spacing;
                Vec2r       ppos    = gridPos + Vec2r(disSmall(gen), disLarge(gen));
                PDPositions.push_back(ppos);
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

        for(Int x = 0; x < gridLY[0]; ++x) {
            for(Int y = 0; y < gridLY[1]; ++y) {
                const Vec2r gridPos = minLY + Vec2r(x + 0.5, y + 0.5) * spacing;
                Vec2r       ppos    = gridPos + Vec2r(disLarge(gen), disSmall(gen));
                PDPositions.push_back(ppos);
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

        for(Int x = 0; x < gridUY[0]; ++x) {
            for(Int y = 0; y < gridUY[1]; ++y) {
                const Vec2r gridPos = minUY + Vec2r(x + 0.5, y + 0.5) * spacing;
                Vec2r       ppos    = gridPos + Vec2r(disLarge(gen), disSmall(gen));
                PDPositions.push_back(ppos);
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void BoxBoundary<3, RealType >::generateBoundaryParticles_Impl(Vec_Vec3<RealType>& PDPositions, RealType particleRadius, Int numBDLayers /*= 2*/)
{
    RealType spacing = RealType(2.0) * particleRadius;
    PDPositions.resize(0);
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

        for(Int x = 0; x < gridLX[0]; ++x) {
            for(Int y = 0; y < gridLX[1]; ++y) {
                for(Int z = 0; z < gridLX[2]; ++z) {
                    const Vec3r gridPos = minLX + Vec3r(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disSmall(gen), disLarge(gen), disLarge(gen));
                    PDPositions.push_back(ppos);
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

        for(Int x = 0; x < gridUX[0]; ++x) {
            for(Int y = 0; y < gridUX[1]; ++y) {
                for(Int z = 0; z < gridUX[2]; ++z) {
                    const Vec3r gridPos = minUX + Vec3r(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disSmall(gen), disLarge(gen), disLarge(gen));
                    PDPositions.push_back(ppos);
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

        for(Int x = 0; x < gridLY[0]; ++x) {
            for(Int y = 0; y < gridLY[1]; ++y) {
                for(Int z = 0; z < gridLY[2]; ++z) {
                    const Vec3r gridPos = minLY + Vec3r(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disLarge(gen), disSmall(gen), disLarge(gen));
                    PDPositions.push_back(ppos);
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

        for(Int x = 0; x < gridUY[0]; ++x) {
            for(Int y = 0; y < gridUY[1]; ++y) {
                for(Int z = 0; z < gridUY[2]; ++z) {
                    const Vec3r gridPos = minUY + Vec3r(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disLarge(gen), disSmall(gen), disLarge(gen));
                    PDPositions.push_back(ppos);
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

        for(Int x = 0; x < gridLZ[0]; ++x) {
            for(Int y = 0; y < gridLZ[1]; ++y) {
                for(Int z = 0; z < gridLZ[2]; ++z) {
                    const Vec3r gridPos = minLZ + Vec3r(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disLarge(gen), disLarge(gen), disSmall(gen));
                    PDPositions.push_back(ppos);
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

        for(Int x = 0; x < gridUX[0]; ++x) {
            for(Int y = 0; y < gridUX[1]; ++y) {
                for(Int z = 0; z < gridUX[2]; ++z) {
                    const Vec3r gridPos = minUZ + Vec3r(x + 0.5, y + 0.5, z + 0.5) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disLarge(gen), disLarge(gen), disSmall(gen));
                    PDPositions.push_back(ppos);
                }
            }
        }
    }
}
