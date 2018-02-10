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

#include <Banana/Setup.h>
#include <ParticleSolvers/ParticleSolver.h>
#include <map>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class ParticleSolverFactory
{
public:
    ParticleSolverFactory() = delete;
    using PtrCreateMethod   = SharedPtr<ParticleSolver<N, RealType> >(*)();
    ////////////////////////////////////////////////////////////////////////////////
    static bool                                    registerSolver(const String& solverName, PtrCreateMethod funcCreate);
    static SharedPtr<ParticleSolver<N, RealType> > createSolver(const String& solverName);

private:
    static std::map<String, PtrCreateMethod>  s_CreateMethods;
    static std::map<String, PtrCreateMethod>* s_CreateMethods2;
};

template<Int N, class RealType>
std::map<String, typename ParticleSolverFactory<N, RealType>::PtrCreateMethod> ParticleSolverFactory<N, RealType>::s_CreateMethods;

template<Int N, class RealType>
std::map<String, typename ParticleSolverFactory<N, RealType>::PtrCreateMethod>* ParticleSolverFactory<N, RealType>::s_CreateMethods2 = new std::map<String, typename ParticleSolverFactory<N, RealType>::PtrCreateMethod>;

using ParticleSolverFactory2D = ParticleSolverFactory<2, Real>;
using ParticleSolverFactory3D = ParticleSolverFactory<3, Real>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool ParticleSolverFactory<N, RealType >::registerSolver(const String& solverName, PtrCreateMethod funcCreate)
{
    auto y = s_CreateMethods2;
    auto x = s_CreateMethods.size();
    s_CreateMethods["abc"] = nullptr;


    auto it = s_CreateMethods.find(solverName);
    if(it == s_CreateMethods.end()) {
        s_CreateMethods[solverName] = funcCreate;
        return true;
    }
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
SharedPtr<ParticleSolver<N, RealType> > ParticleSolverFactory<N, RealType >::createSolver(const String& solverName)
{
    if(auto it = s_CreateMethods.find(name); it != s_CreateMethods.end()) {
        return it->second();     // call the createFunc
    }
    return nullptr;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Solver>
class RegisteredInFactory
{
protected:
    static bool s_bRegistered;
};

template<class Solver>
bool RegisteredInFactory<Solver>::s_bRegistered = ParticleSolverFactory<Solver::solverDimension(), Solver::SolverRealType>::registerSolver(Solver::solverName(),
                                                                                                                                           Solver::createSolver);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}; // end namespace Banana::ParticleSolvers