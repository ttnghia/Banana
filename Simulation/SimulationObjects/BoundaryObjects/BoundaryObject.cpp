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

#include <Banana/Data/DataIO.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Array/ArrayHelpers.h>

#include <SimulationObjects/BoundaryObjects/BoundaryObject.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// BoundaryObject2D
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real BoundaryObject2D::signedDistance(const Vec2r& ppos, bool bUseCache /*= true*/)
{
    if(bUseCache && m_bSDFGenerated)
        return ArrayHelpers::interpolateValueLinear(m_Grid.getGridCoordinate(ppos), m_SDF);
    else
        return -m_GeometryObj->signedDistance(ppos);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec2r BoundaryObject2D::gradientSignedDistance(const Vec2r& ppos, Real dxyz, bool bUseCache /*= true*/)
{
    if(bUseCache && m_bSDFGenerated)
        return ArrayHelpers::interpolateGradient(m_Grid.getGridCoordinate(ppos), m_SDF);
    else
        return m_GeometryObj->gradientSignedDistance(ppos);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void BoundaryObject2D::generateSignedDistanceField(const Vec2r& domainBMin, const Vec2r& domainBMax, Real sdfCellSize /*= Real(1.0 / 512.0)*/, bool bUseFile /*= false*/)
{
    m_Grid.setGrid(domainBMin, domainBMax, sdfCellSize);

    ////////////////////////////////////////////////////////////////////////////////
    // load sdf from file
    if(bUseFile && !m_SDFFile.empty() && FileHelpers::fileExisted(m_SDFFile))
    {
        if(m_SDF.loadFromFile(m_SDFFile))
        {
            __BNN_ASSERT(m_SDF.equalSize(m_Grid.getNumNodes()));
            m_bSDFGenerated = true;
            return;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    m_SDF.resize(m_Grid.getNumNodes());
    ParallelFuncs::parallel_for<UInt>(m_Grid.getNumNodes(),
                                      [&](UInt i, UInt j)
                                      {
                                          m_SDF(i, j) = signedDistance(m_Grid.getWorldCoordinate(i, j));
                                      });
    m_bSDFGenerated = true;

    ////////////////////////////////////////////////////////////////////////////////
    // save cache sdf
    if(bUseFile && !m_SDFFile.empty())
        m_SDF.saveToFile(m_SDFFile);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// BoundaryObject3D
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real BoundaryObject3D::signedDistance(const Vec3r& ppos, bool bUseCache /*= true*/)
{
    if(bUseCache && m_bSDFGenerated)
        return ArrayHelpers::interpolateValueLinear(m_Grid.getGridCoordinate(ppos), m_SDF);
    else
        return -m_GeometryObj->signedDistance(ppos);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3r BoundaryObject3D::gradientSignedDistance(const Vec3r& ppos, Real dxyz, bool bUseCache /*= true*/)
{
    if(bUseCache && m_bSDFGenerated)
        return ArrayHelpers::interpolateGradient(m_Grid.getGridCoordinate(ppos), m_SDF);
    else
        return m_GeometryObj->gradientSignedDistance(ppos);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void BoundaryObject3D::generateSignedDistanceField(const Vec3r& domainBMin, const Vec3r& domainBMax, Real sdfCellSize /*= Real(1.0 / 512.0)*/, bool bUseFile /*= false*/)
{
    m_Grid.setGrid(domainBMin, domainBMax, sdfCellSize);
    printf("obj: %s\n", m_GeometryObj->name().c_str());
    ////////////////////////////////////////////////////////////////////////////////
    // load sdf from file
    if(bUseFile && !m_SDFFile.empty() && FileHelpers::fileExisted(m_SDFFile))
    {
        if(m_SDF.loadFromFile(m_SDFFile))
        {
            __BNN_ASSERT(m_SDF.equalSize(m_Grid.getNumNodes()));
            m_bSDFGenerated = true;
            return;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    m_SDF.resize(m_Grid.getNumNodes());
    ParallelFuncs::parallel_for<UInt>(m_Grid.getNumNodes(),
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          m_SDF(i, j, k) = signedDistance(m_Grid.getWorldCoordinate(i, j, k));
                                      });
    m_bSDFGenerated = true;

    ////////////////////////////////////////////////////////////////////////////////
    // save cache sdf
    if(bUseFile && !m_SDFFile.empty())
        m_SDF.saveToFile(m_SDFFile);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana