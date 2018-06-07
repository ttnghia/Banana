//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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

#pragma once

#include <Banana/Grid/Grid.h>
#include <ParticleSolvers/ParticleSolver.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleSolvers/MPM/Snow2DData.h>
#include <ParticleSolvers/ParticleSolverFactory.h>
#include <Banana/Array/Array.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationParameters_Snow2D : public SimulationParameters
{
	SimulationParameters_Snow2D() = default;

	////////////////////////////////////////////////////////////////////////////////
	Real CFLFactor = 0.04_f;
	Real PIC_FLIP_ratio = ParticleSolverDefaultParameters::PIC_FLIP_Ratio;
	Real minTimestep = ParticleSolverDefaultParameters::MinTimestep;
	Real maxTimestep = ParticleSolverDefaultParameters::MaxTimestep;
	Real boundaryReflectionMultiplier = ParticleSolverDefaultParameters::BoundaryReflectionMultiplier;

	Real CGRelativeTolerance = ParticleSolverDefaultParameters::CGRelativeTolerance;
	UInt maxCGIteration = ParticleSolverDefaultParameters::CGMaxIteration;

	Real thresholdCompression = 1.0_f - 1.9e-2_f; //Fracture threshold for compression (1-2.5e-2)
	Real thresholdStretching = 1.0_f + 7.5e-3_f; //Fracture threshold for stretching (1+7.5e-3)
	Real hardening = 5.0_f;            //How much plastic deformation strengthens material (10)
	Real materialDensity = 100.0_f;          //Density of snow in kg/m^2 (400 for 3d)
	Real YoungsModulus = 1.5e5_f;          //Young's modulus (springiness) (1.4e5)
	Real PoissonsRatio = 0.2_f;            //Poisson's ratio (transverse/axial strain ratio) (.2)
	Real implicitRatio = 0_f;              //Percentage that should be implicit vs explicit for velocity update

	Real maxImplicitError = 1e4_f;                //Maximum allowed error for conjugate residual
	Real minImplicitError = 1e-4_f;               //Minimum allowed error for conjugate residual

	Int kernelSpan = 2;

	Real  cellSize = ParticleSolverDefaultParameters::CellSize;
	Real  ratioCellSizeParticleRadius = 2.0_f;
	Vec2r movingBMin = Vec2r(-1.0);
	Vec2r movingBMax = Vec2r(1.0);

	// the following need to be computed
	Real  particleRadius;
	Real  particleMass;
	Vec2r domainBMin;
	Vec2r domainBMax;

	Real cellArea;

	Real lambda, mu;     //Lame parameters (_s denotes starting configuration)

	////////////////////////////////////////////////////////////////////////////////
	virtual void parseParameters(const JParams& jParams)
	{
		JSONHelpers::readValue(jParams, thresholdCompression, "ThresholdCompression");
		JSONHelpers::readValue(jParams, thresholdStretching, "ThresholdStretching");
		JSONHelpers::readValue(jParams, hardening, "Hardening");
		JSONHelpers::readValue(jParams, YoungsModulus, "YoungsModulus");
		JSONHelpers::readValue(jParams, PoissonsRatio, "PoissonsRatio");

		JSONHelpers::readValue(jParams, implicitRatio, "ImplicitRatio");
	}

	virtual void makeReady() override
	{
		particleRadius = cellSize / ratioCellSizeParticleRadius;
		particleMass = particleRadius * particleRadius * materialDensity;

		cellArea = cellSize * cellSize;
		domainBMin = movingBMin - Vec2r(cellSize * ParticleSolverDefaultParameters::NExpandCells);
		domainBMax = movingBMax + Vec2r(cellSize * ParticleSolverDefaultParameters::NExpandCells);

		lambda = YoungsModulus * PoissonsRatio / ((1.0_f + PoissonsRatio) * (1.0_f - 2.0_f * PoissonsRatio)),
			mu = YoungsModulus / (2.0_f + 2.0_f * PoissonsRatio);
	}

	////////////////////////////////////////////////////////////////////////////////
	virtual void printParams(const SharedPtr<Logger>& logger) override
	{
		logger->printLog("MPM-3D simulation parameters:");
		logger->printLogIndent("Maximum timestep: " + Formatters::toSciString(maxTimestep));
		logger->printLogIndent("CFL factor: " + std::to_string(CFLFactor));
		logger->printLogIndent("PIC/FLIP ratio: " + std::to_string(PIC_FLIP_ratio));

		logger->printLogIndent("Domain box: " + Formatters::toString(domainBMin) + String(" -> ") + Formatters::toString(domainBMax));
		logger->printLogIndent("Moving box: " + Formatters::toString(movingBMin) + String(" -> ") + Formatters::toString(movingBMax));
		logger->printLogIndent("Cell size: " + std::to_string(cellSize));
		Vec2ui numDomainCells(static_cast<UInt>(ceil((domainBMax[0] - domainBMin[0]) / cellSize)),
							  static_cast<UInt>(ceil((domainBMax[1] - domainBMin[1]) / cellSize)));
		Vec2ui numMovingCells(static_cast<UInt>(ceil((movingBMax[0] - movingBMin[0]) / cellSize)),
							  static_cast<UInt>(ceil((movingBMax[1] - movingBMin[1]) / cellSize)));
		logger->printLogIndent("Number of cells: " + std::to_string(numDomainCells[0] * numDomainCells[1]));
		logger->printLogIndent("Number of nodes: " + std::to_string((numDomainCells[0] + 1u) * (numDomainCells[1] + 1u)));
		logger->printLogIndent("Grid resolution: " + Formatters::toString(numDomainCells));
		logger->printLogIndent("Moving grid resolution: " + Formatters::toString(numMovingCells));

		logger->printLogIndent("Boundary restitution: " + std::to_string(boundaryReflectionMultiplier));
		logger->printLogIndent("ConjugateGradient solver tolerance: " + Formatters::toSciString(CGRelativeTolerance));
		logger->printLogIndent("Max CG iterations: " + Formatters::toSciString(maxCGIteration));

		logger->printLogIndent("Particle radius: " + std::to_string(particleRadius));
		logger->newLine();
	}
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationData_Snow2D
{
	struct ParticleData : ParticleSimulationData<2, Real>
	{
		//    Vec_Vec2r   positions_tmp;
		//    Vec_VecUInt neighborList;

		//    Vec_Mat2x2r energyDerivatives;
		Vec_Real    volumes, densities;
		Vec_Mat2x2r velocityGradients;

		//Deformation gradient (elastic and plastic parts)
		Vec_Mat2x2r elasticDeformGrad, plasticDeformGrad;

		//Cached SVD's for elastic deformation gradient
		Vec_Mat2x2r svd_w, svd_v;
		Vec_Vec2r   svd_e;

		//Cached polar decomposition
		Vec_Mat2x2r polar_r, polar_s;

		//Grid interpolation weights
		Vec_Vec2r particleGridPos;
		Vec_Vec2r weightGradients;                 // * 16
		Vec_Real  weights;                         // * 16

		////////////////////////////////////////////////////////////////////////////////

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

			weightGradients.reserve(nParticles * 16);
			weights.reserve(nParticles * 16);
		}

		virtual void addParticles(const Vec_Vec2r& newPositions, const Vec_Vec2r& newVelocities)
		{
			__BNN_REQUIRE(newPositions.size() == newVelocities.size());
			reserve(static_cast<UInt>(newPositions.size()));

			for(size_t p = 0; p < newPositions.size(); ++p)
			{
				positions.push_back(newPositions[p]);
				velocities.push_back(newVelocities[p]);
				volumes.push_back(0);
				densities.push_back(0);
				velocityGradients.push_back(Mat2x2r(1.0));

				elasticDeformGrad.push_back(Mat2x2r(1.0));
				plasticDeformGrad.push_back(Mat2x2r(1.0));
				svd_w.push_back(Mat2x2r(1.0));
				svd_e.push_back(Vec2r(1.0));
				svd_v.push_back(Mat2x2r(1.0));
				polar_r.push_back(Mat2x2r(1.0));
				polar_s.push_back(Mat2x2r(1.0));

				particleGridPos.push_back(Vec2r(0));

				for(int i = 0; i < 16; ++i)
				{
					weightGradients.push_back(Vec2r(0));
					weights.push_back(0_f);
				}
			}
		}

		virtual UInt removeParticles(Vec_Int8& removeMarker)
		{
			if(!STLHelpers::contain(removeMarker, Int8(1)))
			{
				return 0u;
			}

			STLHelpers::eraseByMarker(positions, removeMarker);
			STLHelpers::eraseByMarker(velocities, removeMarker);
			STLHelpers::eraseByMarker(volumes, removeMarker);     // need to erase, or just resize?
			STLHelpers::eraseByMarker(densities, removeMarker);     // need to erase, or just resize?
			STLHelpers::eraseByMarker(velocityGradients, removeMarker);     // need to erase, or just resize?
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
			return static_cast<UInt>(removeMarker.size() - positions.size());
		}
	} particleData;

	////////////////////////////////////////////////////////////////////////////////
	struct GridData : GridSimulationData<2, Real>
	{
		Array2r         mass;
		Array2c         active;
		Array<2, Vec2r> velocity, velocity_new;

		// variable for implicit velocity solving
		Array2c         imp_active;
		Array<2, Vec2r> force,
			err,          //error of estimate
			r,            //residual of estimate
			p,            //residual gradient? squared residual?
			Ep, Er;       //yeah, I really don't know how this works...
		Array2r rDotEr;               //r.dot(Er)

		//Array2SpinLock nodeLocks;
		Array2r        boundarySDF;

		////////////////////////////////////////////////////////////////////////////////
		virtual void resize(const Vec2<UInt>& gridSize)
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
			velocity.assign(Vec2r(0));
			velocity_new.assign(Vec2r(0));
			force.assign(Vec2r(0));
			err.assign(Vec2r(0));
			r.assign(Vec2r(0));
			p.assign(Vec2r(0));
			Ep.assign(Vec2r(0));
			Er.assign(Vec2r(0));
			rDotEr.assign(0_f);
		}
	} gridData;
};
