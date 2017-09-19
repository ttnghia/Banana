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

#include <Banana/Setup.h>
#include <Banana/Array/Array.h>
#include <Banana/Grid/Grid.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Utils/FileHelpers.h>
#include <Banana/Utils/JSONHelpers.h>
#include <Banana/Geometry/GeometryObject.h>
#include <Banana/Geometry/GeometryObjectFactory.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleTools/ParticleHelpers.h>

#include <json.hpp>

#include <cassert>
#include <locale>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class BoundaryObjectInterface
{
public:
    using GeometryPtr = SharedPtr<GeometryObjects::GeometryObject<N, RealType> >;
    static constexpr UInt objDimension() noexcept { return static_cast<UInt>(N); }
    BoundaryObjectInterface() = delete;
    BoundaryObjectInterface(const String& geometryType) : m_GeometryObj(GeometryObjectFactory::createGeometry<N, RealType>(geometryType)) { __BNN_ASSERT(m_GeometryObj != nullptr); }

    String& name() { return m_MeshFile; }
    String& meshFile() { return m_MeshFile; }
    String& particleFile() { return m_ParticleFile; }
    String& SDFFile() { return m_SDFFile; }

    RealType& margin() { return m_Margin; }
    RealType& restitution() { return m_RestitutionCoeff; }
    bool&     isDynamic() { return m_bDynamics; }
    void      setParameters(const nlohmann::json& jParams) { m_jParams = jParams; parseParameters(); }

    UInt                        getNumBDParticles() const noexcept { return static_cast<UInt>(m_BDParticles.size()); }
    Vector<VecX<N, RealType> >& getBDParticles() { return m_BDParticles; }
    const Array<N, RealType>&   getSDF() { return m_SDF; }
    GeometryPtr&                getGeometry() { return m_GeometryObj; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() {} // todo: need this?
    virtual void advanceFrame() {}
    virtual void generateBoundaryParticles(RealType spacing, int numBDLayers = 2, bool useCache = true) { __BNN_UNIMPLEMENTED_FUNC }


    RealType signedDistance(const VecX<N, RealType>& ppos, bool bUseCache = true)
    {
        if(bUseCache && m_bSDFGenerated)
            return ArrayHelpers::interpolateValueLinear(m_Grid.getGridCoordinate(ppos), m_SDF);
        else
            return m_GeometryObj->signedDistance(ppos, false);
    }

    VecX<N, RealType> gradientSignedDistance(const VecX<N, RealType>& ppos, RealType dxyz = RealType(1.0 / 512.0), bool bUseCache = true)
    {
        if(bUseCache && m_bSDFGenerated)
            return ArrayHelpers::interpolateGradient(m_Grid.getGridCoordinate(ppos), m_SDF);
        else
            return m_GeometryObj->gradientSignedDistance(ppos);
    }

    void generateSDF(const VecX<N, RealType>& domainBMin, const VecX<N, RealType>& domainBMax, RealType sdfCellSize = RealType(1.0 / 512.0), bool bUseCache = false)
    {
        m_Grid.setGrid(domainBMin, domainBMax, sdfCellSize);

        ////////////////////////////////////////////////////////////////////////////////
        // load sdf from file
        if(bUseCache && !m_SDFFile.empty() && FileHelpers::fileExisted(m_SDFFile))
        {
            if(m_SDF.loadFromFile(m_SDFFile))
            {
                __BNN_ASSERT(m_SDF.equalSize(m_Grid.getNNodes()));
                m_bSDFGenerated = true;
                return;
            }
        }

        ////////////////////////////////////////////////////////////////////////////////
        computeSDF();
        m_bSDFGenerated = true;

        ////////////////////////////////////////////////////////////////////////////////
        // save cache sdf
        if(bUseCache && !m_SDFFile.empty())
            m_SDF.saveToFile(m_SDFFile);
    }

    virtual bool constrainToBoundary(VecX<N, RealType>& ppos, VecX<N, RealType>& pvel) /*= 0;*/ { return true; }

protected:
    virtual void parseParameters() { __BNN_UNIMPLEMENTED_FUNC }
    virtual void computeSDF() { __BNN_UNIMPLEMENTED_FUNC }

    RealType m_Margin           = RealType(0.0);
    RealType m_RestitutionCoeff = ParticleSolverConstants::DefaultBoundaryRestitution;

    JParams                    m_jParams;
    GeometryPtr                m_GeometryObj;
    Array<N, RealType>         m_SDF;
    Grid<N, RealType>          m_Grid;
    Vector<VecX<N, RealType> > m_BDParticles;

    String m_MeshFile     = String("");
    String m_ParticleFile = String("");
    String m_SDFFile      = String("");

    bool m_bDynamics     = false;
    bool m_bSDFGenerated = false;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
template<Int N, class RealType>
class BoundaryObject : public BoundaryObjectInterface<N, RealType>
{};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class BoundaryObject<2, RealType> : public BoundaryObjectInterface<2, RealType>
{
public:
    BoundaryObject(const String& geometryType) : BoundaryObjectInterface(geometryType) {}

    void computeSDF()
    {
        m_SDF.resize(m_Grid.getNNodes());
        ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
                                          [&](UInt i, UInt j)
                                          {
                                              m_SDF(i, j) = signedDistance(m_Grid.getWorldCoordinate(i, j));
                                          });
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
template<class RealType>
class BoundaryObject<3, RealType> : public BoundaryObjectInterface<3, RealType>
{
public:
    BoundaryObject(const String& geometryType) : BoundaryObjectInterface(geometryType) {}

    void computeSDF()
    {
        m_SDF.resize(m_Grid.getNNodes());
        ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
                                          [&](UInt i, UInt j, UInt k)
                                          {
                                              m_SDF(i, j, k) = signedDistance(m_Grid.getWorldCoordinate(i, j, k));
                                          });
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana