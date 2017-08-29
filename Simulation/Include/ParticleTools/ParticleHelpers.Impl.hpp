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
template<class Real>
Vec2i Banana::ParticleHelpers::createGrid<Real>(const Vec2<Real>& bmin, const Vec2<Real>& bmax, Real spacing)
{
    Vec2<Real> fgrid = (bmax - bmin) / spacing;
    return Vec2i(static_cast<int>(round(fgrid[0])), static_cast<int>(round(fgrid[1])));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
Vec3i Banana::ParticleHelpers::createGrid<Real>(const Vec3<Real>& bmin, const Vec3<Real>& bmax, Real spacing)
{
    Vec3<Real> fgrid = (bmax - bmin) / spacing;
    return Vec3i(static_cast<int>(round(fgrid[0])), static_cast<int>(round(fgrid[1])), static_cast<int>(round(fgrid[2])));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
void Banana::ParticleHelpers::transform<Real>(Vec_Vec3<Real>& particles, const Vec3<Real>& translation, const Vec3<Real>& rotation)
{
    Real azimuth = rotation[0];
    Real elevation = rotation[1];
    Real roll = rotation[2];
    Real sinA, cosA, sinE, cosE, sinR, cosR;

    Vec3<Real> R[3];

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

        particles[i] = Vec3<Real>(tmp[0] + translation[0],
                                  tmp[1] + translation[1],
                                  tmp[2] + translation[2]);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
UInt Banana::ParticleHelpers::loadBinary<Real>(const std::string& fileName, Vec_Vec2<Real>& particles, Real& particleRadius)
{
    DataBuffer buffer;
    __BNN_ASSERT_MSG(FileHelpers::readFile(buffer.buffer(), fileName), "Could not open file for reading.");

    UInt numParticles;
    UInt64 segmentStart = 0;
    UInt64 segmentSize;

    segmentSize = sizeof(UInt);
    memcpy(&numParticles, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = sizeof(Real);
    memcpy(&particleRadius, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    Real* particleData = reinterpret_cast<Real*>(&buffer.data()[segmentStart]);
    __BNN_ASSERT(particleData != nullptr);
    segmentSize   = numParticles * sizeof(Real) * 2;
    segmentStart += segmentSize;
    __BNN_ASSERT(segmentStart == buffer.size());

    ////////////////////////////////////////////////////////////////////////////////
    particles.reserve(particles.size() + static_cast<size_t>(numParticles));

    for(UInt i = 0; i < numParticles; ++i)
    {
        particles.emplace_back(Vec2<Real>(particleData[i * 2],
                                          particleData[i * 2 + 1]));
    }

    return numParticles;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
UInt Banana::ParticleHelpers::loadBinary<Real>(const std::string& fileName, Vec_Vec3<Real>& particles, Real& particleRadius)
{
    DataBuffer buffer;
    __BNN_ASSERT_MSG(FileHelpers::readFile(buffer.buffer(), fileName), "Could not open file for reading.");

    UInt numParticles;
    UInt64 segmentStart = 0;
    UInt64 segmentSize;

    segmentSize = sizeof(UInt);
    memcpy(&numParticles, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = sizeof(Real);
    memcpy(&particleRadius, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    Real* particleData = reinterpret_cast<Real*>(&buffer.data()[segmentStart]);
    __BNN_ASSERT(particleData != nullptr);
    segmentSize   = numParticles * sizeof(Real) * 3;
    segmentStart += segmentSize;
    __BNN_ASSERT(segmentStart == buffer.size());

    ////////////////////////////////////////////////////////////////////////////////
    particles.reserve(particles.size() + static_cast<size_t>(numParticles));

    for(UInt i = 0; i < numParticles; ++i)
    {
        particles.emplace_back(Vec3<Real>(particleData[i * 3],
                                          particleData[i * 3 + 1],
                                          particleData[i * 3 + 2]));
    }

    return numParticles;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real, class VectorType>
void ParticleHelpers::saveBinary<Real, VectorType>(const std::string& fileName, std::vector<VectorType>& particles, Real& particleRadius)
{
    static_assert(sizeof(Real) * 2 == sizeof(VectorType) || sizeof(Real) * 3 == sizeof(VectorType), "Inconsistent type of particle radius and vector data!");

    DataBuffer buffer;
    UInt     numParticles = static_cast<UInt>(particles.size());

    buffer.push_back(numParticles);
    buffer.push_back(particleRadius);
    buffer.pushFloatArray(particles, false);

    FileHelpers::writeBinaryFile(fileName, buffer.buffer());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
void Banana::ParticleHelpers::jitter<Real>(Vec3<Real>& ppos, Real maxJitter)
{
    ppos += maxJitter * Vec3<Real>(MathHelpers::frand11<Real>(),
                                   MathHelpers::frand11<Real>(),
                                   MathHelpers::frand11<Real>())
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
void Banana::ParticleHelpers::clamp<Real>(Vec3<Real>& ppos, const Vec3<Real>& bmin, const Vec3<Real>& bmax, Real margin /*= 0*/)
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
template<class Real>
void Banana::ParticleHelpers::relaxPosition<Real>(const Vec_Vec3<Real>& particles, RelaxationMethod method)
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
