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
template<class RealType>
Vec2i Banana::ParticleHelpers::createGrid<RealType>(const Vec2<RealType>& bmin, const Vec2<RealType>& bmax, RealType spacing)
{
    Vec2<RealType> fgrid = (bmax - bmin) / spacing;
    return Vec2i(static_cast<int>(round(fgrid[0])), static_cast<int>(round(fgrid[1])));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
Vec3i Banana::ParticleHelpers::createGrid<RealType>(const Vec3<RealType>& bmin, const Vec3<RealType>& bmax, RealType spacing)
{
    Vec3<RealType> fgrid = (bmax - bmin) / spacing;
    return Vec3i(static_cast<int>(round(fgrid[0])), static_cast<int>(round(fgrid[1])), static_cast<int>(round(fgrid[2])));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::ParticleHelpers::transform<RealType>(Vec_Vec3<RealType>& particles, const Vec3<RealType>& translation, const Vec3<RealType>& rotation)
{
    RealType azimuth = rotation[0];
    RealType elevation = rotation[1];
    RealType roll = rotation[2];
    RealType sinA, cosA, sinE, cosE, sinR, cosR;

    Vec3<RealType> R[3];

    sinA = (RealType)sin(azimuth);
    cosA = (RealType)cos(azimuth);
    sinE = (RealType)sin(elevation);
    cosE = (RealType)cos(elevation);
    sinR = (RealType)sin(roll);
    cosR = (RealType)cos(roll);

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
        RealType tmp[3];

        for(int j = 0; j < 3; ++j)
            tmp[j] = glm::dot(R[j], particles[i]);

        particles[i] = Vec3<RealType>(tmp[0] + translation[0],
                                      tmp[1] + translation[1],
                                      tmp[2] + translation[2]);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
UInt32 Banana::ParticleHelpers::loadBinary<RealType>(const std::string& fileName, Vec_Vec2<RealType>& particles, RealType& particleRadius)
{
    DataBuffer buffer;
    __BNN_ASSERT_MSG(FileHelpers::readFile(buffer.buffer(), fileName), "Could not open file for reading.");

    UInt32 numParticles;
    UInt64 segmentStart = 0;
    UInt64 segmentSize;

    segmentSize = sizeof(UInt32);
    memcpy(&numParticles, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = sizeof(RealType);
    memcpy(&particleRadius, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    RealType* particleData = reinterpret_cast<RealType*>(&buffer.data()[segmentStart]);
    __BNN_ASSERT(particleData != nullptr);
    segmentSize   = numParticles * sizeof(RealType) * 2;
    segmentStart += segmentSize;
    __BNN_ASSERT(segmentStart == buffer.size());

    ////////////////////////////////////////////////////////////////////////////////
    particles.reserve(particles.size() + static_cast<size_t>(numParticles));

    for(UInt32 i = 0; i < numParticles; ++i)
    {
        particles.emplace_back(Vec2<RealType>(particleData[i * 2],
                                              particleData[i * 2 + 1]));
    }

    return numParticles;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
UInt32 Banana::ParticleHelpers::loadBinary<RealType>(const std::string& fileName, Vec_Vec3<RealType>& particles, RealType& particleRadius)
{
    DataBuffer buffer;
    __BNN_ASSERT_MSG(FileHelpers::readFile(buffer.buffer(), fileName), "Could not open file for reading.");

    UInt32 numParticles;
    UInt64 segmentStart = 0;
    UInt64 segmentSize;

    segmentSize = sizeof(UInt32);
    memcpy(&numParticles, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = sizeof(RealType);
    memcpy(&particleRadius, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    RealType* particleData = reinterpret_cast<RealType*>(&buffer.data()[segmentStart]);
    __BNN_ASSERT(particleData != nullptr);
    segmentSize   = numParticles * sizeof(RealType) * 3;
    segmentStart += segmentSize;
    __BNN_ASSERT(segmentStart == buffer.size());

    ////////////////////////////////////////////////////////////////////////////////
    particles.reserve(particles.size() + static_cast<size_t>(numParticles));

    for(UInt32 i = 0; i < numParticles; ++i)
    {
        particles.emplace_back(Vec3<RealType>(particleData[i * 3],
                                              particleData[i * 3 + 1],
                                              particleData[i * 3 + 2]));
    }

    return numParticles;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType, class VectorType>
void ParticleHelpers::saveBinary<RealType, VectorType>(const std::string& fileName, std::vector<VectorType>& particles, RealType& particleRadius)
{
    static_assert(sizeof(RealType) * 2 == sizeof(VectorType) || sizeof(RealType) * 3 == sizeof(VectorType), "Inconsistent type of particle radius and vector data!");

    DataBuffer buffer;
    UInt32     numParticles = static_cast<UInt32>(particles.size());

    buffer.push_back(numParticles);
    buffer.push_back(particleRadius);
    buffer.pushFloatArray(particles, false);

    FileHelpers::writeBinaryFile(fileName, buffer.buffer());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::ParticleHelpers::jitter<RealType>(Vec3<RealType>& ppos, RealType maxJitter)
{
    ppos += maxJitter * Vec3<RealType>(MathHelpers::frand11<RealType>(),
                                       MathHelpers::frand11<RealType>(),
                                       MathHelpers::frand11<RealType>())
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::ParticleHelpers::clamp<RealType>(Vec3<RealType>& ppos, const Vec3<RealType>& bmin, const Vec3<RealType>& bmax, RealType margin /*= 0*/)
{
    for(int i = 0; i < 3; ++i)
    {
        if(ppos[i] < bmin[i] + margin ||
           ppos[i] > bmax[i] - margin)
            ppos[i] = MathHelpers::min(MathHelpers::max(ppos[i], bmin[i] + margin),
                                       bmax[i] - margin);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::ParticleHelpers::relaxPosition<RealType>(const Vec_Vec3<RealType>& particles, RelaxationMethod method)
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
