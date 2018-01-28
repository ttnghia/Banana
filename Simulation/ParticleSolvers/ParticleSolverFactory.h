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
#include <ParticleSolvers/HybridFluid/AFLIP_3DSolver.h>
#include <ParticleSolvers/HybridFluid/APIC_3DSolver.h>
#include <ParticleSolvers/HybridFluid/FLIP_3DSolver.h>
#include <ParticleSolvers/HybridFluid/PIC_3DSolver.h>

#include <ParticleSolvers/SPH/WCSPH_3DSolver.h>
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// mpm solvers
#include <ParticleSolvers/MPM/MPM_2DSolver.h>
#include <ParticleSolvers/MPM/MPM_3DSolver.h>
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
        if(solverName == "AFLIP_3DSolver") {
            __BNN_REQUIRE(N == 3);
            return dynamic_pointer_cast<ParticleSolver<N, RealType> >(std::make_shared<AFLIP_3DSolver>());
        } else if(solverName == "APIC_3DSolver") {
            __BNN_REQUIRE(N == 3);
            return dynamic_pointer_cast<ParticleSolver<N, RealType> >(std::make_shared<APIC_3DSolver>());
        } else if(solverName == "FLIP_3DSolver") {
            __BNN_REQUIRE(N == 3);
            return dynamic_pointer_cast<ParticleSolver<N, RealType> >(std::make_shared<FLIP_3DSolver>());
        } else if(solverName == "PIC_3DSolver") {
            __BNN_REQUIRE(N == 3);
            return dynamic_pointer_cast<ParticleSolver<N, RealType> >(std::make_shared<PIC_3DSolver>());
        } else if(solverName == "WCSPH_3DSolver") {
            __BNN_REQUIRE(N == 3);
            return dynamic_pointer_cast<ParticleSolver<N, RealType> >(std::make_shared<WCSPH_3DSolver>());
        }
        ////////////////////////////////////////////////////////////////////////////////
        // MPM solver
        else if(solverName == "MPM_2DSolver") {
            __BNN_REQUIRE(N == 2);
            return dynamic_pointer_cast<ParticleSolver<N, RealType> >(std::make_shared<MPM_2DSolver>());
        } else if(solverName == "MPM_3DSolver") {
            __BNN_REQUIRE(N == 3);
            return dynamic_pointer_cast<ParticleSolver<N, RealType> >(std::make_shared<MPM_3DSolver>());
        }

        return nullptr;
    }
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleSolverFactory