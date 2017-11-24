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

////////////////////////////////////////////////////////////////////////////////
// fluid solvers
#include <ParticleSolvers/HybridFluid/AFLIP3D_Solver.h>
#include <ParticleSolvers/HybridFluid/APIC3D_Solver.h>
#include <ParticleSolvers/HybridFluid/FLIP3D_Solver.h>
#include <ParticleSolvers/HybridFluid/PIC3D_Solver.h>
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// mpm solvers
#include <ParticleSolvers/MPM/MPM2D_Solver.h>
#include <ParticleSolvers/MPM/MPM3D_Solver.h>
////////////////////////////////////////////////////////////////////////////////


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class ParticleSolverFactory
{
public:
    static SharedPtr<ParticleSolver<N, RealType> > createSolver(const String& solverName)
    {
        ////////////////////////////////////////////////////////////////////////////////
        // fluid solvers
        if(solverName == "AFLIP3D_Solver") {
            __BNN_REQUIRE(N == 3);
            return dynamic_pointer_cast<ParticleSolver<N, RealType> >(std::make_shared<AFLIP3D_Solver>());
        } else if(solverName == "APIC3D_Solver") {
            __BNN_REQUIRE(N == 3);
            return dynamic_pointer_cast<ParticleSolver<N, RealType> >(std::make_shared<APIC3D_Solver>());
        } else if(solverName == "FLIP3D_Solver") {
            __BNN_REQUIRE(N == 3);
            return dynamic_pointer_cast<ParticleSolver<N, RealType> >(std::make_shared<FLIP3D_Solver>());
        } else if(solverName == "PIC3D_Solver") {
            __BNN_REQUIRE(N == 3);
            return dynamic_pointer_cast<ParticleSolver<N, RealType> >(std::make_shared<PIC3D_Solver>());
        }
        ////////////////////////////////////////////////////////////////////////////////
        // MPM solver
        else if(solverName == "MPM2D_Solver") {
            __BNN_REQUIRE(N == 2);
            return dynamic_pointer_cast<ParticleSolver<N, RealType> >(std::make_shared<MPM2D_Solver>());
        } else if(solverName == "MPM3D_Solver") {
            __BNN_REQUIRE(N == 3);
            return dynamic_pointer_cast<ParticleSolver<N, RealType> >(std::make_shared<MPM3D_Solver>());
        }
    }
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleSolverFactory