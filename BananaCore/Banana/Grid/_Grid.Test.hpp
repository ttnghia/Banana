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

#include <Banana/Grid/Grid.h>

#include <catch.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test Grid3D", "[Grid3D]")
{
    Banana::Grid<3, Real> grid1;
    (void)grid1;

    ////////////////////////////////////////////////////////////////////////////////
    Banana::Grid<3, Real> grid(Vec3r(0), Vec3r(1), 0.1f);
    REQUIRE(grid.getNCells()[0] == 10);
    REQUIRE(grid.getNCells()[1] == 10);
    REQUIRE(grid.getNCells()[2] == 10);

    REQUIRE(grid.isValidCell(Vec3i(1, 2, 3)));
    REQUIRE(!grid.isValidCell(Vec3i(1, 2, -1)));

    REQUIRE(grid.getNearestValidCellIdx(Vec3i(1, 2, -1)) == Vec3i(1, 2, 0));
    REQUIRE(grid.getValidCellIdx<int>(Vec3f(0.15, 0.25, -0.5)) == Vec3i(1, 2, 0));

    //auto x = grid.getCellLinearizedIndex(1, 2);
    //////////////////////////////////////////////////////////////////////////////////
    Vec_Vec3r particles;
    Vec_UInt  neighborList;
    Vec3f     p1 = Vec3f(0.15, 0.25, 0.5);
    Vec3f     p2 = Vec3f(0.25, 0.25, 0.5);
    particles.push_back(p1);
    particles.push_back(p2);
    grid.collectIndexToCells(particles);
    grid.getNeighborList(p1, neighborList);
    REQUIRE(neighborList.size() == 2);
}
