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
template<class IndexType>
Vec2<IndexType> ParticleHelpers::createGrid(const Vec2r& bmin, const Vec2r& bmax, Real spacing)
{
    Vec2r fgrid = (bmax - bmin) / spacing;
    return Vec2<IndexType>(static_cast<IndexType>(round(fgrid[0])),
                           static_cast<IndexType>(round(fgrid[1])));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IndexType>
Vec3<IndexType> ParticleHelpers::createGrid(const Vec3r& bmin, const Vec3r& bmax, Real spacing)
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
