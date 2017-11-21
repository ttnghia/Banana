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

template<Int N, class RealType>
void compress(const Vector<VecX<N, RealType> >& dvec, VecX<N, RealType>& dMin, VecX<N, RealType>& dMax, Vec_UInt16& compressedData)
{
    ParallelSTL::min_max<N, RealType>(dvec, dMin, dMax);
    const VecX<N, RealType> diff = dMax - dMin;

    compressedData.resize(N * dvec.size());
    ParallelFuncs::parallel_for<size_t>(0, dvec.size(),
                                        [&](size_t i)
                                        {
                                            const auto& vec = dvec[i];
                                            for(int j = 0; j < N; ++j) {
                                                compressedData[i * N + j] = static_cast<UInt16>(std::numeric_limits<UInt16>::max() * ((vec[j] - dMin[j]) / diff[j]));
                                            }
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void compress(const Vector<VecX<N, RealType> >& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/)
{
    VecX<N, RealType> dMin, dMax;
    Vec_UInt16        compressedData;
    compress(dvec, dMin, dMax, compressedData);

    // convert bmin and bmax to Vec3f
    VecX<N, float> dMinf, dMaxf;
    for(Int i = 0; i < N; ++i) {
        dMinf[i] = static_cast<float>(dMin[i]);
        dMaxf[i] = static_cast<float>(dMax[i]);
    }
    buffer.clearBuffer();
    if(bWriteVectorSize) { buffer.append(static_cast<UInt>(dvec.size())); }
    buffer.append((const unsigned char*)glm::value_ptr(dMinf), sizeof(float) * N);
    buffer.append((const unsigned char*)glm::value_ptr(dMaxf), sizeof(float) * N);
    buffer.append((const unsigned char*)compressedData.data(), compressedData.size() * sizeof(UInt16));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void compress(const Vector<MatXxX<N, RealType> >& dvec, RealType& dMin, RealType& dMax, Vec_UInt16& compressedData)
{
    Int NN = N * N;
    ParallelSTL::min_max<N, RealType>(dvec, dMin, dMax);
    const RealType diff = dMax - dMin;

    compressedData.resize(NN * dvec.size());
    ParallelFuncs::parallel_for<size_t>(0, dvec.size(),
                                        [&](size_t i)
                                        {
                                            MatXxX<N, RealType> mat = dvec[i];
                                            const RealType* mdata   = glm::value_ptr(mat);
                                            for(int j = 0; j < NN; ++j) {
                                                compressedData[i * NN + j] = static_cast<UInt16>(std::numeric_limits<UInt16>::max() * ((mdata[j] - dMin) / diff));
                                            }
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void compress(const Vector<MatXxX<N, RealType> >& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/)
{
    RealType   dMin, dMax;
    Vec_UInt16 compressedData;
    compress(dvec, dMin, dMax, compressedData);

    float dMinf = static_cast<float>(dMin);
    float dMaxf = static_cast<float>(dMax);
    buffer.clearBuffer();
    if(bWriteVectorSize) { buffer.append(static_cast<UInt>(dvec.size())); }
    buffer.append(&dMinf,                                      sizeof(float));
    buffer.append(&dMaxf,                                      sizeof(float));
    buffer.append((const unsigned char*)compressedData.data(), compressedData.size() * sizeof(UInt16));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void compress(const Vector<RealType>& dvec, RealType& dMin, RealType& dMax, Vec_UInt16& compressedData)
{
    ParallelSTL::min_max<RealType>(dvec, dMin, dMax);
    const RealType diff = dMax - dMin;

    compressedData.resize(dvec.size());
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
    RealType   dMin, dMax;
    Vec_UInt16 compressedData;
    compress(dvec, dMin, dMax, compressedData);

    // convert bmin and bmax to Vec3f
    float dMinf = static_cast<float>(dMin);
    float dMaxf = static_cast<float>(dMax);
    buffer.clearBuffer();
    if(bWriteVectorSize) { buffer.append(static_cast<UInt>(dvec.size())); }
    buffer.append((const unsigned char*)&dMinf,                sizeof(float));
    buffer.append((const unsigned char*)&dMaxf,                sizeof(float));
    buffer.append((const unsigned char*)compressedData.data(), compressedData.size() * sizeof(UInt16));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void compress(const Vector<Vector<RealType> >& dvec, Vector<RealType>& dMin, Vector<RealType>& dMax, Vec_VecUInt16& compressedData)
{
    __BNN_ASSERT(dvec.size() = dMin.size() && dvec.size() == dMax.size());

    compressedData.resize(dvec.size());
    ParallelFuncs::parallel_for<size_t>(0, dvec.size(),
                                        [&](size_t i)
                                        {
                                            compress(dvec[i], dMin[i], dMax[i], compressedData[i]);
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void compress(const Vector<Vector<RealType> >& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/)
{
    Vector<RealType> dMin, dMax;
    Vec_VecUInt16    compressedData;
    compress(dvec, dMin, dMax, compressedData);

    Vector<float> dMinf(dvec.size());
    Vector<float> dMaxf(dvec.size());

    for(size_t i = 0; i < dvec.size(); ++i) {
        dMinf[i] = static_cast<float>(dMin[i]);
        dMaxf[i] = static_cast<float>(dMax[i]);
    }

    buffer.clearBuffer();
    if(bWriteVectorSize) { buffer.append(static_cast<UInt>(dvec.size())); }
    for(size_t i = 0; i < dvec.size(); ++i) {
        buffer.append(static_cast<UInt>(compressedData[i].size()));
        buffer.append((const unsigned char*)&dMinf[i],                sizeof(float));
        buffer.append((const unsigned char*)&dMaxf[i],                sizeof(float));
        buffer.append((const unsigned char*)compressedData[i].data(), compressedData[i].size() * sizeof(UInt16));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void decompress(Vector<VecX<N, RealType> >& dvec, const VecX<N, RealType>& dMin, const VecX<N, RealType>& dMax, const Vec_UInt16& compressedData)
{
    const VecX<N, RealType> diff = dMax - dMin;
    __BNN_ASSERT((compressedData.size() / N) * N == compressedData.size());

    dvec.resize(compressedData.size() / N);
    ParallelFuncs::parallel_for<size_t>(0, dvec.size(),
                                        [&](size_t i)
                                        {
                                            VecX<N, RealType> vec;
                                            for(int j = 0; j < N; ++j) {
                                                vec[j] = static_cast<VecX<N, RealType>::value_type>(compressedData[i * N + j]) * diff[j] /
                                                         static_cast<VecX<N, RealType>::value_type>(std::numeric_limits<UInt16>::max()) + dMin[j];
                                            }
                                            dvec[i] = vec;
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
template<Int N, class RealType>
void decompress(Vector<MatXxX<N, RealType> >& dvec, RealType dMin, RealType dMax, const Vec_UInt16& compressedData)
{
    Int            NN   = N * N;
    const RealType diff = dMax - dMin;
    __BNN_ASSERT((compressedData.size() / NN) * NN == compressedData.size());

    dvec.resize(compressedData.size() / NN);
    ParallelFuncs::parallel_for<size_t>(0, dvec.size(),
                                        [&](size_t i)
                                        {
                                            MatXxX<N, RealType> mat;
                                            RealType* mdata = glm::value_ptr(mat);

                                            for(int j = 0; j < NN; ++j) {
                                                mdata[j] = static_cast<VecX<N, RealType>::value_type>(compressedData[i * NN + j]) * diff /
                                                           static_cast<VecX<N, RealType>::value_type>(std::numeric_limits<UInt16>::max()) + dMin;
                                            }
                                            dvec[i] = mat;
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void decompress(Vector<MatXxX<N, RealType> >& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/)
{
    float  dMinf, dMaxf;
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

    segmentSize = nParticles * N * N * sizeof(UInt16);
    __BNN_ASSERT(segmentStart + segmentSize == buffer.size());
    Vec_UInt16 compressedData(nParticles* N* N);
    memcpy(compressedData.data(), &buffer.data()[segmentStart], segmentSize);

    RealType dMin = static_cast<RealType>(dMinf);
    RealType dMax = static_cast<RealType>(dMaxf);
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
    float  dMinf, dMaxf;
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
void decompress(Vector<Vector<RealType> >& dvec, const Vector<RealType>& dMin, const Vector<RealType>& dMax, const Vec_VecUInt16& compressedData)
{
    __BNN_ASSERT(compressedData.size() = dMin.size() && compressedData.size() == dMax.size());

    dvec.resize(compressedData.size());
    ParallelFuncs::parallel_for<size_t>(0, dvec.size(),
                                        [&](size_t i)
                                        {
                                            decompress(dvec[i], dMin[i], dMax[i], compressedData[i]);
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void decompress(Vector<Vector<RealType> >& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/)
{
    Vector<float> dMinf, dMaxf;

    UInt64 segmentStart = 0;
    UInt64 segmentSize;

    if(nParticles == 0) {
        segmentSize = sizeof(UInt);
        memcpy(&nParticles, &buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;
    }

    dMinf.resize(nParticles);
    dMaxf.resize(nParticles);
    Vec_VecUInt16 compressedData(nParticles);
    for(UInt i = 0; i < nParticles; ++i) {
        UInt iSize;
        segmentSize = sizeof(UInt);
        memcpy(&iSize, &buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        segmentSize = sizeof(float);
        memcpy(&dMinf[i], &buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;
        memcpy(&dMaxf[i], &buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        compressedData[i].resize(iSize);
        segmentSize = iSize * sizeof(UInt16);
        __BNN_ASSERT(segmentStart + segmentSize <= buffer.size());
        memcpy(compressedData[i].data(), &buffer.data()[segmentStart], segmentSize);
    }

    for(UInt i = 0; i < nParticles; ++i) {
        decompress(dvec[i], static_cast<RealType>(dMinf[i]), static_cast<RealType>(dMaxf[i]), compressedData[i]);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void springForceDx(const VecX<N, RealType>& eij, RealType dij, RealType d0, RealType KSpring, RealType KDamping, MatXxX<N, RealType>& springDx, MatXxX<N, RealType>& dampingDx)
{
    const MatXxX<N, RealType> xijxijT = glm::outerProduct(eij, eij);
    dampingDx = xijxijT * (-KDamping);
    springDx  = MatXxX<N, RealType>(1.0) * (dij / d0 - RealType(1.0)) + xijxijT;
    springDx *= (-KSpring / dij);
}
