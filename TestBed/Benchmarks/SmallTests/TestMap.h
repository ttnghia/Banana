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

#include <Banana/Utils/Timer.h>

#include <catch.hpp>

#include <iostream>
#include <unordered_map>
#include <map>
#include <chrono>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <ctime>

using namespace Banana;

#define NUM_DATA 300
#define NUM_TEST 100'000'000
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test Simple Map Data", "[TestMap]")
{
    std::map<int, double>           M1;
    std::unordered_map<int, double> M2;
    Timer                           timer;
    double                          s = 0;
    srand(time(NULL));

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    for(int i = 0; i < NUM_DATA; ++i)
    {
        M1[i] = double(i);
        M2[i] = double(i);
    }
    timer.tock();
    std::cout << "Generate data: " << timer.getRunTime() << std::endl;

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();

    for(int i = 0; i < NUM_TEST; ++i)
    {
        int x = int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX)));
        if(M1.find(x) != M1.end())
            s += M1[x];
    }
    timer.tock();
    std::cout << "Normal map: " << timer.getRunTime() << std::endl;


    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    for(int i = 0; i < NUM_TEST; ++i)
    {
        int x = int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX)));
        if(M2.find(x) != M2.end())
            s += M2[x];
    }
    timer.tock();
    std::cout << "Unordered map: " << timer.getRunTime() << std::endl;
    std::cout << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct pair_pair_hash
{
#if 0
    template<class T>
    std::size_t operator ()(const std::pair<T, std::pair<T, T> >& p) const
    {
        const int h  = (sizeof(size_t) - sizeof(T)) * 4;
        const int hh = (sizeof(size_t) - sizeof(T)) * 4 / 2;
        return (p.first << h) ^ (p.second.first << hh) ^ p.second.second;
    }

#else
    template<class T>
    std::size_t operator ()(const std::pair<T, std::pair<T, T> >& p) const
    {
        size_t seed = 0;
        hash_combine(seed, p.first);
        hash_combine(seed, p.second.first);
        hash_combine(seed, p.second.second);
        return seed;
    }
#endif
};

struct tuple_hash
{
#if 0
    template<class T>
    std::size_t operator ()(const std::tuple<T, T, T>& t) const
    {
        const int h  = (sizeof(size_t) - sizeof(T)) * 4;
        const int hh = (sizeof(size_t) - sizeof(T)) * 4 / 2;
        return (std::get<0>(t) << h) ^ (std::get<1>(t) << hh) ^ std::get<2>(t);
    }

#else
    template<class T>
    std::size_t operator ()(const std::tuple<T, T, T>& t) const
    {
        size_t seed = 0;
        hash_combine(seed, std::get<0>(t));
        hash_combine(seed, std::get<1>(t));
        hash_combine(seed, std::get<2>(t));
        return seed;
    }
#endif
};

TEST_CASE("Test More Complex Map Data", "[TestMap]")
{
    std::map<std::pair<int, std::pair<int, int> >, double>                           M1;
    std::unordered_map<std::pair<int, std::pair<int, int> >, double, pair_pair_hash> M2;
    std::map<std::tuple<int, int, int>, double>                                      M3;
    std::unordered_map<std::tuple<int, int, int>, double, tuple_hash>                M4;
    Timer                                                                            timer;
    double                                                                           s = 0;
    srand(time(NULL));

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    for(int i = 0; i < NUM_DATA; ++i)
        for(int j = 0; j < NUM_DATA; ++j)
            for(int k = 0; k < NUM_DATA; ++k)
            {
                M1[std::make_pair(i, std::make_pair(j, k))] = double(i + j + k);
            }
    timer.tock();
    std::cout << "Generate data for normal map with pair of pair key: " << timer.getRunTime() << std::endl;

    timer.tick();
    for(int i = 0; i < NUM_TEST; ++i)
    {
        auto x = std::make_pair(int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX))),
                                std::make_pair(int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX))),
                                               int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX)))));
        if(M1.find(x) != M1.end())
            s += M1[x];
    }
    timer.tock();
    M1.clear();
    std::cout << "Normal map with pair of pair key: " << timer.getRunTime() << std::endl << std::endl;

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    for(int i = 0; i < NUM_DATA; ++i)
        for(int j = 0; j < NUM_DATA; ++j)
            for(int k = 0; k < NUM_DATA; ++k)
            {
                M2[std::make_pair(i, std::make_pair(j, k))] = double(i + j + k);
            }
    timer.tock();
    std::cout << "Generate data for unordered map with pair of pair key: " << timer.getRunTime() << std::endl;

    timer.tick();
    for(int i = 0; i < NUM_TEST; ++i)
    {
        auto x = std::make_pair(int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX))),
                                std::make_pair(int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX))),
                                               int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX)))));
        if(M2.find(x) != M2.end())
            s += M2[x];
    }
    timer.tock();
    M2.clear();
    std::cout << "Unordered map with pair of pair key: " << timer.getRunTime() << std::endl << std::endl;

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    for(int i = 0; i < NUM_DATA; ++i)
        for(int j = 0; j < NUM_DATA; ++j)
            for(int k = 0; k < NUM_DATA; ++k)
            {
                M3[std::make_tuple(i, j, k)] = double(i + j + k);
            }
    timer.tock();
    std::cout << "Generate data for normal map with tuple key: " << timer.getRunTime() << std::endl;

    timer.tick();
    for(int i = 0; i < NUM_TEST; ++i)
    {
        auto x = std::make_tuple(int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX))),
                                 int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX))),
                                 int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX))));
        if(M3.find(x) != M3.end())
            s += M3[x];
    }
    timer.tock();
    M3.clear();
    std::cout << "Normal map with tuple key: " << timer.getRunTime() << std::endl << std::endl;

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    for(int i = 0; i < NUM_DATA; ++i)
        for(int j = 0; j < NUM_DATA; ++j)
            for(int k = 0; k < NUM_DATA; ++k)
            {
                M4[std::make_tuple(i, j, k)] = double(i + j + k);
            }
    timer.tock();
    std::cout << "Generate data for unordered map with tuple key: " << timer.getRunTime() << std::endl;

    timer.tick();
    for(int i = 0; i < NUM_TEST; ++i)
    {
        auto x = std::make_tuple(int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX))),
                                 int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX))),
                                 int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX))));
        if(M4.find(x) != M4.end())
            s += M4[x];
    }
    timer.tock();
    M4.clear();
    std::cout << "Unordered map with tuple key: " << timer.getRunTime() << std::endl << std::endl;
    std::cout << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test More Complex Map Data Again", "[TestMap]")
{
    std::map<int, std::map<int, std::map<int, double> > >                               M1;
    std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, double> > > M2;
    Timer                                                                               timer;
    double                                                                              s = 0;
    srand(time(NULL));

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    for(int i = 0; i < NUM_DATA; ++i)
        for(int j = 0; j < NUM_DATA; ++j)
            for(int k = 0; k < NUM_DATA; ++k)
            {
                M1[i][j][k] = double(i + j + k);
            }
    timer.tock();
    std::cout << "Generate data for map of map of map: " << timer.getRunTime() << std::endl;

    timer.tick();
    for(int i = 0; i < NUM_TEST; ++i)
    {
        int x = int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX)));
        int y = int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX)));
        int z = int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX)));

        if(M1.find(x) != M1.end() && M1[x].find(y) != M1[x].end() && M1[x][y].find(z) != M1[x][y].end())
            s += M1[x][y][z];
    }
    timer.tock();
    M1.clear();
    std::cout << "Normal map: " << timer.getRunTime() << std::endl << std::endl;

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    for(int i = 0; i < NUM_DATA; ++i)
        for(int j = 0; j < NUM_DATA; ++j)
            for(int k = 0; k < NUM_DATA; ++k)
            {
                M2[i][j][k] = double(i + j + k);
            }
    timer.tock();
    std::cout << "Generate data for unordered_map of unordered_map of unordered_map: " << timer.getRunTime() << std::endl;

    timer.tick();
    for(int i = 0; i < NUM_TEST; ++i)
    {
        int x = int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX)));
        int y = int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX)));
        int z = int(double(NUM_DATA) * (double(rand()) / double(RAND_MAX)));

        if(M2.find(x) != M2.end() && M2[x].find(y) != M2[x].end() && M2[x][y].find(z) != M2[x][y].end())
            s += M2[x][y][z];
    }
    timer.tock();
    M2.clear();
    std::cout << "Unordered map: " << timer.getRunTime() << std::endl;
    std::cout << std::endl;
}
