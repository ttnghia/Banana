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

#include <Banana/TypeNames.h>

#include <random>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class BoxBoundarySampler
{
public:
    static void sampleParticles(const Vec3<RealType>& bMin, const Vec3<RealType>& bMax, Vec_Vec3<RealType>& bdParticles,
                                RealType spacing, int numBDLayers = 2)
    {
        bdParticles.resize(0);
        std::random_device rd;
        std::mt19937       gen(rd());

        std::normal_distribution<RealType> disSmall(0, RealType(0.02) * spacing);
        std::normal_distribution<RealType> disLarge(0, RealType(0.1) * spacing);

        Vec3<RealType> boundaryBMin = bMin - Vec3<RealType>(spacing * (RealType(numBDLayers) - RealType(0.5)));
        Vec3<RealType> boundaryBMax = bMax + Vec3<RealType>(spacing * (RealType(numBDLayers) - RealType(0.501)));

        ////////////////////////////////////////////////////////////////////////////////
        for(RealType x = boundaryBMin[0]; x <= boundaryBMax[0]; x += spacing)
        {
            if(x > bMin[0] && x < bMax[0])
                x = bMax[0] + spacing * RealType(0.5);

            for(RealType y = boundaryBMin[1]; y <= boundaryBMax[1]; y += spacing)
            {
                if(y > bMin[1] && y < bMax[1])
                    y = bMax[1] + spacing * RealType(0.5);

                for(RealType z = boundaryBMin[2]; z <= boundaryBMax[2]; z += spacing)
                {
                    if(z > bMin[2] && z < bMax[2])
                        z = bMax[2] + spacing * RealType(0.5);

                    const Vec3<RealType> gridPos(x, y, z);
                    Vec3<RealType>       ppos = gridPos + Vec3<RealType>(disLarge(gen), disLarge(gen), disLarge(gen));

                    if(gridPos[0] < bMin[0] || gridPos[0] > bMax[0])
                        ppos[0] = gridPos[0] + disSmall(gen);

                    if(gridPos[1] < bMin[1] || gridPos[1] > bMax[1])
                        ppos[1] = gridPos[1] + disSmall(gen);

                    if(gridPos[2] < bMin[2] || gridPos[2] > bMax[2])
                        ppos[2] = gridPos[2] + disSmall(gen);

                    bdParticles.push_back(ppos);
                }
            }
        }
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana