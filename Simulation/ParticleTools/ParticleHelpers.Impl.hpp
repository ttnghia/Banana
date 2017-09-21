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
template<Int N, class RealType>
UInt loadBinary(const String& fileName, Vector<VecX<N, RealType> >& particles, RealType particleRadius)
{
    DataBuffer buffer;
    __BNN_ASSERT_MSG(FileHelpers::readFile(buffer.buffer(), fileName), "Could not open file for reading.");

    float  fRadius;
    UInt   numParticles;
    UInt64 segmentStart = 0;
    UInt64 segmentSize;

    segmentSize = sizeof(UInt);
    memcpy(&numParticles, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = sizeof(float);
    memcpy(&fRadius, &buffer.data()[segmentStart], segmentSize);
    __BNN_ASSERT_APPROX_NUMBERS(static_cast<float>(particleRadius), fRadius, MEpsilon);
    segmentStart += segmentSize;

    const float* particleData = reinterpret_cast<const float*>(&buffer.data()[segmentStart]);
    __BNN_ASSERT(particleData != nullptr);
    segmentSize   = numParticles * sizeof(float) * N;
    segmentStart += segmentSize;
    __BNN_ASSERT(segmentStart == buffer.size());

    ////////////////////////////////////////////////////////////////////////////////
    particles.reserve(particles.size() + static_cast<size_t>(numParticles));

    for(UInt i = 0; i < numParticles; ++i)
    {
        VecX<N, RealType> ppos;
        for(UInt j = 0; j < N; ++j)
            ppos[j] = RealType(particleData[i * N + j]);
        particles.push_back(ppos);
    }

    return numParticles;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void saveBinary(const String& fileName, Vector<VecX<N, RealType> >& particles, RealType particleRadius)
{
    DataBuffer buffer;
    buffer.append(static_cast<UInt>(particles.size()));
    buffer.appendFloat(particleRadius);
    buffer.appendFloatArray(particles, false);
    FileHelpers::writeBinaryFile(buffer.buffer(), fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void compress(const Vector<VecX<N, RealType> >& positions, VecX<N, RealType>& bMin, VecX<N, RealType>& bMax, Vec_UInt16& compressedData)
{
    compressedData.resize(N * positions.size());
    ParallelSTL::min_max_vector<N, RealType>(positions, bMin, bMax);
    const VecX<N, RealType> diff = bMax - bMin;

    ParallelFuncs::parallel_for<size_t>(0, positions.size(),
                                        [&](size_t i)
                                        {
                                            const auto& vec = positions[i];
                                            for(int j = 0; j < N; ++j)
                                                compressedData[i * N + j] = static_cast<UInt16>(std::numeric_limits<UInt16>::max() * ((vec[j] - bMin[j]) / diff[j]));
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void decompress(Vector<VecX<N, RealType> >& positions, const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax, const Vec_UInt16& compressedData)
{
    const VecX<N, RealType> diff = bMax - bMin;
    __BNN_ASSERT((compressedData.size() / N) * N == compressedData.size());
    positions.resize(compressedData.size() / N);

    ParallelFuncs::parallel_for<size_t>(0, positions.size(),
                                        [&](size_t i)
                                        {
                                            VecX<N, RealType> vec;
                                            for(int j = 0; j < N; ++j)
                                                vec[j] = static_cast<VecX<N, RealType>::value_type>(compressedData[i * N + j]) * diff[j] /
                                                         static_cast<VecX<N, RealType>::value_type>(std::numeric_limits<UInt16>::max()) + bMin[j];
                                            positions[i] = vec;
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void compressAndSaveBinary(const String& fileName, const Vector<VecX<N, RealType> >& positions, RealType particleRadius)
{
    // compress
    Vec_UInt16        compressedData;
    VecX<N, RealType> bMin, bMax;
    compress(positions, bMin, bMax, compressedData);

    VecX<N, float> bMinf, bMaxf;
    // convert bmin and bmax to Vec3f
    for(Int i = 0; i < N; ++i)
    {
        bMinf[i] = static_cast<float>(bMin[i]);
        bMaxf[i] = static_cast<float>(bMax[i]);
    }

    // save
    DataBuffer buffer;
    buffer.append(static_cast<UInt>(positions.size()));
    buffer.appendFloat(particleRadius);
    buffer.append((const unsigned char*)glm::value_ptr(bMinf), sizeof(float) * N);
    buffer.append((const unsigned char*)glm::value_ptr(bMaxf), sizeof(float) * N);
    buffer.append((const unsigned char*)compressedData.data(), compressedData.size() * sizeof(UInt16));
    FileHelpers::writeBinaryFile(buffer.buffer(), fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
UInt loadBinaryAndDecompress(const String& fileName, Vector<VecX<N, RealType> >& positions, RealType particleRadius)
{
    DataBuffer buffer;
    __BNN_ASSERT_MSG(FileHelpers::readFile(buffer.buffer(), fileName), "Could not open file for reading.");

    UInt           numParticles;
    float          fRadius;
    VecX<N, float> bMinf, bMaxf;

    UInt64 segmentStart = 0;
    UInt64 segmentSize;

    segmentSize = sizeof(UInt);
    memcpy(&numParticles, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = sizeof(float);
    memcpy(&fRadius, &buffer.data()[segmentStart], segmentSize);
    __BNN_ASSERT_APPROX_NUMBERS(static_cast<float>(particleRadius), fRadius, MEpsilon);
    segmentStart += segmentSize;

    segmentSize = sizeof(float) * N;
    memcpy(glm::value_ptr(bMinf), &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;
    memcpy(glm::value_ptr(bMaxf), &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = numParticles * N * sizeof(UInt16);
    __BNN_ASSERT(segmentStart + segmentSize == buffer.size());
    Vec_UInt16 compressedData(numParticles* N);
    memcpy(compressedData.data(), &buffer.data()[segmentStart], segmentSize);

    VecX<N, RealType> bMin, bMax;
    for(Int i = 0; i < N; ++i)
    {
        bMin[i] = static_cast<RealType>(bMinf[i]);
        bMax[i] = static_cast<RealType>(bMaxf[i]);
    }
    decompress(positions, bMin, bMax, compressedData);
    return numParticles;
}