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
#include <Banana/Grid/Grid.h>
#include <Banana/Array/Array.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Utils/Timer.h>
#include <Banana/ParallelHelpers/Scheduler.h>
#include <Banana/ParallelHelpers/ParallelObjects.h>

using namespace Banana;

#define RESOLUTION 128

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec_Vec3r positions;
Grid3r    grid;

Array3r                           arrayData;
Array3<ParallelObjects::SpinLock> arrayLock;

Real sdfRadius;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void testSequentialScattering()
{
    arrayData.assign(sdfRadius * Real(3.0));
    for(size_t p = 0; p < positions.size(); ++p) {
        const auto& ppos     = positions[p];
        const auto  cellIdx  = grid.getCellIdx<Int>(ppos);
        const auto  cellDown = MathHelpers::max(Vec3i(0), cellIdx - Vec3i(1));
        const auto  cellUp   = MathHelpers::min(cellIdx + Vec3i(1), Vec3i(grid.getNCells()) - Vec3i(1));

        for(Int k = cellDown[2]; k <= cellUp[2]; ++k) {
            for(Int j = cellDown[1]; j <= cellUp[1]; ++j) {
                for(Int i = cellDown[0]; i <= cellUp[0]; ++i) {
                    const auto sample = grid.getWorldCoordinate(Vec3r(i + 0.5, j + 0.5, k + 0.5));
                    const Real phiVal = glm::length(sample - positions[p]) - sdfRadius;

                    if(phiVal < arrayData(i, j, k)) {
                        arrayData(i, j, k) = phiVal;
                    }
                }
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void testParallelCellGathering()
{
    arrayData.assign(sdfRadius * Real(3.0));
    for(size_t p = 0; p < positions.size(); ++p) {
        const auto& ppos     = positions[p];
        const auto  cellIdx  = grid.getCellIdx<Int>(ppos);
        const auto  cellDown = MathHelpers::max(Vec3i(0), cellIdx - Vec3i(1));
        const auto  cellUp   = MathHelpers::min(cellIdx + Vec3i(1), Vec3i(grid.getNCells()) - Vec3i(1));

        Scheduler::parallel_for(cellDown[0], cellUp[0],
                                cellDown[1], cellUp[1],
                                cellDown[2], cellUp[2],
                                [&](int i, int j, int k)
                                {
                                    const auto sample = grid.getWorldCoordinate(Vec3r(i + 0.5, j + 0.5, k + 0.5));
                                    const Real phiVal = glm::length(sample - positions[p]) - sdfRadius;

                                    if(phiVal < arrayData(i, j, k)) {
                                        arrayData(i, j, k) = phiVal;
                                    }
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void testParallelParticleScattering()
{
    arrayData.assign(sdfRadius * Real(3.0));
    Scheduler::parallel_for(positions.size(),
                            [&](size_t p)
                            {
                                const auto& ppos    = positions[p];
                                const auto cellIdx  = grid.getCellIdx<Int>(ppos);
                                const auto cellDown = MathHelpers::max(Vec3i(0), cellIdx - Vec3i(1));
                                const auto cellUp   = MathHelpers::min(cellIdx + Vec3i(1), Vec3i(grid.getNCells()) - Vec3i(1));

                                for(Int k = cellDown[2]; k <= cellUp[2]; ++k) {
                                    for(Int j = cellDown[1]; j <= cellUp[1]; ++j) {
                                        for(Int i = cellDown[0]; i <= cellUp[0]; ++i) {
                                            const auto sample = grid.getWorldCoordinate(Vec3r(i + 0.5, j + 0.5, k + 0.5));
                                            const auto phiVal = glm::length(sample - ppos) - sdfRadius;

                                            arrayLock(i, j, k).lock();
                                            if(phiVal < arrayData(i, j, k)) {
                                                arrayData(i, j, k) = phiVal;
                                            }
                                            arrayLock(i, j, k).unlock();
                                        }
                                    }
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void setupData()
{
    auto cellSize = Real(1.0) / Real(RESOLUTION);
    auto pradius  = cellSize / Real(4.0);
    auto pGrid    = NumberHelpers::createGrid<Int>(Vec3r(0), Vec3r(1.0), pradius * Real(2.0));
    sdfRadius = cellSize * Real(1.01 * sqrt(3) / 2.0);

    grid.setGrid(Vec3r(0), Vec3r(1.0), cellSize);
    arrayData.resize(grid.getNCells());
    arrayLock.resize(grid.getNCells());
    ////////////////////////////////////////////////////////////////////////////////
    printf("Grid size: %s\n",           __BNN_TO_CSTRING(grid.getNCells()));
    printf("Number of particles: %s\n", NumberHelpers::formatWithCommas(glm::compMul(pGrid)).c_str());
    ////////////////////////////////////////////////////////////////////////////////
    positions.reserve(glm::compMul(pGrid));
    for(Int i = 0; i < RESOLUTION; ++i) {
        for(Int j = 0; j < RESOLUTION; ++j) {
            for(Int k = 0; k < RESOLUTION; ++k) {
                auto ppos = Vec3r(i, j, k) * Real(2.0 * pradius);
                NumberHelpers::jitter(ppos, pradius * Real(0.1));
                positions.push_back(ppos);
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int main()
{
    setupData();
    ////////////////////////////////////////////////////////////////////////////////
    double t0;
    {
        __BNN_PERORMANCE_TEST_BEGIN;
        testSequentialScattering();
        __BNN_PERORMANCE_TEST_END_WITH_RUN_TIME("sequential cell scattering", t0);
    }
    ////////////////////////////////////////////////////////////////////////////////
    double t1;
    {
        __BNN_PERORMANCE_TEST_BEGIN;
        testParallelCellGathering();
        __BNN_PERORMANCE_TEST_END_WITH_RUN_TIME("parallel cell gathering", t1);
    }
    ////////////////////////////////////////////////////////////////////////////////
    double t2;
    {
        __BNN_PERORMANCE_TEST_BEGIN;
        testParallelParticleScattering();
        __BNN_PERORMANCE_TEST_END_WITH_RUN_TIME("parallel particle scattering", t2);
    }
    ////////////////////////////////////////////////////////////////////////////////
    printf("\nSpeedup parallel cell gathering: %f\n",                   t0 / t1);
    printf("Speedup parallel particle scattering: %f\n",                t0 / t2);
    printf("Ratio parallel cell gathering/particle scattering: %f\n\n", t1 / t2);
    return 0;
}
