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

#include <Banana/Setup.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Utils/Timer.h>
#include <Banana/ParallelHelpers/Scheduler.h>

#include <map>

using namespace Banana;
#define NUM_TESTS     10

#define NUM_PARTICLES (10'000'000)
#define NUM_OBJECTS   4
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec_Vec3r           positions;
Vec_Vec3r           forces;
Vec_Real            masses;
Vec_Int8            objectIdx;
std::map<int, Real> objectMasses;

Real particleRadius;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void testVector()
{
    Scheduler::parallel_for(NUM_PARTICLES,
                            [&](int p)
                            {
                                positions[p] = forces[p] / masses[p] * Real(1e-5);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void testMap()
{
    Scheduler::parallel_for(NUM_PARTICLES,
                            [&](int p)
                            {
                                positions[p] = forces[p] / objectMasses[objectIdx[p]] * Real(1e-5);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void setupData()
{
    printf("Initializing...");
    srand(time(NULL));
    positions.resize(NUM_PARTICLES);
    forces.resize(NUM_PARTICLES);
    masses.resize(NUM_PARTICLES);
    objectIdx.resize(NUM_PARTICLES);
    ////////////////////////////////////////////////////////////////////////////////
    for(int i = 0; i < NUM_PARTICLES; ++i) {
        positions[i] = MathHelpers::vrand11<Vec3f>();
        forces[i]    = MathHelpers::vrand11<Vec3f>();
        masses[i]    = MathHelpers::frand11<Real>();
        objectIdx[i] = static_cast<Int8>(rand() % NUM_OBJECTS);
    }

    for(int i = 0; i < NUM_OBJECTS; ++i) {
        objectMasses[i] = MathHelpers::frand11<Real>();
    }
    printf("finished.\n\n");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int main()
{
    for(int i = 0; i < NUM_TESTS; ++i) {
        setupData();
        ////////////////////////////////////////////////////////////////////////////////
        {
            __BNN_PERORMANCE_TEST_BEGIN;
            testVector();
            __BNN_PERORMANCE_TEST_END("vector");
        }

        {
            __BNN_PERORMANCE_TEST_BEGIN;
            testMap();
            __BNN_PERORMANCE_TEST_END("map");
        }
        printf("\n\n");
    }
    ////////////////////////////////////////////////////////////////////////////////
    return 0;
}
