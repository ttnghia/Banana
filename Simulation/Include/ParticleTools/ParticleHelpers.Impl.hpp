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
template<class IndexType>
Vec2<IndexType> ParticleHelpers::createGrid(const Vec2r& bmin, const Vec2r& bmax, Real spacing)
{
    Vec2r fgrid = (bmax - bmin) / spacing;
    return Vec2<IndexType>(static_cast<IndexType>(round(fgrid[0])),
                           static_cast<IndexType>(round(fgrid[1])));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IndexType>
Vec3<IndexType> ParticleHelpers::createGrid<Real>(const Vec3r& bmin, const Vec3r& bmax, Real spacing)
{
    Vec3r fgrid = (bmax - bmin) / spacing;
    return Vec3<IndexType>(static_cast<IndexType>(round(fgrid[0])),
                           static_cast<IndexType>(round(fgrid[1])),
                           static_cast<IndexType>(round(fgrid[2])));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType>
UInt ParticleHelpers::loadBinary(const String& fileName, Vector<VectorType>& particles, Real& particleRadius)
{
    DataBuffer buffer;
    __BNN_ASSERT_MSG(FileHelpers::readFile(buffer.buffer(), fileName), "Could not open file for reading.");

    UInt   dimension = sizeof(VectorType) / sizeof(Real);
    float  fRadius;
    UInt   numParticles;
    UInt64 segmentStart = 0;
    UInt64 segmentSize;

    segmentSize = sizeof(UInt);
    memcpy(&numParticles, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = sizeof(float);
    memcpy(&fRadius, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    float* particleData = reinterpret_cast<float*>(&buffer.data()[segmentStart]);
    __BNN_ASSERT(particleData != nullptr);
    segmentSize   = numParticles * sizeof(float) * dimension;
    segmentStart += segmentSize;
    __BNN_ASSERT(segmentStart == buffer.size());

    ////////////////////////////////////////////////////////////////////////////////
    particles.reserve(particles.size() + static_cast<size_t>(numParticles));

    for(UInt i = 0; i < numParticles; ++i)
    {
        VectorType ppos;
        for(UInt j = 0; j < dimension; ++j)
            ppos[j] = Real(particleData[i * dimension + j]);
        particles.push_back(v);
    }

    return numParticles;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real, class VectorType>
void ParticleHelpers::saveBinary<Real, VectorType>(const String& fileName, Vector<VectorType>& particles, Real& particleRadius)
{
    static_assert(sizeof(Real) * 2 == sizeof(VectorType) || sizeof(Real) * 3 == sizeof(VectorType), "Inconsistent type of particle radius and vector data!");

    DataBuffer buffer;
    buffer.append(static_cast<UInt>(particles.size()));
    buffer.appendFloat(particleRadius);
    buffer.appendFloatArray(particles, false);
    FileHelpers::writeBinaryFile(fileName, buffer.buffer());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType>
void ParticleHelpers::jitter(VectorType& ppos, Real maxJitter)
{
    VectorType pjitter;
    for(UInt j = 0; j < pjitter.length(); ++j)
        pjitter[j] = MathHelpers::frand11<Real>();

    ppos += maxJitter * pjitter;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType>
void ParticleHelpers::clamp(VectorType& ppos, const VectorType& bmin, const VectorType& bmax, Real margin /*= 0*/)
{
    UInt dimension = sizeof(VectorType) / sizeof(Real);
    for(UInt i = 0; i < dimension; ++i)
    {
        if(ppos[i] < bmin[i] + margin ||
           ppos[i] > bmax[i] - margin)
            ppos[i] = MathHelpers::min(MathHelpers::max(ppos[i],
                                                        bmin[i] + margin),
                                       bmax[i] - margin);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
void ParticleHelpers::relaxPosition<Real>(const Vec_Vec3r& particles, RelaxationMethod method)
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
void transform(Vec_Vec3r& particles, const Vec3r& translation, const Vec3r& rotation)
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
