//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once
#define NOMINMAX

#include <Banana/Utils/NumberHelpers.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>
#include <Banana/NeighborSearch/NeighborSearch.h>
#include <Banana/Grid/Grid3DHashing.h>
#include <catch.hpp>

#include <iostream>
#include <vector>
#include <array>
#include <limits>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <random>

using Clock = std::chrono::high_resolution_clock;
using namespace Banana;

//#define TEST_GRID3D
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vector<Vec3r> positions;
Grid3DHashing grid3D = Grid3DHashing(Vec3r(-2), Vec3r(2), Real(1.0 / 128.0));

const size_t N               = 50;
const size_t N_enright_steps = 50;

const Real r_omega  = Real(0.75);
const Real r_omega2 = r_omega * r_omega;
const Real radius   = Real(2.0) * (Real(2.0) * r_omega / static_cast<Real>(N - 1));
const Real radius2  = radius * radius;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real compute_average_number_of_neighbors(NeighborSearch const& nsearch)
{
    unsigned long res = 0;
    const auto&   d   = nsearch.point_set(0);

    for(int i = 0; i < d.n_points(); ++i)
        res += static_cast<unsigned long>(d.n_neighbors(0, i));

    return static_cast<Real>(res) / static_cast<Real>(d.n_points());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real compute_average_distance(NeighborSearch const& nsearch)
{
    unsigned long long res   = 0;
    auto const&        d     = nsearch.point_set(0);
    unsigned long long count = 0;

    for(int i = 0; i < d.n_points(); ++i)
    {
        size_t nn = d.n_neighbors(0, i);

        for(int j = 0; j < nn; ++j)
        {
            UInt k = d.neighbor(0, i, j);
            res += std::abs(i - static_cast<int>(k));
            count++;
        }
    }
    return static_cast<Real>(res) / static_cast<Real>(count);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vector<Vector<UInt> > brute_force_search(size_t n_positions)
{
    Vector<Vector<UInt> > brute_force_neighbors(n_positions);

    ParallelFuncs::parallel_for<size_t>(0, n_positions,
                                        [&](size_t i)
                                        {
                                            Vector<UInt>& neighbors = brute_force_neighbors[i];
                                            Vec3r const& xa = positions[i];

                                            for(int j = 0; j < n_positions; ++j)
                                            {
                                                if(i == size_t(j))
                                                    continue;

                                                Vec3r const& xb = positions[j];

                                                Real l2 = glm::length2(xa - xb);
                                                if(l2 <= radius * radius)
                                                    neighbors.push_back(j);
                                            }
                                        });

    return std::move(brute_force_neighbors);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void compare_with_bruteforce_search(NeighborSearch const& nsearch)
{
    const PointSet& d0                    = nsearch.point_set(0);
    auto            brute_force_neighbors = brute_force_search(d0.n_points());

    for(int i = 0; i < d0.n_points(); ++i)
    {
        auto const& bfn = brute_force_neighbors[i];

        if(bfn.size() != d0.n_neighbors(0, i))
        {
            std::cerr << "ERROR: Not the same number of neighbors: " << bfn.size() << " != " << d0.n_neighbors(0, i) << std::endl;
            std::cerr << "NSearch: ";
            for(auto x : d0.neighbors(0, i))
                std::cerr << x << ", ";
            std::cerr << std::endl;

            std::cerr << "Brute force: ";
            for(auto x : bfn)
                std::cerr << x << ", ";
            std::cerr << std::endl << std::endl;
        }

        for(int j = 0; j < d0.n_neighbors(0, i); ++j)
        {
            if(std::find(bfn.begin(), bfn.end(), d0.neighbor(0, i, j)) == bfn.end())
            {
                std::cerr << "ERROR: Neighbor not found in brute force list." << std::endl;
            }
        }
    }

    std::cout << "    ...Grid search and NSearch have the same result." << std::endl << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void compare_with_grid_search(NeighborSearch const& nsearch, Vector<Vector<UInt> >& gridSearchResult)
{
    const PointSet& d0 = nsearch.point_set(0);

    for(int i = 0; i < d0.n_points(); ++i)
    {
        auto const& bfn = gridSearchResult[i];

        if(bfn.size() != d0.n_neighbors(0, i))
        {
            std::cerr << "ERROR: Not the same number of neighbors." << std::endl;
        }

        for(int j = 0; j < d0.n_neighbors(0, i); ++j)
        {
            if(std::find(bfn.begin(), bfn.end(), d0.neighbor(0, i, j)) == bfn.end())
            {
                std::cerr << "ERROR: Neighbor not found in grid search list." << std::endl;
            }
        }
    }

    std::cout << "    ...Grid search and NSearch have the same result." << std::endl << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void compare_single_query_with_bruteforce_search(NeighborSearch& nsearch)
{
    Vector<Vector<UInt> > neighbors;
    const PointSet&       d0                    = nsearch.point_set(0);
    auto                  brute_force_neighbors = brute_force_search(d0.n_points());

    for(int i = 0; i < d0.n_points(); ++i)
    {
        const auto& bfn = brute_force_neighbors[i];
        neighbors.clear();
        nsearch.find_neighbors(0, i, neighbors);

        if(bfn.size() != neighbors[0].size())
        {
            std::cerr << "ERROR: Not the same number of neighbors." << std::endl;
        }
        for(int j = 0; j < neighbors.size(); ++j)
        {
            if(std::find(bfn.begin(), bfn.end(), neighbors[0][j]) == bfn.end())
            {
                std::cerr << "ERROR: Neighbor not found in brute force list." << std::endl;
            }
        }
    }

    std::cout << "    ...Grid search and NSearch have the same result." << std::endl << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3r enright_velocity_field(Vec3r const& x)
{
    Real sin_pi_x_2 = std::sin(Real(M_PI) * x[0]);
    Real sin_pi_y_2 = std::sin(Real(M_PI) * x[1]);
    Real sin_pi_z_2 = std::sin(Real(M_PI) * x[2]);
    sin_pi_x_2 *= sin_pi_x_2;
    sin_pi_y_2 *= sin_pi_y_2;
    sin_pi_z_2 *= sin_pi_z_2;

    Real sin_2_pi_x = static_cast<Real>(std::sin(Real(2.0 * M_PI) * x[0]));
    Real sin_2_pi_y = static_cast<Real>(std::sin(Real(2.0 * M_PI) * x[1]));
    Real sin_2_pi_z = static_cast<Real>(std::sin(Real(2.0 * M_PI) * x[2]));

    return Vec3r(static_cast<Real>(2.0) * sin_pi_x_2 * sin_2_pi_y * sin_2_pi_z,
                 -sin_2_pi_x * sin_pi_y_2 * sin_2_pi_z,
                 -sin_2_pi_x * sin_2_pi_y * sin_pi_z_2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void advect()
{
    const Real timestep = Real(0.01);
    ParallelFuncs::parallel_for<size_t>(0, positions.size(), [&](size_t i)
                                        {
                                            Vec3r& x = positions[i];
                                            Vec3r v = enright_velocity_field(x);
                                            x[0] += timestep * v[0];
                                            x[1] += timestep * v[1];
                                            x[2] += timestep * v[1];
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test CompactNSearch", "[CompactNSearch]")
{
    Real min_x = std::numeric_limits<Real>::max();
    Real max_x = std::numeric_limits<Real>::min();
    positions.reserve(N * N * N);

    for(UInt i = 0; i < N; ++i)
    {
        for(UInt j = 0; j < N; ++j)
        {
            for(UInt k = 0; k < N; ++k)
            {
                Vec3r x = Vec3r(r_omega * (2.0 * static_cast<Real>(i) / static_cast<Real>(N - 1) - 1.0),
                                r_omega * (2.0 * static_cast<Real>(j) / static_cast<Real>(N - 1) - 1.0),
                                r_omega * (2.0 * static_cast<Real>(k) / static_cast<Real>(N - 1) - 1.0));

                Real l2 = x[0] * x[0] + x[1] * x[1] + x[2] * x[2];

                if(l2 < r_omega2)
                {
                    x[0] += Real(0.35);
                    x[1] += Real(0.35);
                    x[2] += Real(0.35);
                    positions.push_back(x);

                    if(min_x > x[0])
                        min_x = x[0];

                    if(max_x < x[0])
                        max_x = x[0];
                }
            }
        }
    }
    //std::random_shuffle(positions.begin(), positions.end());

    NeighborSearch nsearch(radius, true);
    nsearch.add_point_set(glm::value_ptr(positions.front()), positions.size(), true, true);
    //nsearch.add_point_set(glm::value_ptr(positions.front()), positions.size(), true, true);

    {
        auto t0 = Clock::now();
        nsearch.find_neighbors();
        auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
        std::cout << "Before z_sort: neighborhood search took " << NumberHelpers::formatWithCommas(runTime) << "ms" << std::endl << std::endl;
        compare_with_bruteforce_search(nsearch);
    }

    //nsearch.update_point_sets();
    //Vector<Vector<UInt> > neighbors2;
    //nsearch.find_neighbors(0, 1, neighbors2);
    //Vector<Vector<UInt> > neighbors3;
    //nsearch.find_neighbors(1, 2, neighbors3);

    std::cout << "#Points                                = " << NumberHelpers::formatWithCommas(positions.size()) << std::endl;
    std::cout << "Search radius                          = " << radius << std::endl;
    std::cout << "Min x                                  = " << min_x << std::endl;
    std::cout << "Max x                                  = " << max_x << std::endl;
    std::cout << "Average number of neighbors            = " << compute_average_number_of_neighbors(nsearch) << std::endl;
    std::cout << "Average index distance prior to z-sort = " << NumberHelpers::formatWithCommas(compute_average_distance(nsearch)) << std::endl;

    {
        auto t0 = Clock::now();
        nsearch.z_sort();
        auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
        std::cout << "z_sort took " << NumberHelpers::formatWithCommas(runTime) << "ms" << std::endl << std::endl;
    }

    for(auto i = 0u; i < nsearch.n_point_sets(); ++i)
    {
        auto const& d = nsearch.point_set(i);
        d.sort_field(positions.data());
    }

    {
        auto t0 = Clock::now();
        nsearch.find_neighbors();
        auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
        std::cout << "After z_sort: neighborhood search took " << NumberHelpers::formatWithCommas(runTime) << "ms" << std::endl << std::endl;
        compare_with_bruteforce_search(nsearch);
        //compare_single_query_with_bruteforce_search(nsearch);
    }

    //compare_with_bruteforce_search(nsearch);




    ////////////////////////////////////////////////////////////////////////////////
    // search using grid3d
#ifdef TEST_GRID3D
    Vector<Vector<UInt> > neighborsByCell(positions.size());

    {
        auto t0 = Clock::now();
        grid3D.collectIndexToCells(positions);
        int cellSpan = int(ceil(radius / grid3D.getCellSize()));
        grid3D.getNeighborList(positions, neighborsByCell, radius2, cellSpan);
        //getFinalNeighborList(neighborsByCell, neighborsByCellFinal);
        auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
        std::cout << "Using Grid3D to find neighborhood took " << NumberHelpers::formatWithCommas(runTime) << "ms" << std::endl;

        compare_with_grid_search(nsearch, neighborsByCell);
    }
#endif

    std::cout << "Average index distance after z-sort    = " << NumberHelpers::formatWithCommas(compute_average_distance(nsearch)) << std::endl;

    std::cout << "Moving points:" << std::endl;
    for(int i = 0; i < N_enright_steps; ++i)
    {
        std::cout << std::endl << "Enright step " << i << ". ";
        advect();

        {
            for(auto& ppos : positions)
            {
                if(min_x > ppos[0])
                    min_x = ppos[0];
                if(max_x < ppos[0])
                    max_x = ppos[0];
            }
            std::cout << "Min x = " << min_x << ", Max x = " << max_x << std::endl;
        }

        {
            auto t0 = Clock::now();
            nsearch.find_neighbors();
            auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
            std::cout << "Neighborhood search took " << NumberHelpers::formatWithCommas(runTime) << "ms" << std::endl;
            std::cout << "Average index distance = " << NumberHelpers::formatWithCommas(compute_average_distance(nsearch)) << std::endl;
            compare_with_bruteforce_search(nsearch);
            //compare_single_query_with_bruteforce_search(nsearch);
        }

#ifdef TEST_GRID3D
        {
            auto t0 = Clock::now();
            grid3D.collectIndexToCells(positions);
            int cellSpan = int(ceil(radius / grid3D.getCellSize()));
            grid3D.getNeighborList(positions, neighborsByCell, radius2, cellSpan);
            auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
            std::cout << "Using Grid3D to find neighborhood took " << NumberHelpers::formatWithCommas(runTime) << "ms" << std::endl;

            compare_with_grid_search(nsearch, neighborsByCell);
        }
#endif

        nsearch.z_sort();
        for(auto i = 0u; i < nsearch.n_point_sets(); ++i)
        {
            auto const& d = nsearch.point_set(i);
            d.sort_field(positions.data());
        }

        {
            auto t0 = Clock::now();
            nsearch.find_neighbors();
            auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
            std::cout << "Neighborhood search after z_sort took " << NumberHelpers::formatWithCommas(runTime) << "ms" << std::endl;
            std::cout << "Average index distance = " << NumberHelpers::formatWithCommas(compute_average_distance(nsearch)) << std::endl;
        }

#ifdef TEST_GRID3D
        {
            auto t0 = Clock::now();
            grid3D.collectIndexToCells(positions);
            int cellSpan = int(ceil(radius / grid3D.getCellSize()));
            grid3D.getNeighborList(positions, neighborsByCell, radius2, cellSpan);
            auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
            std::cout << "Using Grid3D to find neighborhood after z_sort took " << NumberHelpers::formatWithCommas(runTime) << "ms" << std::endl;
        }
#endif
    }
}
