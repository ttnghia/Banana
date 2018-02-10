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

#pragma once

#include <Banana/Grid/Grid.h>
#include <ParticleSolvers/ParticleSolver.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleSolvers/ParticleSolverFactory.h>
#include <ParticleSolvers/MPM/Snow3DData.h>
#include <Banana/Array/Array.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationParameters_Snow3D : public SimulationParameters
{
    SimulationParameters_Snow3D() = default;

    ////////////////////////////////////////////////////////////////////////////////
    Real CFLFactor           = 0.04_f;
    Real PIC_FLIP_ratio      = SolverDefaultParameters::PIC_FLIP_Ratio;
    Real minTimestep         = SolverDefaultParameters::MinTimestep;
    Real maxTimestep         = SolverDefaultParameters::MaxTimestep;
    Real boundaryRestitution = SolverDefaultParameters::BoundaryRestitution;

    Real CGRelativeTolerance = SolverDefaultParameters::CGRelativeTolerance;
    UInt maxCGIteration      = SolverDefaultParameters::CGMaxIteration;

    Real thresholdCompression = 1.0_f - 1.9e-2_f; //Fracture threshold for compression (1-2.5e-2)
    Real thresholdStretching  = 1.0_f + 7.5e-3_f; //Fracture threshold for stretching (1+7.5e-3)
    Real hardening            = 5.0_f;            //How much plastic deformation strengthens material (10)
    Real materialDensity      = 100.0_f;          //Density of snow in kg/m^2 (400 for 3d)
    Real YoungsModulus        = 1.5e5_f;          //Young's modulus (springiness) (1.4e5)
    Real PoissonsRatio        = 0.2_f;            //Poisson's ratio (transverse/axial strain ratio) (.2)
    Real implicitRatio        = 0_f;              //Percentage that should be implicit vs explicit for velocity update

    Real maxImplicitError = 1e4_f;                //Maximum allowed error for conjugate residual
    Real minImplicitError = 1e-4_f;               //Minimum allowed error for conjugate residual

    Int kernelSpan = 2;

    Real  cellSize                    = SolverDefaultParameters::CellSize;
    Real  ratioCellSizeParticleRadius = 2.0_f;
    Vec3r domainBMin                  = Vec3r(0.0);
    Vec3r domainBMax                  = Vec3r(1.0);

    // the following need to be computed


    Real  cellArea;
    Vec3r movingBMin;
    Vec3r movingBMax;

    Real lambda, mu;         //Lame parameters (_s denotes starting configuration)

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() override
    {
        particleRadius = cellSize / ratioCellSizeParticleRadius;
        particleMass   = particleRadius * particleRadius * materialDensity;

        cellArea   = cellSize * cellSize;
        movingBMin = domainBMin + Vec3r(cellSize * SolverDefaultParameters::NExpandCells);
        movingBMax = domainBMax - Vec3r(cellSize * SolverDefaultParameters::NExpandCells);

        lambda = YoungsModulus * PoissonsRatio / ((1.0_f + PoissonsRatio) * (1.0_f - 2.0_f * PoissonsRatio)),
        mu     = YoungsModulus / (2.0_f + 2.0_f * PoissonsRatio);
    }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void printParams(const SharedPtr<Logger>& logger) override
    {
        logger->printLog("MPM-3D simulation parameters:");
        logger->printLogIndent("Maximum timestep: " + NumberHelpers::formatToScientific(maxTimestep));
        logger->printLogIndent("CFL factor: " + std::to_string(CFLFactor));
        logger->printLogIndent("PIC/FLIP ratio: " + std::to_string(PIC_FLIP_ratio));

        logger->printLogIndent("Domain box: " + NumberHelpers::toString(domainBMin) + String(" -> ") + NumberHelpers::toString(domainBMax));
        logger->printLogIndent("Moving box: " + NumberHelpers::toString(movingBMin) + String(" -> ") + NumberHelpers::toString(movingBMax));
        logger->printLogIndent("Cell size: " + std::to_string(cellSize));
        Vec3ui numDomainCells(static_cast<UInt>(ceil((domainBMax[0] - domainBMin[0]) / cellSize)),
                              static_cast<UInt>(ceil((domainBMax[1] - domainBMin[1]) / cellSize)),
                              static_cast<UInt>(ceil((domainBMax[2] - domainBMin[2]) / cellSize)));
        Vec3ui numMovingCells(static_cast<UInt>(ceil((movingBMax[0] - movingBMin[0]) / cellSize)),
                              static_cast<UInt>(ceil((movingBMax[1] - movingBMin[1]) / cellSize)),
                              static_cast<UInt>(ceil((movingBMax[2] - movingBMin[2]) / cellSize)));
        logger->printLogIndent("Number of cells: " + std::to_string(numDomainCells[0] * numDomainCells[1]));
        logger->printLogIndent("Number of nodes: " + std::to_string((numDomainCells[0] + 1u) * (numDomainCells[1] + 1u)));
        logger->printLogIndent("Grid resolution: " + NumberHelpers::toString(numDomainCells));
        logger->printLogIndent("Moving grid resolution: " + NumberHelpers::toString(numMovingCells));

        logger->printLogIndent("Boundary restitution: " + std::to_string(boundaryRestitution));
        logger->printLogIndent("ConjugateGradient solver tolerance: " + NumberHelpers::formatToScientific(CGRelativeTolerance));
        logger->printLogIndent("Max CG iterations: " + NumberHelpers::formatToScientific(maxCGIteration));

        logger->printLogIndent("Particle radius: " + std::to_string(particleRadius));
        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationData_Snow3D
{
    struct ParticleData : public ParticleSimulationData<3, Real>
    {
        Vec_Real    volumes, densities;
        Vec_Mat3x3r velocityGradients;

        //Deformation gradient (elastic and plastic parts)
        Vec_Mat3x3r elasticDeformGrad, plasticDeformGrad;

        //Cached SVD's for elastic deformation gradient
        Vec_Mat3x3r svd_w, svd_v;
        Vec_Vec3r   svd_e;

        //Cached polar decomposition
        Vec_Mat3x3r polar_r, polar_s;

        //Grid interpolation weights
        Vec_Vec3r particleGridPos;
        Vec_Vec3r weightGradients;                         // * 16
        Vec_Real  weights;                                 // * 16



        virtual void reserve(UInt nParticles)
        {
            positions.reserve(nParticles);
            velocities.reserve(nParticles);
            volumes.reserve(nParticles);
            densities.reserve(nParticles);
            velocityGradients.reserve(nParticles);

            elasticDeformGrad.reserve(nParticles);
            plasticDeformGrad.reserve(nParticles);

            svd_w.reserve(nParticles);
            svd_e.reserve(nParticles);
            svd_v.reserve(nParticles);

            polar_r.reserve(nParticles);
            polar_s.reserve(nParticles);

            particleGridPos.reserve(nParticles);

            weightGradients.reserve(nParticles * 64);
            weights.reserve(nParticles * 64);
        }

        virtual void addParticles(const Vec_Vec3r& newPositions, const Vec_Vec3r& newVelocities)
        {
            __BNN_REQUIRE(newPositions.size() == newVelocities.size());
            reserve(static_cast<UInt>(newPositions.size()));

            for(size_t p = 0; p < newPositions.size(); ++p) {
                positions.push_back(newPositions[p]);
                velocities.push_back(newVelocities[p]);
                volumes.push_back(0);
                densities.push_back(0);
                velocityGradients.push_back(Mat3x3r(1.0));

                elasticDeformGrad.push_back(Mat3x3r(1.0));
                plasticDeformGrad.push_back(Mat3x3r(1.0));
                svd_w.push_back(Mat3x3r(1.0));
                svd_e.push_back(Vec3r(1.0));
                svd_v.push_back(Mat3x3r(1.0));
                polar_r.push_back(Mat3x3r(1.0));
                polar_s.push_back(Mat3x3r(1.0));

                particleGridPos.push_back(Vec3r(0));

                for(int i = 0; i < 64; ++i) {
                    weightGradients.push_back(Vec3r(0));
                    weights.push_back(0_f);
                }
            }
        }

        virtual UInt removeParticles(Vec_Int8& removeMarker)
        {
            if(!STLHelpers::contain(removeMarker, Int8(1))) {
                return 0u;
            }

            STLHelpers::eraseByMarker(positions,         removeMarker);
            STLHelpers::eraseByMarker(velocities,        removeMarker);
            STLHelpers::eraseByMarker(volumes,           removeMarker);               // need to erase, or just resize?
            STLHelpers::eraseByMarker(densities,         removeMarker);               // need to erase, or just resize?
            STLHelpers::eraseByMarker(velocityGradients, removeMarker);               // need to erase, or just resize?
            ////////////////////////////////////////////////////////////////////////////////
            elasticDeformGrad.resize(positions.size());
            plasticDeformGrad.resize(positions.size());

            svd_w.resize(positions.size());
            svd_e.resize(positions.size());
            svd_v.resize(positions.size());

            polar_r.resize(positions.size());
            polar_s.resize(positions.size());

            particleGridPos.resize(positions.size());

            weightGradients.resize(positions.size());
            weights.resize(positions.size());

            ////////////////////////////////////////////////////////////////////////////////
            // resize removeMarker eventually
            return static_cast<UInt>(removeMarker.size() - positions.size());
        }
    } particleData;


    ////////////////////////////////////////////////////////////////////////////////
    struct GridData : public GridSimulationData<3, Real>
    {
        Array3r         mass;
        Array3c         active;
        Array<3, Vec3r> velocity, velocity_new;

        // variable for implicit velocity solving
        Array3c         imp_active;
        Array<3, Vec3r> force,
                        err,          //error of estimate
                        r,            //residual of estimate
                        p,            //residual gradient? squared residual?
                        Ep, Er;       //yeah, I really don't know how this works...
        Array3r rDotEr;               //r.dot(Er)

        Array3SpinLock nodeLocks;
        Array3r        boundarySDF;

        ////////////////////////////////////////////////////////////////////////////////
        virtual void resize(const Vec3ui& gridSize)
        {
            mass.resize(gridSize);
            active.resize(gridSize);
            velocity.resize(gridSize);
            velocity_new.resize(gridSize);
            imp_active.resize(gridSize);
            force.resize(gridSize);
            err.resize(gridSize);
            r.resize(gridSize);
            p.resize(gridSize);
            Ep.resize(gridSize);
            Er.resize(gridSize);
            rDotEr.resize(gridSize);

            boundarySDF.resize(gridSize);
            nodeLocks.resize(gridSize);
        }

        ////////////////////////////////////////////////////////////////////////////////
        void resetGrid()
        {
            mass.assign(0_f);
            active.assign(char(0));
            imp_active.assign(char(0));
            velocity.assign(Vec3r(0));
            velocity_new.assign(Vec3r(0));
            force.assign(Vec3r(0));
            err.assign(Vec3r(0));
            r.assign(Vec3r(0));
            p.assign(Vec3r(0));
            Ep.assign(Vec3r(0));
            Er.assign(Vec3r(0));
            rDotEr.assign(0_f);
        }
    } gridData;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Snow3DSolver : public ParticleSolver3D, public RegisteredInFactory<Snow3DSolver>
{
public:
    Snow3DSolver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    static String                      solverName() { return String("MPM3DSolver"); }
    static SharedPtr<ParticleSolver3D> createSolver() { return std::make_shared<Snow3DSolver>(); }

    virtual String getSolverName() { return Snow3DSolver::solverName(); }
    virtual String getSolverDescription() override { return String("Simulation using MPM-3D Solver"); }

    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void sortParticles() override;

    ////////////////////////////////////////////////////////////////////////////////
    auto&       solverParams() { return m_SimParams; }
    const auto& solverParams() const { return m_SimParams; }
    auto&       solverData() { return m_SimData; }
    const auto& solverData() const { return m_SimData; }

protected:
    virtual void loadSimParams(const nlohmann::json& jParams) override;
    virtual void generateParticles(const nlohmann::json& jParams) override;
    virtual bool advanceScene(UInt frame, Real fraction = 0_f) override;
    virtual void allocateSolverMemory() override {}
    virtual void setupDataIO() override;
    virtual Int  loadMemoryState() override;
    virtual Int  saveMemoryState() override;
    virtual Int  saveFrameData() override;

    Real timestepCFL();
    void advanceVelocity(Real timestep);
    void updateParticles(Real timestep);

    ////////////////////////////////////////////////////////////////////////////////
    // grid processing
    void massToGrid();
    void velocityToGrid(Real timestep);
    void constrainGridVelocity(Real timestep);

    //Compute grid velocities
    void explicitVelocities(Real timestep);
    void implicitVelocities(Real timestep);
    void recomputeImplicitForces(Real timestep);

    //Map grid velocities back to particles
    void velocityToParticles(Real timestep);
    void constrainParticleVelocity(Real timestep);

    ////////////////////////////////////////////////////////////////////////////////
    // particle processing
    void calculateParticleVolumes();
    void updateParticlePositions(Real timestep);
    void updateGradients(Real timestep);
    void applyPlasticity();

    Mat3x3r computeEnergyDerivative(UInt p);
    Vec3r   computeDeltaForce(UInt p, const Vec3r& u, const Vec3r& weight_grad, Real timestep);      //Computes stress force delta, for implicit velocity update

    ////////////////////////////////////////////////////////////////////////////////
    auto&       particleData() { return solverData().particleData; }
    const auto& particleData() const { return solverData().particleData; }
    auto&       gridData() { return solverData().gridData; }
    const auto& gridData() const { return solverData().gridData; }

    SimulationParameters_Snow3D m_SimParams;
    SimulationData_Snow3D       m_SimData;

    Grid3r m_Grid;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana