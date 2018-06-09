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

#pragma once

#include <Banana/Setup.h>
#include <Banana/Data/DataIO.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// template functions are defined in a hpp file

template<Int N, class RealType> auto getAABB(const Vec_VecX<N, RealType>& positions);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType> void compress(const Vec_VecX<N, RealType>& dvec, VecX<N, RealType>& dMin, VecX<N, RealType>& dMax, Vec_UInt16& compressedData);
template<Int N, class RealType> void compress(const Vec_VecX<N, RealType>& dvec, DataBuffer& buffer, bool bWriteVectorSize = true);
template<Int N, class RealType> void compress(const Vector<MatXxX<N, RealType>>& dvec, RealType& dMin, RealType& dMax, Vec_UInt16& compressedData);
template<Int N, class RealType> void compress(const Vector<MatXxX<N, RealType>>& dvec, DataBuffer& buffer, bool bWriteVectorSize = true);

template<class RealType> void compress(const Vector<RealType>& dvec, RealType& dMin, RealType& dMax, Vec_UInt16& compressedData);
template<class RealType> void compress(const Vector<RealType>& dvec, DataBuffer& buffer, bool bWriteVectorSize = true);
template<class RealType> void compress(const Vector<Vector<RealType>>& dvec, Vector<RealType>& dMin, Vector<RealType>& dMax, Vec_VecUInt16& compressedData);
template<class RealType> void compress(const Vector<Vector<RealType>>& dvec, DataBuffer& buffer, bool bWriteVectorSize = true);

template<Int N, class RealType> void decompress(Vec_VecX<N, RealType>& dvec, const VecX<N, RealType>& dMin, const VecX<N, RealType>& dMax, const Vec_UInt16& compressedData);
template<Int N, class RealType> void decompress(Vec_VecX<N, RealType>& dvec, const DataBuffer& buffer, UInt nParticles = 0);
template<Int N, class RealType> void decompress(Vector<MatXxX<N, RealType>>& dvec, RealType dMin, RealType dMax, const Vec_UInt16& compressedData);
template<Int N, class RealType> void decompress(Vector<MatXxX<N, RealType>>& dvec, const DataBuffer& buffer, UInt nParticles = 0);

template<class RealType> void decompress(Vector<RealType>& dvec, RealType dMin, RealType dMax, const Vec_UInt16& compressedData);
template<class RealType> void decompress(Vector<RealType>& dvec, const DataBuffer& buffer, UInt nParticles = 0);
template<class RealType> void decompress(Vector<Vector<RealType>>& dvec, const Vector<RealType> dMin, const Vector<RealType>& dMax, const Vec_VecUInt16& compressedData);
template<class RealType> void decompress(Vector<Vector<RealType>>& dvec, const DataBuffer& buffer, UInt nParticles = 0);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType> bool loadParticlesFromObj(const String& fileName, Vec_VecX<N, RealType>& positions);
template<Int N, class RealType> bool loadParticlesFromBGEO(const String& fileName, Vec_VecX<N, RealType>& positions, RealType& particleRadius);
template<Int N, class RealType> bool loadParticlesFromBNN(const String& fileName, Vec_VecX<N, RealType>& positions, RealType& particleRadius);
template<Int N, class RealType> bool loadParticlesFromBinary(const String& fileName, Vec_VecX<N, RealType>& positions, RealType& particleRadius);

template<Int N, class RealType> bool saveParticlesToObj(const String& fileName, const Vec_VecX<N, RealType>& positions);
template<Int N, class RealType> bool saveParticlesToBGEO(const String& fileName, const Vec_VecX<N, RealType>& positions, RealType particleRadius);
template<Int N, class RealType> bool saveParticlesToBNN(const String& fileName, const Vec_VecX<N, RealType>& positions, RealType particleRadius);
template<Int N, class RealType> bool saveParticlesToBinary(const String& fileName, const Vec_VecX<N, RealType>& positions, RealType particleRadius);
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// non-template functions
void connectedComponentAnalysis(const Vec_VecUInt& connectionList, Vec_Int8& componentIdx, UInt& nComponents);
UInt spawnComponent(UInt p, Int depth, UInt8 currentIdx, const Vec_VecUInt& connectionList, Vec_Int8& componentIdx);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleHelpers
