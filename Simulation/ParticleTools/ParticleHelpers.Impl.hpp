#include "ParticleHelpers.h"
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
template<class VectorType>
UInt ParticleHelpers::loadBinary(const String& fileName, Vector<VectorType>& particles, Real& particleRadius)
{
    DataBuffer buffer;
    __BNN_ASSERT_MSG(FileHelpers::readFile(buffer.buffer(), fileName), "Could not open file for reading.");

    const UInt dimension = sizeof(VectorType) / sizeof(Real);
    float      fRadius;
    UInt       numParticles;
    UInt64     segmentStart = 0;
    UInt64     segmentSize;

    segmentSize = sizeof(UInt);
    memcpy(&numParticles, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = sizeof(float);
    memcpy(&fRadius, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    const float* particleData = reinterpret_cast<const float*>(&buffer.data()[segmentStart]);
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
        particles.push_back(ppos);
    }

    return numParticles;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType>
void ParticleHelpers::saveBinary(const String& fileName, Vector<VectorType>& particles, Real& particleRadius)
{
    static_assert(sizeof(Real) * 2 == sizeof(VectorType) || sizeof(Real) * 3 == sizeof(VectorType), "Inconsistent type of particle radius and vector data!");

    DataBuffer buffer;
    buffer.append(static_cast<UInt>(particles.size()));
    buffer.appendFloat(particleRadius);
    buffer.appendFloatArray(particles, false);
    FileHelpers::writeBinaryFile(buffer.buffer(), fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType>
bool Banana::ParticleHelpers::isInside(const VectorType& ppos, const VectorType& bmin, const VectorType& bmax)
{
    for(int i = 0; i < ppos.length(); ++i)
    {
        if(ppos[i] < bmin[i] || ppos[i] > bmax[i])
            return false;
    }

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType>
void ParticleHelpers::jitter(VectorType& ppos, Real maxJitter)
{
    VectorType pjitter;
    for(Int j = 0; j < pjitter.length(); ++j)
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
template<class VectorType>
void Banana::ParticleHelpers::compress(const Vector<VectorType>& positions, VectorType& bmin, VectorType& bmax, Vec_UInt16& compressedData)
{
    compressedData.resize(dimension * positions.size());
    ParallelSTL::min_max_vector<VectorType>(positions, bmin, bmax);
    const VectorType diff      = bmax - bmin;
    const int        dimension = diff.length();

    ParallelFuncs::parallel_for<size_t>(0, positions.size(),
                                        [&](size_t i)
                                        {
                                            const VectorType& vec = positions[i];
                                            for(int j = 0; j < dimension; ++j)
                                                compressedData[i * dimension + j] = static_cast<UInt16>(std::numeric_limits<UInt16>::max() * (vec[j] - bmin[j]) * diff[j]);
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType>
void Banana::ParticleHelpers::decompress(Vector<VectorType>& positions, const VectorType& bmin, const VectorType& bmax, const Vec_UInt16& compressedData)
{
    const VectorType diff      = bmax - bmin;
    const UInt       dimension = static_cast<UInt>(diff.length());
    __BNN_ASSERT((compressedData.size() / dimension) * dimension == compressedData.size());
    positions.resize(compressedData.size() / dimension);

    ParallelFuncs::parallel_for<size_t>(0, positions.size(),
                                        [&](size_t i)
                                        {
                                            VectorType vec;
                                            for(int j = 0; j < dimension; ++j)
                                                vec[j] = static_cast<decltype(diff[j])>(compressedData[i * dimension + j]) * diff[j] /
                                                         static_cast<decltype(diff[j])>(std::numeric_limits<UInt16>::max()) + bmin[j];
                                            positions[i] = vec;
                                        });
}
