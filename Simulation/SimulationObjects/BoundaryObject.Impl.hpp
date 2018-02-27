//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
//    /                    Created: 2017 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

template<Int N, class RealType>
UInt BoundaryObject<N, RealType >::generateBoundaryParticles(Vec_VecX<N, RealType>& PDPositions,
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
void BoundaryObject<N, RealType >::constrainToBoundary(VecNr& ppos)
{
    const auto phiVal = signedDistance(ppos);
    if(phiVal < 0) {
        auto grad     = gradSignedDistance(ppos);
        auto mag2Grad = glm::length2(grad);

        if(mag2Grad > Tiny) {
            grad /= sqrt(mag2Grad);
            ppos -= phiVal * grad;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool BoundaryObject<N, RealType >::constrainToBoundary(VecNr& ppos, VecNr& pvel)
{
    const auto phiVal = signedDistance(ppos);
    if(phiVal < 0) {
        auto grad     = gradSignedDistance(ppos);
        auto mag2Grad = glm::length2(grad);

        if(mag2Grad > Tiny) {
            grad /= sqrt(mag2Grad);
            ppos -= phiVal * grad;
            if(m_bReflectVelocityAtBoundary) {
                pvel = glm::reflect(pvel, grad) * m_BoundaryReflectionMultiplier;
                return true;
            }
        }
    }

    return false;
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
        Vec2r minLX = boxMin() - Vec2r(spacing * RealType(numBDLayers * RealType(0.999)));
        Vec2r maxLX = boxMax() + Vec2r(spacing * RealType(numBDLayers * RealType(0.999)));
        maxLX.x = boxMin().x;
        Vec2i gridLX = NumberHelpers::createGrid<Int>(minLX, maxLX, spacing);
        for(Int x = 0; x < gridLX.x; ++x) {
            for(Int y = 0; y < gridLX.y + 1; ++y) {
                const Vec2r gridPos = minLX + Vec2r(x, y) * spacing;
                Vec2r       ppos    = gridPos + Vec2r(disSmall(gen), disLarge(gen));
                PDPositions.push_back(ppos);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane x > 0
    {
        Vec2r minUX = boxMin() - Vec2r(spacing * RealType(numBDLayers * RealType(0.999)));
        Vec2r maxUX = boxMax() + Vec2r(spacing * RealType(numBDLayers * RealType(0.999)));
        minUX.x = boxMax().x;
        Vec2i gridUX = NumberHelpers::createGrid<Int>(minUX, maxUX, spacing);
        for(Int x = 0; x < gridUX.x; ++x) {
            for(Int y = 0; y < gridUX.y + 1; ++y) {
                const Vec2r gridPos = minUX + Vec2r(x + 1, y) * spacing;
                Vec2r       ppos    = gridPos + Vec2r(disSmall(gen), disLarge(gen));
                PDPositions.push_back(ppos);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane y < 0
    {
        Vec2r minLY = boxMin() - Vec2r(spacing * RealType(numBDLayers) * RealType(0.999));
        Vec2r maxLY;
        minLY.x = boxMin().x;
        maxLY.x = boxMax().x;
        maxLY.y = boxMin().y;
        Vec2i gridLY = NumberHelpers::createGrid<Int>(minLY, maxLY, spacing);
        for(Int x = 0; x < gridLY.x + 1; ++x) {
            for(Int y = 0; y < gridLY.y; ++y) {
                const Vec2r gridPos = minLY + Vec2r(x, y) * spacing;
                Vec2r       ppos    = gridPos + Vec2r(disLarge(gen), disSmall(gen));
                PDPositions.push_back(ppos);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane y > 0
    {
        Vec2r minUY;
        Vec2r maxUY = boxMax() + Vec2r(spacing * RealType(numBDLayers) * RealType(0.999));
        minUY.x = boxMin().x;
        minUY.y = boxMax().y;
        maxUY.x = boxMax().x;
        Vec2i gridUY = NumberHelpers::createGrid<Int>(minUY, maxUY, spacing);
        for(Int x = 0; x < gridUY.x + 1; ++x) {
            for(Int y = 0; y < gridUY.y; ++y) {
                const Vec2r gridPos = minUY + Vec2r(x, y + 1) * spacing;
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
        Vec3r minLX = boxMin() - Vec3r(spacing * RealType(numBDLayers * RealType(0.999)));
        Vec3r maxLX = boxMax() + Vec3r(spacing * RealType(numBDLayers * RealType(0.999)));
        maxLX.x = boxMin().x;
        Vec3i gridLX = NumberHelpers::createGrid<Int>(minLX, maxLX, spacing);

        for(Int x = 0; x < gridLX.x; ++x) {
            for(Int y = 0; y < gridLX.y + 1; ++y) {
                for(Int z = 0; z < gridLX.z + 1; ++z) {
                    const Vec3r gridPos = minLX + Vec3r(x, y, z) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disSmall(gen), disLarge(gen), disLarge(gen));
                    PDPositions.push_back(ppos);
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane x > 0
    {
        Vec3r minUX = boxMin() - Vec3r(spacing * RealType(numBDLayers * RealType(0.999)));
        Vec3r maxUX = boxMax() + Vec3r(spacing * RealType(numBDLayers * RealType(0.999)));
        minUX.x = boxMax().x;
        Vec3i gridUX = NumberHelpers::createGrid<Int>(minUX, maxUX, spacing);

        for(Int x = 0; x < gridUX.x; ++x) {
            for(Int y = 0; y < gridUX.y + 1; ++y) {
                for(Int z = 0; z < gridUX.z + 1; ++z) {
                    const Vec3r gridPos = minUX + Vec3r(x + 1, y, z) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disSmall(gen), disLarge(gen), disLarge(gen));
                    PDPositions.push_back(ppos);
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane y < 0
    {
        Vec3r minLY = boxMin() - Vec3r(spacing * RealType(numBDLayers) * RealType(0.999));
        Vec3r maxLY = boxMax() + Vec3r(spacing * RealType(numBDLayers) * RealType(0.999));
        minLY.x = boxMin().x;
        maxLY.x = boxMax().x;
        maxLY.y = boxMin().y;
        Vec3i gridLY = NumberHelpers::createGrid<Int>(minLY, maxLY, spacing);

        for(Int x = 0; x < gridLY.x + 1; ++x) {
            for(Int y = 0; y < gridLY.y; ++y) {
                for(Int z = 0; z < gridLY.z + 1; ++z) {
                    const Vec3r gridPos = minLY + Vec3r(x, y, z) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disLarge(gen), disSmall(gen), disLarge(gen));
                    PDPositions.push_back(ppos);
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane y > 0
    {
        Vec3r minUY = boxMin() - Vec3r(spacing * RealType(numBDLayers) * RealType(0.999));
        Vec3r maxUY = boxMax() + Vec3r(spacing * RealType(numBDLayers) * RealType(0.999));
        minUY.x = boxMin().x;
        minUY.y = boxMax().y;
        maxUY.x = boxMax().x;
        Vec3i gridUY = NumberHelpers::createGrid<Int>(minUY, maxUY, spacing);

        for(Int x = 0; x < gridUY.x + 1; ++x) {
            for(Int y = 0; y < gridUY.y; ++y) {
                for(Int z = 0; z < gridUY.z + 1; ++z) {
                    const Vec3r gridPos = minUY + Vec3r(x, y + 1, z) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disLarge(gen), disSmall(gen), disLarge(gen));
                    PDPositions.push_back(ppos);
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane z < 0
    {
        Vec3r minLZ = boxMin() - Vec3r(spacing * RealType(numBDLayers) * RealType(0.999));
        Vec3r maxLZ;
        minLZ.x = boxMin().x;
        minLZ.y = boxMin().y;
        maxLZ.x = boxMax().x;
        maxLZ.y = boxMax().y;
        maxLZ.z = boxMin().z;
        Vec3i gridLZ = NumberHelpers::createGrid<Int>(minLZ, maxLZ, spacing);

        for(Int x = 0; x < gridLZ.x + 1; ++x) {
            for(Int y = 0; y < gridLZ.y + 1; ++y) {
                for(Int z = 0; z < gridLZ.z; ++z) {
                    const Vec3r gridPos = minLZ + Vec3r(x, y, z) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disLarge(gen), disLarge(gen), disSmall(gen));
                    PDPositions.push_back(ppos);
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // plane z > 0
    {
        Vec3r minUZ;
        Vec3r maxUZ = boxMax() + Vec3r(spacing * RealType(numBDLayers) * RealType(0.999));
        minUZ.x = boxMin().x;
        minUZ.y = boxMin().y;
        minUZ.z = boxMax().z;
        maxUZ.x = boxMax().x;
        maxUZ.y = boxMax().y;
        Vec3i gridUX = NumberHelpers::createGrid<Int>(minUZ, maxUZ, spacing);

        for(Int x = 0; x < gridUX.x + 1; ++x) {
            for(Int y = 0; y < gridUX.y + 1; ++y) {
                for(Int z = 0; z < gridUX.z; ++z) {
                    const Vec3r gridPos = minUZ + Vec3r(x, y, z + 1) * spacing;
                    Vec3r       ppos    = gridPos + Vec3r(disLarge(gen), disLarge(gen), disSmall(gen));
                    PDPositions.push_back(ppos);
                }
            }
        }
    }
}
