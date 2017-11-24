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

#include <ParticleTools/ParticleHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleHelpers::transform(Vec_Vec3r& particles, const Vec3r& translation, const Vec3r& rotation)
{
    Real azimuth = rotation[0];
    Real elevation = rotation[1];
    Real roll = rotation[2];
    Real sinA, cosA, sinE, cosE, sinR, cosR;

    Vec3r R[3];

    sinA = (Real)sin(azimuth);
    cosA = (Real)cos(azimuth);
    sinE = (Real)sin(elevation);
    cosE = (Real)cos(elevation);
    sinR = (Real)sin(roll);
    cosR = (Real)cos(roll);

    R[0][0] = cosR * cosA - sinR * sinA * sinE;
    R[0][1] = sinR * cosA + cosR * sinA * sinE;
    R[0][2] = -sinA * cosE;

    R[1][0] = -sinR * cosE;
    R[1][1] = cosR * cosE;
    R[1][2] = sinE;

    R[2][0] = cosR * sinA + sinR * cosA * sinE;
    R[2][1] = sinR * sinA - cosR * cosA * sinE;
    R[2][2] = cosA * cosE;

    for(size_t i = 0, iend = particles.size(); i != iend; ++i) {
        Real tmp[3];

        for(int j = 0; j < 3; ++j) {
            tmp[j] = glm::dot(R[j], particles[i]);
        }

        particles[i] = Vec3r(tmp[0] + translation[0],
                             tmp[1] + translation[1],
                             tmp[2] + translation[2]);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleHelpers::connectedComponentAnalysis(const Vec_VecUInt& connectionList, Vec_Int8& componentIdx, UInt& nComponents)
{
    componentIdx.assign(connectionList.size(), Int8(-1));

    // label from first particle
    UInt nProcessed = 0;
    nProcessed += spawnComponent(0, 0, 0, connectionList, componentIdx);

    nComponents = 1;
    Int8 currentCompIdx = 0;

    bool new_label = false;

    while(nProcessed < connectionList.size()) {
        bool bLabeled = false;

        for(UInt p = 0; p < connectionList.size(); ++p) {
            // Firstly, find any particle that has not been label
            // and that particle has a labeled neighbor
            if(componentIdx[p] >= 0) {
                continue;
            }

            if(new_label) {
                nProcessed += spawnComponent(p, 0, currentCompIdx, connectionList, componentIdx);

                bLabeled  = true;
                new_label = false;
            } else {
                // find the component index from neighbor
                Int8 neighborCompIdx = -1;

                for(UInt q : connectionList[p]) {
                    if(componentIdx[q] >= 0) {
                        neighborCompIdx = componentIdx[q];
                        break;
                    }
                }

                // has a labeled neighbor?
                // get component id from neighbor
                if(neighborCompIdx >= 0) {
                    nProcessed += spawnComponent(p, 0, neighborCompIdx, connectionList, componentIdx);
                    bLabeled    = true;
                }
            }
        }

        // not any particle has been labeled in the last loop
        // while num_process still < num particles
        // that means, we arrive at a new component
        if(!bLabeled) {
            ++currentCompIdx;
            ++nComponents;

            new_label = true;
        }
    }

    __BNN_REQUIRE(nProcessed == connectionList.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
UInt ParticleHelpers::spawnComponent(UInt p, Int depth, UInt8 currentIdx, const Vec_VecUInt& connectionList, Vec_Int8& componentIdx)
{
    componentIdx[p] = currentIdx;
    UInt nProcessed = 1;

    // max depth = 1024 to avoid stack overflow due to many time recursively call this function
    if(depth < 1024) {
        for(UInt q : connectionList[p]) {
            if(componentIdx[q] < 0) {
                nProcessed += spawnComponent(q, depth + 1, currentIdx, connectionList, componentIdx);
            }
        }
    }

    return nProcessed;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
