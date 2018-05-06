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
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PIC_Parameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Parameters<N, RealType>::parseParameters(const JParams& jParams)
{
    SimulationParameters<N, RealType>::parseParameters(jParams);
    ////////////////////////////////////////////////////////////////////////////////
    JSONHelpers::readBool(jParams, bExitIfPressureProjectionFailed, "ExitIfPressureProjectionFailed");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Parameters<N, RealType>::makeReady()
{
    SimulationParameters<N, RealType>::makeReady();
    sdfRadius = cellSize * RealType(1.01 * sqrt(N) / 2.0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Parameters<N, RealType>::printParams(const SharedPtr<Logger>& logger)
{
    logger->printLog(String("PIC parameters:"));
    SimulationParameters<N, RealType>::printParams(logger);
    logger->printLogIndent(String("Fluid SDF radius: ") + std::to_string(sdfRadius));
    logger->printLogIndent(String("Exit if pressure projection failed: ") + (bExitIfPressureProjectionFailed ? String("Yes") : String("No")));
    ////////////////////////////////////////////////////////////////////////////////
    logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PIC_Data
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Data<N, RealType>::PIC_ParticleData::reserve(UInt nParticles)
{
    positions.reserve(nParticles);
    velocities.reserve(nParticles);
    objectIndex.reserve(nParticles);
    aniKernelCenters.reserve(nParticles);
    aniKernelMatrices.reserve(nParticles);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Data<N, RealType>::PIC_ParticleData::addParticles(const Vec_VecN& newPositions, const Vec_VecN& newVelocities, const JParams& jParams)
{
    __BNN_UNUSED(jParams);
    __BNN_REQUIRE(newPositions.size() == newVelocities.size());
    positions.insert(positions.end(), newPositions.begin(), newPositions.end());
    velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());
    ////////////////////////////////////////////////////////////////////////////////
    // add the object index for new particles to the list
    objectIndex.insert(objectIndex.end(), newPositions.size(), static_cast<UInt16>(nObjects));
    ++nObjects;                         // increase the number of objects
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
UInt PIC_Data<N, RealType>::PIC_ParticleData::removeParticles(const Vec_Int8& removeMarker)
{
    __BNN_REQUIRE(removeMarker.size() == positions.size());
    if(!STLHelpers::contain(removeMarker, Int8(1))) {
        return 0u;
    }

    STLHelpers::eraseByMarker(positions,   removeMarker);
    STLHelpers::eraseByMarker(velocities,  removeMarker);
    STLHelpers::eraseByMarker(objectIndex, removeMarker);
    ////////////////////////////////////////////////////////////////////////////////
    return static_cast<UInt>(removeMarker.size() - positions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Data<N, RealType>::PIC_GridData::resize(const VecX<N, UInt>& gridSize)
{
    for(Int d = 0; d < N; ++d) {
        auto extra = VecX<N, UInt>(0);
        extra[d] = 1u;
        ////////////////////////////////////////////////////////////////////////////////
        velocities[d].resize(gridSize + extra, 0);
        weights[d].resize(gridSize + extra, 0);
        valids[d].resize(gridSize + extra, 0);
        extrapolates[d].resize(gridSize + extra, 0);
        tmpVels[d].resize(gridSize + extra, 0);
        tmpValids[d].resize(gridSize + extra, 0);
    }
    activeCellIdx.resize(gridSize, 0);
    fluidSDFLock.resize(gridSize);
    fluidSDF.resize(gridSize, 0);
    boundarySDF.resize(gridSize + VecX<N, UInt>(1), 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Data<N, RealType>::initialize()
{
    particleData = std::make_shared<PIC_ParticleData>();
    gridData     = std::make_shared<PIC_GridData>();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Data<N, RealType>::makeReady(const SharedPtr<SimulationParameters<N, RealType>>& simParams)
{
    if(simParams->maxNParticles > 0) {
        particleData->reserve(simParams->maxNParticles);
    }
    particleData->setupNeighborSearch(simParams->cellSize);
    particleData->defaultParticleMass = simParams->defaultParticleMass;

    grid.setGrid(simParams->domainBMin, simParams->domainBMax, simParams->cellSize);
    gridData->resize(grid.getNCells());
    matrix.reserve(grid.getNTotalCells());
    rhs.reserve(grid.getNTotalCells());
    pressure.reserve(grid.getNTotalCells());

    pcgSolver.reserve(grid.getNTotalCells());
    pcgSolver.setSolverParameters(simParams->CGRelativeTolerance, simParams->maxCGIteration);
    pcgSolver.setPreconditioners(PCGSolver<RealType>::MICCL0_SYMMETRIC);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
