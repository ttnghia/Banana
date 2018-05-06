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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// implementation of base classes
#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleSolvers/ParticleSolverData.hpp>
#include <ParticleSolvers/ParticleSolver.h>
#include <ParticleSolvers/ParticleSolver.hpp>
template class Banana::ParticleSolvers::ParticleSolver<2, Real>;
template class Banana::ParticleSolvers::ParticleSolver<3, Real>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// SPH
#include <ParticleSolvers/SPH/WCSPH_Solver.h>
#include <ParticleSolvers/SPH/SPH_Data.hpp>
#include <ParticleSolvers/SPH/WCSPH_Solver.hpp>
template class Banana::ParticleSolvers::WCSPH_Solver<2, Real>;
template class Banana::ParticleSolvers::WCSPH_Solver<3, Real>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Hybrid fluid solver
#include <ParticleSolvers/PICFluid/PIC_Solver.h>
#include <ParticleSolvers/PICFluid/PIC_Data.hpp>
#include <ParticleSolvers/PICFluid/PIC_Solver.hpp>
template class Banana::ParticleSolvers::PIC_Solver<2, Real>;
template class Banana::ParticleSolvers::PIC_Solver<3, Real>;;

////////////////////////////////////////////////////////////////////////////////
#include <ParticleSolvers/PICFluid/FLIP_Solver.h>
#include <ParticleSolvers/PICFluid/FLIP_Solver.hpp>
template class Banana::ParticleSolvers::FLIP_Solver<2, Real>;
template class Banana::ParticleSolvers::FLIP_Solver<3, Real>;

////////////////////////////////////////////////////////////////////////////////
#include <ParticleSolvers/PICFluid/APIC_Solver.h>
#include <ParticleSolvers/PICFluid/APIC_Solver.hpp>
template class Banana::ParticleSolvers::APIC_Solver<2, Real>;
template class Banana::ParticleSolvers::APIC_Solver<3, Real>;

////////////////////////////////////////////////////////////////////////////////
#include <ParticleSolvers/PICFluid/AFLIP_Solver.h>
#include <ParticleSolvers/PICFluid/AFLIP_Solver.hpp>
template class Banana::ParticleSolvers::AFLIP_Solver<2, Real>;
template class Banana::ParticleSolvers::AFLIP_Solver<3, Real>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// hyperelastic MPM
#include <ParticleSolvers/MPM/MPM_Solver.h>
#include <ParticleSolvers/MPM/MPM_Data.hpp>
#include <ParticleSolvers/MPM/MPM_Solver.hpp>
template class Banana::ParticleSolvers::MPM_Solver<2, Real>;
template class Banana::ParticleSolvers::MPM_Solver<3, Real>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// hair mpm
#include <ParticleSolvers/MPM/HairMPM_Solver.h>
#include <ParticleSolvers/MPM/HairMPM_Data.hpp>
#include <ParticleSolvers/MPM/HairMPM_Solver.hpp>
template class Banana::ParticleSolvers::HairMPM_Solver<2, Real>;
template class Banana::ParticleSolvers::HairMPM_Solver<3, Real>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// mass-spring system
#include <ParticleSolvers/MassSpringSystems/MSS_Solver.h>
#include <ParticleSolvers/MassSpringSystems/MSS_Data.hpp>
#include <ParticleSolvers/MassSpringSystems/MSS_Solver.hpp>
template class Banana::ParticleSolvers::MSS_Solver<2, Real>;
template class Banana::ParticleSolvers::MSS_Solver<3, Real>;

////////////////////////////////////////////////////////////////////////////////
#include <ParticleSolvers/MassSpringSystems/Peridynamics_Solver.h>
#include <ParticleSolvers/MassSpringSystems/Peridynamics_Data.hpp>
#include <ParticleSolvers/MassSpringSystems/Peridynamics_Solver.hpp>
template class Banana::ParticleSolvers::Peridynamics_Solver<2, Real>;
template class Banana::ParticleSolvers::Peridynamics_Solver<3, Real>;

////////////////////////////////////////////////////////////////////////////////
//#include <ParticleSolvers/MassSpringSystems/Cloth_Solver.h>
//#include <ParticleSolvers/MassSpringSystems/Cloth_Data.hpp>
//#include <ParticleSolvers/MassSpringSystems/Cloth_Solver.hpp>
//template class Banana::ParticleSolvers::Cloth_Solver<2, Real>;
//template class Banana::ParticleSolvers::Cloth_Solver<3, Real>;