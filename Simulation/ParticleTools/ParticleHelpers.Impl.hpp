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
    UInt   nParticles;
    UInt64 segmentStart = 0;
    UInt64 segmentSize;

    segmentSize = sizeof(UInt);
    memcpy(&nParticles, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = sizeof(float);
    memcpy(&fRadius, &buffer.data()[segmentStart], segmentSize);
    __BNN_ASSERT_APPROX_NUMBERS(static_cast<float>(particleRadius), fRadius, MEpsilon);
    segmentStart += segmentSize;

    const float* particleData = reinterpret_cast<const float*>(&buffer.data()[segmentStart]);
    __BNN_ASSERT(particleData != nullptr);
    segmentSize   = nParticles * sizeof(float) * N;
    segmentStart += segmentSize;
    __BNN_ASSERT(segmentStart == buffer.size());

    ////////////////////////////////////////////////////////////////////////////////
    particles.reserve(particles.size() + static_cast<size_t>(nParticles));

    for(UInt i = 0; i < nParticles; ++i) {
        VecX<N, RealType> ppos;
        for(UInt j = 0; j < N; ++j)
            ppos[j] = RealType(particleData[i * N + j]);
        particles.push_back(ppos);
    }

    return nParticles;
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
void compress(const Vector<VecX<N, RealType> >& positions, VecX<N, RealType>& dMin, VecX<N, RealType>& dMax, Vec_UInt16& compressedData)
{
    compressedData.resize(N * positions.size());
    ParallelSTL::min_max_vector<N, RealType>(positions, dMin, dMax);
    const VecX<N, RealType> diff = dMax - dMin;

    ParallelFuncs::parallel_for<size_t>(0, positions.size(),
                                        [&](size_t i)
                                        {
                                            const auto& vec = positions[i];
                                            for(int j = 0; j < N; ++j)
                                                compressedData[i * N + j] = static_cast<UInt16>(std::numeric_limits<UInt16>::max() * ((vec[j] - dMin[j]) / diff[j]));
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void compress(const Vector<VecX<N, RealType> >& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/)
{
    Vec_UInt16        compressedData(N* dvec.size());
    VecX<N, RealType> dMin, dMax;
    VecX<N, float>    dMinf, dMaxf;
    ParallelSTL::min_max_vector<N, RealType>(dvec, dMin, dMax);

    // convert bmin and bmax to Vec3f
    for(Int i = 0; i < N; ++i) {
        dMinf[i] = static_cast<float>(dMin[i]);
        dMaxf[i] = static_cast<float>(dMax[i]);
    }

    const VecX<N, RealType> diff = dMax - dMin;
    ParallelFuncs::parallel_for<size_t>(0, dvec.size(),
                                        [&](size_t i)
                                        {
                                            const auto& vec = dvec[i];
                                            for(int j = 0; j < N; ++j)
                                                compressedData[i * N + j] = static_cast<UInt16>(std::numeric_limits<UInt16>::max() * ((vec[j] - dMin[j]) / diff[j]));
                                        });

    buffer.clearBuffer();
    if(bWriteVectorSize) buffer.append(static_cast<UInt>(dvec.size()));
    buffer.append((const unsigned char*)glm::value_ptr(dMinf), sizeof(float) * N);
    buffer.append((const unsigned char*)glm::value_ptr(dMaxf), sizeof(float) * N);
    buffer.append((const unsigned char*)compressedData.data(), compressedData.size() * sizeof(UInt16));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void compress(const Vector<RealType>& dvec, RealType& dMin, RealType& dMax, Vec_UInt16& compressedData)
{
    compressedData.resize(dvec.size());
    ParallelSTL::min_max<RealType>(dvec, dMin, dMax);
    const RealType diff = dMax - dMin;

    ParallelFuncs::parallel_for<size_t>(0, dvec.size(),
                                        [&](size_t i)
                                        {
                                            compressedData[i] = static_cast<UInt16>(std::numeric_limits<UInt16>::max() * ((dvec[i] - dMin) / diff));
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void compress(const Vector<RealType>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/)
{
    RealType dMin, dMax;
    ParallelSTL::min_max<RealType>(dvec, dMin, dMax);

    // convert bmin and bmax to Vec3f
    float dMinf = static_cast<float>(dMin);
    float dMaxf = static_cast<float>(dMax);

    const RealType diff = dMax - dMin;
    Vec_UInt16     compressedData(dvec.size());
    ParallelFuncs::parallel_for<size_t>(0, dvec.size(),
                                        [&](size_t i)
                                        {
                                            compressedData[i] = static_cast<UInt16>(std::numeric_limits<UInt16>::max() * ((dvec[i] - dMin) / diff));
                                        });

    buffer.clearBuffer();
    if(bWriteVectorSize) buffer.append(static_cast<UInt>(dvec.size()));
    buffer.append((const unsigned char*)&dMinf, sizeof(float));
    buffer.append((const unsigned char*)&dMaxf, sizeof(float));
    buffer.append((const unsigned char*)compressedData.data(), compressedData.size() * sizeof(UInt16));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void compress(const Vector<Vector<RealType> >& dvec, RealType& dMin, RealType& dMax, Vec_VecUInt16& compressedData)
{
    ParallelSTL::min_max<RealType>(dvec, dMin, dMax);
    const RealType diff = dMax - dMin;

    compressedData.resize(dvec.size());
    ParallelFuncs::parallel_for<size_t>(0, dvec.size(),
                                        [&](size_t i)
                                        {
                                            compressedData[i].resize(dvec[i].size());
                                            for(size_t j = 0; j < dvec[i].size(); ++j)
                                                compressedData[i][j] = static_cast<UInt16>(std::numeric_limits<UInt16>::max() * ((dvec[i][j] - dMin) / diff));
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void compress(const Vector<Vector<RealType> >& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/)
{
    RealType dMin, dMax;
    ParallelSTL::min_max<RealType>(dvec, dMin, dMax);

    // convert bmin and bmax to Vec3f
    float dMinf = static_cast<float>(dMin);
    float dMaxf = static_cast<float>(dMax);

    const RealType diff = dMax - dMin;
    Vec_UInt16     compressedData(dvec.size());
    ParallelFuncs::parallel_for<size_t>(0, dvec.size(),
                                        [&](size_t i)
                                        {
                                            compressedData[i].resize(dvec[i].size());
                                            for(size_t j = 0; j < dvec[i].size(); ++j)
                                                compressedData[i][j] = static_cast<UInt16>(std::numeric_limits<UInt16>::max() * ((dvec[i][j] - dMin) / diff));
                                        });

    buffer.clearBuffer();
    if(bWriteVectorSize) buffer.append(static_cast<UInt>(dvec.size()));
    buffer.append((const unsigned char*)&dMinf, sizeof(float));
    buffer.append((const unsigned char*)&dMaxf, sizeof(float));
    for(size_t i = 0; i < dvec.size(); ++i) {
        buffer.append(static_cast<UInt>(compressedData[i].size()));
        buffer.append((const unsigned char*)compressedData[i].data(), compressedData[i].size() * sizeof(UInt16));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void compressAndSaveBinary(const String& fileName, const Vector<VecX<N, RealType> >& positions, RealType particleRadius)
{
    // compress
    Vec_UInt16        compressedData;
    VecX<N, RealType> dMin, dMax;
    compress(positions, dMin, dMax, compressedData);

    VecX<N, float> dMinf, dMaxf;
    // convert bmin and bmax to Vec3f
    for(Int i = 0; i < N; ++i) {
        dMinf[i] = static_cast<float>(dMin[i]);
        dMaxf[i] = static_cast<float>(dMax[i]);
    }

    // save
    DataBuffer buffer;
    buffer.append(static_cast<UInt>(positions.size()));
    buffer.appendFloat(particleRadius);
    buffer.append((const unsigned char*)glm::value_ptr(dMinf), sizeof(float) * N);
    buffer.append((const unsigned char*)glm::value_ptr(dMaxf), sizeof(float) * N);
    buffer.append((const unsigned char*)compressedData.data(), compressedData.size() * sizeof(UInt16));
    FileHelpers::writeBinaryFile(buffer.buffer(), fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void compressAndSaveBinary(const String& fileName, const Vector<RealType>& dvec)
{
    // compress
    Vec_UInt16 compressedData;
    RealType   dMin, dMax;
    compress(dvec, dMin, dMax, compressedData);

    // convert bmin and bmax to Vec3f
    float dMinf = static_cast<float>(dMin);
    float dMaxf = static_cast<float>(dMax);

    // save
    DataBuffer buffer;
    buffer.append(static_cast<UInt>(dvec.size()));
    buffer.append((const unsigned char*)&dMaxf, sizeof(float));
    buffer.append((const unsigned char*)&dMinf, sizeof(float));
    buffer.append((const unsigned char*)compressedData.data(), compressedData.size() * sizeof(UInt16));
    FileHelpers::writeBinaryFile(buffer.buffer(), fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void decompress(Vector<VecX<N, RealType> >& positions, const VecX<N, RealType>& dMin, const VecX<N, RealType>& dMax, const Vec_UInt16& compressedData)
{
    const VecX<N, RealType> diff = dMax - dMin;
    __BNN_ASSERT((compressedData.size() / N) * N == compressedData.size());
    positions.resize(compressedData.size() / N);

    ParallelFuncs::parallel_for<size_t>(0, positions.size(),
                                        [&](size_t i)
                                        {
                                            VecX<N, RealType> vec;
                                            for(int j = 0; j < N; ++j)
                                                vec[j] = static_cast<VecX<N, RealType>::value_type>(compressedData[i * N + j]) * diff[j] /
                                                         static_cast<VecX<N, RealType>::value_type>(std::numeric_limits<UInt16>::max()) + dMin[j];
                                            positions[i] = vec;
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void decompress(Vector<VecX<N, RealType> >& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/)
{
    VecX<N, float> dMinf, dMaxf;

    UInt64 segmentStart = 0;
    UInt64 segmentSize;

    if(nParticles == 0) {
        segmentSize = sizeof(UInt);
        memcpy(&nParticles, &buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;
    }

    segmentSize = sizeof(float) * N;
    memcpy(glm::value_ptr(dMinf), &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;
    memcpy(glm::value_ptr(dMaxf), &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = nParticles * N * sizeof(UInt16);
    __BNN_ASSERT(segmentStart + segmentSize == buffer.size());
    Vec_UInt16 compressedData(nParticles* N);
    memcpy(compressedData.data(), &buffer.data()[segmentStart], segmentSize);

    VecX<N, RealType> dMin, dMax;
    for(Int i = 0; i < N; ++i) {
        dMin[i] = static_cast<RealType>(dMinf[i]);
        dMax[i] = static_cast<RealType>(dMaxf[i]);
    }
    decompress(dvec, dMin, dMax, compressedData);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void decompress(Vector<RealType>& dvec, RealType dMin, RealType dMax, const Vec_UInt16& compressedData)
{
    const RealType diff = dMax - dMin;
    dvec.resize(compressedData.size());

    ParallelFuncs::parallel_for<size_t>(0, dvec.size(),
                                        [&](size_t i)
                                        {
                                            dvec[i] = static_cast<RealType>(compressedData[i]) * diff / static_cast<RealType>(std::numeric_limits<UInt16>::max()) + dMin;
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void decompress(Vector<RealType>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/)
{
    float dMinf, dMaxf;

    UInt64 segmentStart = 0;
    UInt64 segmentSize;

    if(nParticles == 0) {
        segmentSize = sizeof(UInt);
        memcpy(&nParticles, &buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;
    }

    segmentSize = sizeof(float);
    memcpy(&dMinf, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;
    memcpy(&dMaxf, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = nParticles * sizeof(UInt16);
    __BNN_ASSERT(segmentStart + segmentSize == buffer.size());
    Vec_UInt16 compressedData(nParticles);
    memcpy(compressedData.data(), &buffer.data()[segmentStart], segmentSize);

    RealType dMin = static_cast<RealType>(dMinf);
    RealType dMax = static_cast<RealType>(dMaxf);
    decompress(dvec, dMin, dMax, compressedData);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void decompress(Vector<Vector<RealType> >& dvec, RealType dMin, RealType dMax, const Vec_VecUInt16& compressedData)
{
    const RealType diff = dMax - dMin;
    dvec.resize(compressedData.size());

    ParallelFuncs::parallel_for<size_t>(0, dvec.size(),
                                        [&](size_t i)
                                        {
                                            dvec[i].resize(compressedData[i].size());
                                            for(size_t j = 0; j < dvec[i].size(); ++j)
                                                dvec[i][j] = static_cast<RealType>(compressedData[i][j]) * diff / static_cast<RealType>(std::numeric_limits<UInt16>::max()) + dMin;
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void decompress(Vector<Vector<RealType> >& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/)
{
    float dMinf, dMaxf;

    UInt64 segmentStart = 0;
    UInt64 segmentSize;

    if(nParticles == 0) {
        segmentSize = sizeof(UInt);
        memcpy(&nParticles, &buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;
    }

    segmentSize = sizeof(float);
    memcpy(&dMinf, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;
    memcpy(&dMaxf, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    Vec_VecUInt16 compressedData(nParticles);
    for(UInt i = 0; i < nParticles; ++i) {
        UInt iSize;
        segmentSize = sizeof(UInt);
        memcpy(&iSize, &buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        compressedData[i].resize(iSize);
        segmentSize = iSize * sizeof(UInt16);
        __BNN_ASSERT(segmentStart + segmentSize <= buffer.size());
        memcpy(compressedData[i].data(), &buffer.data()[segmentStart], segmentSize);
    }

    RealType dMin = static_cast<RealType>(dMinf);
    RealType dMax = static_cast<RealType>(dMaxf);
    decompress(dvec, dMin, dMax, compressedData);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
UInt loadBinaryAndDecompress(const String& fileName, Vector<VecX<N, RealType> >& positions, RealType& particleRadius)
{
    DataBuffer buffer;
    __BNN_ASSERT_MSG(FileHelpers::readFile(buffer.buffer(), fileName), "Could not open file for reading.");

    UInt           nParticles;
    float          fRadius;
    VecX<N, float> dMinf, dMaxf;

    UInt64 segmentStart = 0;
    UInt64 segmentSize;

    segmentSize = sizeof(UInt);
    memcpy(&nParticles, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = sizeof(float);
    memcpy(&fRadius, &buffer.data()[segmentStart], segmentSize);
    //__BNN_ASSERT_APPROX_NUMBERS(static_cast<float>(particleRadius), fRadius, MEpsilon);
    particleRadius = static_cast<RealType>(fRadius);
    segmentStart  += segmentSize;

    segmentSize = sizeof(float) * N;
    memcpy(glm::value_ptr(dMinf), &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;
    memcpy(glm::value_ptr(dMaxf), &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = nParticles * N * sizeof(UInt16);
    __BNN_ASSERT(segmentStart + segmentSize == buffer.size());
    Vec_UInt16 compressedData(nParticles* N);
    memcpy(compressedData.data(), &buffer.data()[segmentStart], segmentSize);

    VecX<N, RealType> dMin, dMax;
    for(Int i = 0; i < N; ++i) {
        dMin[i] = static_cast<RealType>(dMinf[i]);
        dMax[i] = static_cast<RealType>(dMaxf[i]);
    }
    decompress(positions, dMin, dMax, compressedData);
    return nParticles;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
UInt loadBinaryAndDecompress(const String& fileName, Vector<RealType>& dvec)
{
    DataBuffer buffer;
    __BNN_ASSERT_MSG(FileHelpers::readFile(buffer.buffer(), fileName), "Could not open file for reading.");

    UInt  nParticles;
    float dMinf, dMaxf;

    UInt64 segmentStart = 0;
    UInt64 segmentSize;

    segmentSize = sizeof(UInt);
    memcpy(&nParticles, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = sizeof(float);
    memcpy(&dMinf, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;
    memcpy(&dMaxf, &buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = nParticles * sizeof(UInt16);
    __BNN_ASSERT(segmentStart + segmentSize == buffer.size());
    Vec_UInt16 compressedData(nParticles);
    memcpy(compressedData.data(), &buffer.data()[segmentStart], segmentSize);

    RealType dMin = static_cast<RealType>(dMinf);
    RealType dMax = static_cast<RealType>(dMaxf);
    decompress(dvec, dMin, dMax, compressedData);
    return nParticles;
}