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
#define NOMINMAX

#include <Banana/Utils/Formatters.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/ParallelHelpers/Scheduler.h>
#include <Banana/NeighborSearch/NeighborSearch.h>
#include <Banana/Grid/Grid.h>
#include <catch.hpp>

#include <iostream>
#include <vector>
#include <array>
#include <limits>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <random>

using RealType = float;
using Clock    = std::chrono::high_resolution_clock;
using namespace Banana;

#define DIM 2
//#define TEST_BRUTE_FORCE
#define TEST_GRID

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vector<VecX<DIM, RealType>> positions;
Grid<DIM, RealType>         grid = Grid<DIM, RealType>(VecX<DIM, RealType>(-2), VecX<DIM, RealType>(2), RealType(1.0 / 128.0));

const size_t N               = 50;
const size_t N_enright_steps = 5;

const RealType r_omega  = 0.75_f;
const RealType r_omega2 = r_omega * r_omega;
const RealType radius   = 2.001_f * (2.0_f * r_omega / static_cast<RealType>(N - 1));
const RealType radius2  = radius * radius;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
RealType compute_average_number_of_neighbors(const NeighborSearch::NeighborSearch<DIM, RealType>& nsearch)
{
    UInt64      res = 0;
    const auto& d   = nsearch.point_set(0);

    for(UInt i = 0, iend = UInt(d.n_points()); i < iend; ++i) {
        res += static_cast<UInt64>(d.n_neighbors(0, i));
    }

    return static_cast<RealType>(res) / static_cast<RealType>(d.n_points());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
RealType compute_average_distance(const NeighborSearch::NeighborSearch<DIM, RealType>& nsearch)
{
    UInt64      res   = 0;
    UInt64      count = 0;
    auto const& d     = nsearch.point_set(0);

    for(UInt i = 0, iend = UInt(d.n_points()); i < iend; ++i) {
        UInt nn = UInt(d.n_neighbors(0, i));

        for(UInt j = 0; j < nn; ++j) {
            UInt k = d.neighbor(0, i, j);
            res += std::abs(Int(i) - static_cast<Int>(k));
            count++;
        }
    }
    return static_cast<RealType>(res) / static_cast<RealType>(count);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vector<Vector<UInt>> brute_force_search(size_t n_positions)
{
    Vector<Vector<UInt>> brute_force_neighbors(n_positions);
    Scheduler::parallel_for(n_positions,
                            [&](size_t i)
                            {
                                Vector<UInt>& neighbors       = brute_force_neighbors[i];
                                const VecX<DIM, RealType>& xa = positions[i];

                                for(UInt j = 0, jend = UInt(n_positions); j < jend; ++j) {
                                    if(i == size_t(j)) {
                                        continue;
                                    }

                                    const VecX<DIM, RealType>& xb = positions[j];
                                    RealType l2                   = glm::length2(xa - xb);
                                    if(l2 < radius * radius) {
                                        neighbors.push_back(j);
                                    }
                                }
                            });
    return brute_force_neighbors;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool compare_with_bruteforce_search(const NeighborSearch::NeighborSearch<DIM, RealType>& nsearch)
{
    const auto& d0                    = nsearch.point_set(0);
    auto        brute_force_neighbors = brute_force_search(d0.n_points());
    bool        success               = true;

    for(UInt i = 0, iend = UInt(d0.n_points()); i < iend; ++i) {
        auto const& bfn = brute_force_neighbors[i];

        if(bfn.size() != d0.n_neighbors(0, i)) {
            std::cerr << "*************************************ERROR: Not the same number of neighbors: "
                      << bfn.size() << " != " << d0.n_neighbors(0, i) << std::endl;

            Int diff = 0;
            for(auto x : d0.neighbors(0, i)) {
                diff = diff ^ Int(x);
            }
            for(auto x : bfn) {
                diff = diff ^ Int(x);
            }

            std::cerr << "Difference: " << diff << ", r2 = " << glm::length2(positions[i] - positions[diff]) / radius2
                      << ", r = " << glm::length(positions[i] - positions[diff]) / radius;
            std::cerr << std::endl << std::endl;
            success = false;
        }

        for(Int j = 0, jend = Int(d0.n_neighbors(0, i)); j < jend; ++j) {
            if(std::find(bfn.begin(), bfn.end(), d0.neighbor(0, i, j)) == bfn.end()) {
                std::cerr << "ERROR: Neighbor not found in brute force list." << std::endl;
                success = false;
            }
        }
    }
    if(success) {
        std::cout << "    ...brute force search and NSearch have the same result." << std::endl << std::endl;
    }
    return success;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool compare_with_grid_search(const NeighborSearch::NeighborSearch<DIM, RealType>& nsearch, Vector<Vector<UInt>>& gridSearchResult)
{
    const auto& d0      = nsearch.point_set(0);
    bool        success = true;
    for(UInt i = 0, iend = UInt(d0.n_points()); i < iend; ++i) {
        auto const& bfn = gridSearchResult[i];
        if(bfn.size() != d0.n_neighbors(0, i)) {
            std::cerr << "ERROR: Not the same number of neighbors." << std::endl;
            success = false;
        }

        for(UInt j = 0, jend = UInt(d0.n_neighbors(0, i)); j < jend; ++j) {
            if(std::find(bfn.begin(), bfn.end(), d0.neighbor(0, i, j)) == bfn.end()) {
                std::cerr << "ERROR: Neighbor not found in grid search list." << std::endl;
                success = false;
            }
        }
    }
    if(success) {
        std::cout << "    ...Grid search and NSearch have the same result." << std::endl << std::endl;
    }
    return success;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool compare_single_query_with_bruteforce_search(NeighborSearch::NeighborSearch<DIM, RealType>& nsearch)
{
    Vector<Vector<UInt>> neighbors;
    const auto&          d0                    = nsearch.point_set(0);
    auto                 brute_force_neighbors = brute_force_search(d0.n_points());
    bool                 success               = true;

    for(UInt i = 0, iend = UInt(d0.n_points()); i < iend; ++i) {
        const auto& bfn = brute_force_neighbors[i];
        neighbors.clear();
        nsearch.find_neighbors(0, i, neighbors);

        if(bfn.size() != neighbors[0].size()) {
            std::cerr << "ERROR: Not the same number of neighbors." << std::endl;
            success = false;
        }
        for(size_t j = 0; j < neighbors.size(); ++j) {
            if(std::find(bfn.begin(), bfn.end(), neighbors[0][j]) == bfn.end()) {
                std::cerr << "ERROR: Neighbor not found in brute force list." << std::endl;
                success = false;
            }
        }
    }
    if(success) {
        std::cout << "    ...Grid search and NSearch have the same result." << std::endl << std::endl;
    }
    return success;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
VecX<DIM, RealType> enright_velocity_field(VecX<DIM, RealType> const& x)
{
    RealType sin_pi_x_2 = RealType(std::sin(RealType(M_PI) * x[0]));
    RealType sin_pi_y_2 = RealType(std::sin(RealType(M_PI) * x[1]));
    sin_pi_x_2 *= sin_pi_x_2;
    sin_pi_y_2 *= sin_pi_y_2;

    RealType sin_2_pi_x = RealType(std::sin(2.0_f * M_PI * x[0]));
    RealType sin_2_pi_y = RealType(std::sin(2.0_f * M_PI * x[1]));

    if constexpr(DIM == 2) {
        VecX<DIM, RealType> tmp;
        tmp[0] = 2.0_f * sin_pi_x_2 * sin_2_pi_y;
        tmp[1] = -sin_2_pi_x * sin_pi_y_2;
        return tmp;
    } else {
        RealType sin_pi_z_2 = RealType(std::sin(RealType(M_PI) * x[2]));
        sin_pi_z_2 *= sin_pi_z_2;
        RealType sin_2_pi_z = RealType(std::sin(2.0_f * M_PI * x[2]));

        VecX<DIM, RealType> tmp;
        tmp[0] = 2.0_f * sin_pi_x_2 * sin_2_pi_y * sin_2_pi_z;
        tmp[1] = -sin_2_pi_x * sin_pi_y_2 * sin_2_pi_z;
        tmp[2] = -sin_2_pi_x * sin_2_pi_y * sin_pi_z_2;
        return tmp;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void advect()
{
    const RealType timestep = 0.01_f;
    Scheduler::parallel_for<size_t>(0, positions.size(), [&](size_t i)
                                    {
                                        auto& x       = positions[i];
                                        const auto& v = enright_velocity_field(x);
                                        x[0]         += timestep * v[0];
                                        x[1]         += timestep * v[1];
                                        if constexpr(DIM == 3) {
                                            x[2] += timestep * v[1];
                                        }
                                    });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test CompactNSearch", "[CompactNSearch]")
{
    RealType min_x = std::numeric_limits<RealType>::max();
    RealType max_x = std::numeric_limits<RealType>::min();
    positions.reserve(N * N * N);

    if constexpr(DIM == 2) {
        for(UInt i = 0; i < N; ++i) {
            for(UInt j = 0; j < N; ++j) {
                VecX<DIM, RealType> x;
                x[0] = r_omega * (2.0_f * (static_cast<RealType>(i) + MathHelpers::frand11<RealType>()) / static_cast<RealType>(N - 1) - 1.0_f);
                x[1] = r_omega * (2.0_f * (static_cast<RealType>(j) + MathHelpers::frand11<RealType>()) / static_cast<RealType>(N - 1) - 1.0_f);
                RealType l2 = glm::length2(x);

                if(l2 < r_omega2) {
                    x += VecX<DIM, RealType>(0.35_f);
                    positions.push_back(x);

                    if(min_x > x[0]) {
                        min_x = x[0];
                    }
                    if(max_x < x[0]) {
                        max_x = x[0];
                    }
                }
            }
        }
    } else {
        for(UInt i = 0; i < N; ++i) {
            for(UInt j = 0; j < N; ++j) {
                for(UInt k = 0; k < N; ++k) {
                    VecX<DIM, RealType> x;
                    x[0] = r_omega * (2.0_f * (static_cast<RealType>(i) + MathHelpers::frand11<RealType>()) / static_cast<RealType>(N - 1) - 1.0_f);
                    x[1] = r_omega * (2.0_f * (static_cast<RealType>(j) + MathHelpers::frand11<RealType>()) / static_cast<RealType>(N - 1) - 1.0_f);
                    x[2] = r_omega * (2.0_f * (static_cast<RealType>(k) + MathHelpers::frand11<RealType>()) / static_cast<RealType>(N - 1) - 1.0_f);
                    RealType l2 = glm::length2(x);
                    if(l2 < r_omega2) {
                        x += VecX<DIM, RealType>(0.35_f);
                        positions.push_back(x);

                        if(min_x > x[0]) {
                            min_x = x[0];
                        }
                        if(max_x < x[0]) {
                            max_x = x[0];
                        }
                    }
                }
            }
        }
    }

    // randomly shuffle the positions
    {
        auto rng = std::default_random_engine{};
        std::shuffle(positions.begin(), positions.end(), rng);
    }

    NeighborSearch::NeighborSearch<DIM, RealType> nsearch(radius, true);
    //NeighborSearch::NeighborSearch<DIM, RealType> nsearch(radius, false);
    nsearch.add_point_set(glm::value_ptr(positions.front()), UInt(positions.size()), true, true);
    //nsearch.add_point_set(glm::value_ptr(positions.front()), positions.size(), true, true);

    {
        auto t0 = Clock::now();
        nsearch.find_neighbors();
        auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
        std::cout << "Before z_sort: neighborhood search took " << Formatters::toString(runTime) << "ms" << std::endl << std::endl;
#ifdef TEST_BRUTE_FORCE
        REQUIRE(compare_with_bruteforce_search(nsearch));
#endif
    }

    //nsearch.update_point_sets();
    //Vector<Vector<UInt> > neighbors2;
    //nsearch.find_neighbors(0, 1, neighbors2);
    //Vector<Vector<UInt> > neighbors3;
    //nsearch.find_neighbors(1, 2, neighbors3);

    std::cout << "#Points                                = " << Formatters::toString(positions.size()) << std::endl;
    std::cout << "Search radius                          = " << radius << std::endl;
    std::cout << "Min x                                  = " << min_x << std::endl;
    std::cout << "Max x                                  = " << max_x << std::endl;
    std::cout << "Average number of neighbors            = " << compute_average_number_of_neighbors(nsearch) << std::endl;
    std::cout << "Average index distance prior to z-sort = " << Formatters::toString(compute_average_distance(nsearch)) << std::endl;

    {
        auto t0 = Clock::now();
        nsearch.z_sort();
        auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
        std::cout << "z_sort took " << Formatters::toString(runTime) << "ms" << std::endl << std::endl;
    }

    for(auto i = 0u; i < nsearch.n_point_sets(); ++i) {
        auto const& d = nsearch.point_set(i);
        d.sort_field(positions.data());
    }

    {
        auto t0 = Clock::now();
        nsearch.find_neighbors();
        auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
        std::cout << "After z_sort: neighborhood search took " << Formatters::toString(runTime) << "ms" << std::endl << std::endl;
#ifdef TEST_BRUTE_FORCE
        REQUIRE(compare_with_bruteforce_search(nsearch));
        //compare_single_query_with_bruteforce_search(nsearch);
#endif
    }

    //compare_with_bruteforce_search(nsearch);

    ////////////////////////////////////////////////////////////////////////////////
    // search using grid3d
#ifdef TEST_GRID
    Vector<Vector<UInt>> neighborsByCell(positions.size());

    {
        auto t0 = Clock::now();
        grid.collectIndexToCells(positions);
        Int cellSpan = Int(ceil(radius / grid.getCellSize()));
        grid.getNeighborList(positions, neighborsByCell, radius2, cellSpan);
        //getFinalNeighborList(neighborsByCell, neighborsByCellFinal);
        auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
        std::cout << "Using Grid to find neighborhood took " << Formatters::toString(runTime) << "ms" << std::endl;
        REQUIRE(compare_with_grid_search(nsearch, neighborsByCell));
    }
#endif

    std::cout << "Average index distance after z-sort    = " << Formatters::toString(compute_average_distance(nsearch)) << std::endl;

    std::cout << "Moving points:" << std::endl;
    for(size_t i = 0; i < N_enright_steps; ++i) {
        std::cout << std::endl << "Enright step " << i << ". ";
        advect();

        {
            for(auto& ppos : positions) {
                if(min_x > ppos[0]) {
                    min_x = ppos[0];
                }
                if(max_x < ppos[0]) {
                    max_x = ppos[0];
                }
            }
            std::cout << "Min x = " << min_x << ", Max x = " << max_x << std::endl;
        }

        {
            auto t0 = Clock::now();
            nsearch.find_neighbors();
            auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
            std::cout << "Neighborhood search took " << Formatters::toString(runTime) << "ms" << std::endl;
            std::cout << "Average index distance = " << Formatters::toString(compute_average_distance(nsearch)) << std::endl;
#ifdef TEST_BRUTE_FORCE
            REQUIRE(compare_with_bruteforce_search(nsearch));
            //compare_single_query_with_bruteforce_search(nsearch);
#endif
        }

#ifdef TEST_GRID
        {
            auto t0 = Clock::now();
            grid.collectIndexToCells(positions);
            Int cellSpan = Int(ceil(radius / grid.getCellSize()));
            grid.getNeighborList(positions, neighborsByCell, radius2, cellSpan);
            auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
            std::cout << "Using Grid to find neighborhood took " << Formatters::toString(runTime) << "ms" << std::endl;
            REQUIRE(compare_with_grid_search(nsearch, neighborsByCell));
        }
#endif

        nsearch.z_sort();
        for(auto i = 0u; i < nsearch.n_point_sets(); ++i) {
            auto const& d  = nsearch.point_set(i);
            auto        t0 = Clock::now();
            d.sort_field(positions.data());
            auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
            std::cout << "Sort field took " << Formatters::toString(runTime) << "ms" << std::endl;
        }

        {
            auto t0 = Clock::now();
            nsearch.find_neighbors();
            auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
            std::cout << "Neighborhood search after z_sort took " << Formatters::toString(runTime) << "ms" << std::endl;
            std::cout << "Average index distance = " << Formatters::toString(compute_average_distance(nsearch)) << std::endl;
        }

#ifdef TEST_GRID
        {
            auto t0 = Clock::now();
            grid.collectIndexToCells(positions);
            Int cellSpan = Int(ceil(radius / grid.getCellSize()));
            grid.getNeighborList(positions, neighborsByCell, radius2, cellSpan);
            auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
            std::cout << "Using Grid to find neighborhood after z_sort took " << Formatters::toString(runTime) << "ms" << std::endl;
        }
#endif
    }
}
