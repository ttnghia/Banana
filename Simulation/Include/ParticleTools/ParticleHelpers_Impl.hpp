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

#include <Banana/TypeNames.h>
#include <Banana/Macros.h>
#include <Banana/FileHelpers.h>
#include <Banana/Data/DataIO.h>

#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
Vec2i createGrid(const Vec2<ScalarType>& bmin, const Vec2<ScalarType>& bmax, ScalarType spacing)
{
    Vec2<ScalarType> fgrid = (bmax - bmin) / spacing;

    return Vec2i(static_cast<int>(fgrid[0]), static_cast<int>(fgrid[1]));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
Vec3i createGrid(const Vec3<ScalarType>& bmin, const Vec3<ScalarType>& bmax, ScalarType spacing)
{
    Vec3<ScalarType> fgrid = (bmax - bmin) / spacing;

    return Vec3i(static_cast<int>(fgrid[0]), static_cast<int>(fgrid[1]), static_cast<int>(fgrid[2]));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
void transform(Vec_Vec3<ScalarType>& particles, const Vec3<ScalarType>& translation, const Vec3<ScalarType>& rotation)
{
    ScalarType azimuth = rotation[0];
    ScalarType elevation = rotation[1];
    ScalarType roll = rotation[2];
    ScalarType sinA, cosA, sinE, cosE, sinR, cosR;

    Vec3<ScalarType> R[3];

    sinA = (ScalarType)sin(azimuth);
    cosA = (ScalarType)cos(azimuth);
    sinE = (ScalarType)sin(elevation);
    cosE = (ScalarType)cos(elevation);
    sinR = (ScalarType)sin(roll);
    cosR = (ScalarType)cos(roll);

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
        ScalarType tmp[3];

        for(int j = 0; j < 3; ++j)
            tmp[j] = glm::dot(R[j], particles[i]);

        particles[i] = Vec3<ScalarType>(tmp[0] + translation[0],
                                        tmp[1] + translation[1],
                                        tmp[2] + translation[2]);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
UInt32 loadBinary(const std::string& fileName, Vec_Vec2<ScalarType>& particles, ScalarType& particleRadius)
{
    DataBuffer buffer;
    __BNN_ASSERT_MSG(FileHelpers::readFile(buffer.buffer(), fileName), "Could not open file for reading.");

    UInt32 numParticles;
    UInt64 segmentStart = 0;
    UInt64 segmentSize;

    segmentSize = sizeof(UInt32);
    memcpy(&numParticles, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = sizeof(ScalarType);
    memcpy(&particleRadius, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    ScalarType* particleData = reinterpret_cast<ScalarType*>(&buffer.data()[segmentStart]);
    __BNN_ASSERT(particleData != nullptr);
    segmentSize   = numParticles * sizeof(ScalarType) * 2;
    segmentStart += segmentSize;
    __BNN_ASSERT(segmentStart == buffer.size());

    ////////////////////////////////////////////////////////////////////////////////
    particles.resize(numParticles);

    for(UInt32 i = 0; i < numParticles; ++i)
    {
        particles[i] = Vec2<ScalarType>(particleData[i * 2],
                                        particleData[i * 2 + 1]);
    }

    return numParticles;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
UInt32 loadBinary(const std::string& fileName, Vec_Vec3<ScalarType>& particles, ScalarType& particleRadius)
{
    DataBuffer buffer;
    __BNN_ASSERT_MSG(FileHelpers::readFile(buffer.buffer(), fileName), "Could not open file for reading.");

    UInt32 numParticles;
    UInt64 segmentStart = 0;
    UInt64 segmentSize;


    segmentSize = sizeof(UInt32);
    memcpy(&numParticles, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = sizeof(ScalarType);
    memcpy(&particleRadius, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    ScalarType* particleData = reinterpret_cast<ScalarType*>(&buffer.data()[segmentStart]);
    __BNN_ASSERT(particleData != nullptr);
    segmentSize   = numParticles * sizeof(ScalarType) * 3;
    segmentStart += segmentSize;
    __BNN_ASSERT(segmentStart == buffer.size());

    ////////////////////////////////////////////////////////////////////////////////
    particles.resize(numParticles);

    for(UInt32 i = 0; i < numParticles; ++i)
    {
        particles[i] = Vec3<ScalarType>(particleData[i * 3],
                                        particleData[i * 3 + 1],
                                        particleData[i * 3 + 2]);
    }

    return numParticles;
}
