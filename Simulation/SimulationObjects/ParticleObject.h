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
#include <Banana/Geometry/GeometryObject.h>
#include <Banana/Geometry/GeometryObjectFactory.h>
#include <Banana/Grid/Grid.h>
#include <ParticleTools/ParticleHelpers.h>
#include <ParticleTools/BlueNoiseRelaxation/LloydRelaxation.h>
#include <ParticleTools/BlueNoiseRelaxation/SPHBasedRelaxation.h>

#include <json.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class ParticleObject
{
public:
    using GeometryPtr = SharedPtr<GeometryObjects::GeometryObject<N, Real> >;
    static constexpr UInt objDimension() noexcept { return static_cast<UInt>(N); }
    ParticleObject() = delete;
    ParticleObject(const String& geometryType) : m_GeometryObj(GeometryObjectFactory::createGeometry<N, Real>(geometryType)) { __BNN_ASSERT(m_GeometryObj != nullptr); }

    String&      name() { return m_MeshFile; }
    String&      meshFile() { return m_MeshFile; }
    String&      particleFile() { return m_ParticleFile; }
    JParams&     parameters() { return m_jParams; }
    GeometryPtr& getGeometry() { return m_GeometryObj; }

    ////////////////////////////////////////////////////////////////////////////////
    void generateParticles(Vector<VecX<N, RealType> >& positions, Vector<VecX<N, RealType> >& velocities, Real particleRadius, bool bUseCache = true)
    {
        if(bUseCache && !m_ParticleFile.empty() && FileHelpers::fileExisted(m_ParticleFile))
        {
            positions.resize(0);
            ParticleHelpers::loadBinaryAndDecompress(m_ParticleFile, positions, m_ParticleRadius);
            generateVelocities();

            return;
        }

        ////////////////////////////////////////////////////////////////////////////////
        generatePositions(positions, particleRadius);
        relaxPositions(positions, particleRadius);
        generateVelocities(positions, velocities);
        ////////////////////////////////////////////////////////////////////////////////

        if(bUseCache && !m_ParticleFile.empty())
            ParticleHelpers::compressAndSaveBinary(m_ParticleFile, positions, m_ParticleRadius);
    }

    virtual void makeReady() {} // todo: need this?
    virtual void advanceFrame() {}

protected:
    void generatePositions(Vector<VecX<N, RealType> >& positions, Real particleRadius)
    {
        // Firstly, generate a signed distance field
        Grid<VecX<N, RealType>::length(), Real> grid;
    }

    void relaxPositions(Vector<VecX<N, RealType> >& positions, Real particleRadius)
    {
        bool   bRelax      = false;
        String relaxMethod = String("SPH");
        JSONHelpers::readBool(m_jParams, bRelax, "RelaxPosition");
        JSONHelpers::readValue(m_jParams, relaxMethod, "RelaxMethod");

        if(bRelax)
        {
            //if(relaxMethod == "SPH" || relaxMethod == "SPHBased")
            //    SPHBasedRelaxation::relaxPositions(positions, particleRadius);
            //else
            //{
            //    Vector<VecX<N, RealType> > denseSamples;
            //    Real                       denseSampleRatio = 0.1;
            //    JSONHelpers::readValue(m_jParams, denseSampleRatio, "DenseSampleRatio");

            //    generatePositions(denseSamples, particleRadius * denseSampleRatio);
            //    LloydRelaxation::relaxPositions(denseSamples, positions);
            //}
        }
    }

    void generateVelocities(Vector<VecX<N, RealType> >& positions, Vector<VecX<N, RealType> >& velocities)
    {
        VecX<N, RealType> initVelocity = VecX<N, RealType>(0);
        JSONHelpers::readVector(m_jParams, initVelocity, "InitialVelocity");
        velocities.resize(positions.size(), initVelocity);
    }

    String m_ObjName      = String("NoName");
    String m_MeshFile     = String("");
    String m_ParticleFile = String("");

    bool m_bDynamics     = false;
    bool m_bSDFGenerated = false;

    JParams     m_jParams;
    GeometryPtr m_GeometryObj = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana