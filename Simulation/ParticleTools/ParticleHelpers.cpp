//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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

#include <Banana/Utils/FileHelpers.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/ParallelHelpers/Scheduler.h>
#include <ParticleTools/ParticleHelpers.h>

#include <string>
#include <cmath>
#include <vector>
#include <cassert>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
auto getAABB(const Vec_VecX<N, RealType>& positions)
{
    VecX<N, RealType> bMin, bMax;
    ParallelSTL::min_max<N, RealType>(positions, bMin, bMax);
    return std::make_tuple(bMin, bMax);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void compress(const Vec_VecX<N, RealType>& dvec, VecX<N, RealType>& dMin, VecX<N, RealType>& dMax, Vec_UInt16& compressedData)
{
    ParallelSTL::min_max<N, RealType>(dvec, dMin, dMax);
    const VecX<N, RealType> diff = dMax - dMin;

    compressedData.resize(N * dvec.size());
    Scheduler::parallel_for(dvec.size(),
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
void compress(const Vec_VecX<N, RealType>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/)
{
    VecX<N, RealType> dMin, dMax;
    Vec_UInt16        compressedData;
    compress(dvec, dMin, dMax, compressedData);

    // convert bmin and bmax to Vec3f
    VecX<N, float> dMinf, dMaxf;
    for(Int d = 0; d < N; ++d) {
        dMinf[d] = static_cast<float>(dMin[d]);
        dMaxf[d] = static_cast<float>(dMax[d]);
    }
    buffer.clearBuffer();
    if(bWriteVectorSize) { buffer.append(static_cast<UInt>(dvec.size())); }
    buffer.append((const unsigned char*)glm::value_ptr(dMinf), sizeof(float) * N);
    buffer.append((const unsigned char*)glm::value_ptr(dMaxf), sizeof(float) * N);
    buffer.append((const unsigned char*)compressedData.data(), compressedData.size() * sizeof(UInt16));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void compress(const Vector<MatXxX<N, RealType>>& dvec, RealType& dMin, RealType& dMax, Vec_UInt16& compressedData)
{
    Int NN = N * N;
    ParallelSTL::min_max<N, RealType>(dvec, dMin, dMax);
    const RealType diff = dMax - dMin;

    compressedData.resize(NN * dvec.size());
    Scheduler::parallel_for(dvec.size(),
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
void compress(const Vector<MatXxX<N, RealType>>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/)
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
    Scheduler::parallel_for(dvec.size(),
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
void compress(const Vector<Vector<RealType>>& dvec, Vector<RealType>& dMin, Vector<RealType>& dMax, Vec_VecUInt16& compressedData)
{
    __BNN_REQUIRE(dvec.size() == dMin.size() && dvec.size() == dMax.size());

    compressedData.resize(dvec.size());
    Scheduler::parallel_for(dvec.size(), [&](size_t i) { compress(dvec[i], dMin[i], dMax[i], compressedData[i]); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void compress(const Vector<Vector<RealType>>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/)
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
void decompress(Vec_VecX<N, RealType>& dvec, const VecX<N, RealType>& dMin, const VecX<N, RealType>& dMax, const Vec_UInt16& compressedData)
{
    const VecX<N, RealType> diff = dMax - dMin;
    __BNN_REQUIRE((compressedData.size() / N) * N == compressedData.size());

    dvec.resize(compressedData.size() / N);
    Scheduler::parallel_for(dvec.size(),
                            [&](size_t i)
                            {
                                VecX<N, RealType> vec;
                                for(int j = 0; j < N; ++j) {
                                    vec[j] = static_cast<typename VecX<N, RealType>::value_type>(compressedData[i * N + j]) * diff[j] /
                                             static_cast<typename VecX<N, RealType>::value_type>(std::numeric_limits<UInt16>::max()) + dMin[j];
                                }
                                dvec[i] = vec;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void decompress(Vec_VecX<N, RealType>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/)
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
    __BNN_REQUIRE(segmentStart + segmentSize == buffer.size());
    Vec_UInt16 compressedData(nParticles * N);
    memcpy(compressedData.data(), &buffer.data()[segmentStart], segmentSize);

    VecX<N, RealType> dMin, dMax;
    for(Int d = 0; d < N; ++d) {
        dMin[d] = static_cast<RealType>(dMinf[d]);
        dMax[d] = static_cast<RealType>(dMaxf[d]);
    }
    decompress(dvec, dMin, dMax, compressedData);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void decompress(Vector<MatXxX<N, RealType>>& dvec, RealType dMin, RealType dMax, const Vec_UInt16& compressedData)
{
    Int            NN   = N * N;
    const RealType diff = dMax - dMin;
    __BNN_REQUIRE((compressedData.size() / NN) * NN == compressedData.size());

    dvec.resize(compressedData.size() / NN);
    Scheduler::parallel_for(dvec.size(),
                            [&](size_t i)
                            {
                                MatXxX<N, RealType> mat;
                                RealType* mdata = glm::value_ptr(mat);

                                for(int j = 0; j < NN; ++j) {
                                    mdata[j] = static_cast<typename VecX<N, RealType>::value_type>(compressedData[i * NN + j]) * diff /
                                               static_cast<typename VecX<N, RealType>::value_type>(std::numeric_limits<UInt16>::max()) + dMin;
                                }
                                dvec[i] = mat;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void decompress(Vector<MatXxX<N, RealType>>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/)
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
    __BNN_REQUIRE(segmentStart + segmentSize == buffer.size());
    Vec_UInt16 compressedData(nParticles * N * N);
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
    Scheduler::parallel_for(dvec.size(),
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
    __BNN_REQUIRE(segmentStart + segmentSize == buffer.size());
    Vec_UInt16 compressedData(nParticles);
    memcpy(compressedData.data(), &buffer.data()[segmentStart], segmentSize);

    RealType dMin = static_cast<RealType>(dMinf);
    RealType dMax = static_cast<RealType>(dMaxf);
    decompress(dvec, dMin, dMax, compressedData);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void decompress(Vector<Vector<RealType>>& dvec, const Vector<RealType>& dMin, const Vector<RealType>& dMax, const Vec_VecUInt16& compressedData)
{
    __BNN_REQUIRE(compressedData.size() = dMin.size() && compressedData.size() == dMax.size());

    dvec.resize(compressedData.size());
    Scheduler::parallel_for(dvec.size(), [&](size_t i) { decompress(dvec[i], dMin[i], dMax[i], compressedData[i]); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void decompress(Vector<Vector<RealType>>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/)
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
        __BNN_REQUIRE(segmentStart + segmentSize <= buffer.size());
        memcpy(compressedData[i].data(), &buffer.data()[segmentStart], segmentSize);
    }

    for(UInt i = 0; i < nParticles; ++i) {
        decompress(dvec[i], static_cast<RealType>(dMinf[i]), static_cast<RealType>(dMaxf[i]), compressedData[i]);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// non-template functions
void connectedComponentAnalysis(const Vec_VecUInt& connectionList, Vec_Int8& componentIdx, UInt& nComponents)
{
    componentIdx.assign(connectionList.size(), Int8(-1));

    // label from first particle
    UInt nProcessed = 0;
    nProcessed += spawnComponent(0, 0, 0, connectionList, componentIdx);

    nComponents = 1;
    Int8 currentCompIdx = 0;

    bool new_label = false;

    while(nProcessed < connectionList.size()) {
        bool bLabeled = false;

        for(UInt p = 0; p < connectionList.size(); ++p) {
            // Firstly, find any particle that has not been label
            // and that particle has a labeled neighbor
            if(componentIdx[p] >= 0) {
                continue;
            }

            if(new_label) {
                nProcessed += spawnComponent(p, 0, currentCompIdx, connectionList, componentIdx);

                bLabeled  = true;
                new_label = false;
            } else {
                // find the component index from neighbor
                Int8 neighborCompIdx = -1;

                for(UInt q : connectionList[p]) {
                    if(componentIdx[q] >= 0) {
                        neighborCompIdx = componentIdx[q];
                        break;
                    }
                }

                // has a labeled neighbor?
                // get component id from neighbor
                if(neighborCompIdx >= 0) {
                    nProcessed += spawnComponent(p, 0, neighborCompIdx, connectionList, componentIdx);
                    bLabeled    = true;
                }
            }
        }

        // not any particle has been labeled in the last loop
        // while num_process still < num particles
        // that means, we arrive at a new component
        if(!bLabeled) {
            ++currentCompIdx;
            ++nComponents;

            new_label = true;
        }
    }

    __BNN_REQUIRE(nProcessed == connectionList.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
UInt spawnComponent(UInt p, Int depth, UInt8 currentIdx, const Vec_VecUInt& connectionList, Vec_Int8& componentIdx)
{
    componentIdx[p] = currentIdx;
    UInt nProcessed = 1;

    // max depth = 1024 to avoid stack overflow due to many time recursively call this function
    if(depth < 1024) {
        for(UInt q : connectionList[p]) {
            if(componentIdx[q] < 0) {
                nProcessed += spawnComponent(q, depth + 1, currentIdx, connectionList, componentIdx);
            }
        }
    }

    return nProcessed;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template auto getAABB<2, Real>(const Vec_VecX<2, Real>& positions);
template auto getAABB<3, Real>(const Vec_VecX<3, Real>& positions);
////////////////////////////////////////////////////////////////////////////////
template void compress<2, Real>(const Vec_VecX<2, Real>& dvec, VecX<2, Real>& dMin, VecX<2, Real>& dMax, Vec_UInt16& compressedData);
template void compress<3, Real>(const Vec_VecX<3, Real>& dvec, VecX<3, Real>& dMin, VecX<3, Real>& dMax, Vec_UInt16& compressedData);

template void compress<2, Real>(const Vec_VecX<2, Real>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/);
template void compress<3, Real>(const Vec_VecX<3, Real>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/);

template void compress<2, Real>(const Vector<MatXxX<2, Real>>& dvec, Real& dMin, Real& dMax, Vec_UInt16& compressedData);
template void compress<3, Real>(const Vector<MatXxX<3, Real>>& dvec, Real& dMin, Real& dMax, Vec_UInt16& compressedData);

template void compress<2, Real>(const Vector<MatXxX<2, Real>>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/);
template void compress<3, Real>(const Vector<MatXxX<3, Real>>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/);
////////////////////////////////////////////////////////////////////////////////
template void compress<Real>(const Vector<Real>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/);
template void compress<Int>(const Vector<Int>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/);
template void compress<Int16>(const Vector<Int16>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/);
template void compress<UInt>(const Vector<UInt>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/);
template void compress<UInt16>(const Vector<UInt16>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/);

template void compress<Real>(const Vector<Real>& dvec, Real& dMin, Real& dMax, Vec_UInt16& compressedData);
template void compress<Real>(const Vector<Vector<Real>>& dvec, Vector<Real>& dMin, Vector<Real>& dMax, Vec_VecUInt16& compressedData);

template void compress<Real>(const Vector<Vector<Real>>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/);
template void compress<Int>(const Vector<Vector<Int>>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/);
template void compress<Int16>(const Vector<Vector<Int16>>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/);
template void compress<UInt>(const Vector<Vector<UInt>>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/);
template void compress<UInt16>(const Vector<Vector<UInt16>>& dvec, DataBuffer& buffer, bool bWriteVectorSize /*= true*/);
////////////////////////////////////////////////////////////////////////////////
template void decompress<2, Real>(Vec_VecX<2, Real>& dvec, const VecX<2, Real>& dMin, const VecX<2, Real>& dMax, const Vec_UInt16& compressedData);
template void decompress<3, Real>(Vec_VecX<3, Real>& dvec, const VecX<3, Real>& dMin, const VecX<3, Real>& dMax, const Vec_UInt16& compressedData);

template void decompress<2, Real>(Vec_VecX<2, Real>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/);
template void decompress<3, Real>(Vec_VecX<3, Real>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/);

template void decompress<2, Real>(Vector<MatXxX<2, Real>>& dvec, Real dMin, Real dMax, const Vec_UInt16& compressedData);
template void decompress<3, Real>(Vector<MatXxX<3, Real>>& dvec, Real dMin, Real dMax, const Vec_UInt16& compressedData);

template void decompress<2, Real>(Vector<MatXxX<2, Real>>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/);
template void decompress<3, Real>(Vector<MatXxX<3, Real>>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/);
////////////////////////////////////////////////////////////////////////////////
template void decompress<Real>(Vector<Real>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/);
template void decompress<Int>(Vector<Int>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/);
template void decompress<UInt>(Vector<UInt>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/);
template void decompress<Int16>(Vector<Int16>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/);
template void decompress<UInt16>(Vector<UInt16>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/);

template void decompress<Real>(Vector<Real>& dvec, Real dMin, Real dMax, const Vec_UInt16& compressedData);
template void decompress<Real>(Vector<Vector<Real>>& dvec, const Vector<Real> dMin, const Vector<Real>& dMax, const Vec_VecUInt16& compressedData);

template void decompress<Real>(Vector<Vector<Real>>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/);
template void decompress<Int>(Vector<Vector<Int>>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/);
template void decompress<Int16>(Vector<Vector<Int16>>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/);
template void decompress<UInt>(Vector<Vector<UInt>>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/);
template void decompress<UInt16>(Vector<Vector<UInt16>>& dvec, const DataBuffer& buffer, UInt nParticles /*= 0*/);
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleHelpers
