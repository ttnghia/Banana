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

#include <ParticleTools/ParticleHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleHelpers::relaxPosition(Vec_Vec3r& particles, RelaxationMethod method)
{
    if(method == LloydRelaxationMethod)
    {
        //x;
        //LloydRelaxation lloyd(domainParams, particle_radius);
        //lloyd.moving_percentage_threshold = samplingParams->movingPercentageThreshold;
        //lloyd.overlap_ratio_threshold = samplingParams->overlapRatioThreshold;
        //lloyd.remove_ratio_threshold = samplingParams->overlapRatioThreshold;

        //lloyd.relax_particles(samples, particles, 10, 3000);
    }
    else
    {
        //x;
    }
}

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

    for(size_t i = 0, iend = particles.size(); i != iend; ++i)
    {
        Real tmp[3];

        for(int j = 0; j < 3; ++j)
            tmp[j] = glm::dot(R[j], particles[i]);

        particles[i] = Vec3r(tmp[0] + translation[0],
                             tmp[1] + translation[1],
                             tmp[2] + translation[2]);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
