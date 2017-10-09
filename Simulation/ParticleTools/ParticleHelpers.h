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

#pragma once

#include <Banana/Setup.h>
#include <Banana/Data/DataIO.h>
#include <Banana/Utils/FileHelpers.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <ParticleTools/BlueNoiseRelaxation/LloydRelaxation.h>
#include <ParticleTools/BlueNoiseRelaxation/SPHBasedRelaxation.h>

#include <string>
#include <cmath>
#include <vector>
#include <cassert>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// template functions are defined in a hpp file
template<Int N, class RealType> UInt loadBinary(const String& fileName, Vector<VecX<N, RealType> >& particles, RealType particleRadius);
template<Int N, class RealType> void saveBinary(const String& fileName, Vector<VecX<N, RealType> >& particles, RealType particleRadius);

template<Int N, class RealType> void compress(const Vector<VecX<N, RealType> >& positions, VecX<N, RealType>& bMin, VecX<N, RealType>& bMax, Vec_UInt16& compressedData);
template<Int N, class RealType> void compress(const Vector<VecX<N, RealType> >& dvec, DataBuffer& buffer, bool bWriteVectorSize = true);
template<class RealType> void        compress(const Vector<RealType>& dvec, RealType& dMin, RealType& dMax, Vec_UInt16& compressedData);
template<class RealType> void        compress(const Vector<RealType>& dvec, DataBuffer& buffer, bool bWriteVectorSize = true);
template<class RealType> void        compress(const Vector<Vector<RealType> >& dvec, RealType& dMin, RealType& dMax, Vec_VecUInt16& compressedData);
template<class RealType> void        compress(const Vector<Vector<RealType> >& dvec, DataBuffer& buffer, bool bWriteVectorSize = true);
template<Int N, class RealType> void compressAndSaveBinary(const String& fileName, const Vector<VecX<N, RealType> >& positions, RealType particleRadius);
template<class RealType> void        compressAndSaveBinary(const String& fileName, const Vector<RealType>& dvec);

template<Int N, class RealType> void decompress(Vector<VecX<N, RealType> >& positions, const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax, const Vec_UInt16& compressedData);
template<Int N, class RealType> void decompress(Vector<VecX<N, RealType> >& dvec, const DataBuffer& buffer, UInt nParticles = 0);
template<class RealType> void        decompress(Vector<RealType>& dvec, RealType dMin, RealType dMax, const Vec_UInt16& compressedData);
template<class RealType> void        decompress(Vector<RealType>& dvec, const DataBuffer& buffer, UInt nParticles = 0);
template<class RealType> void        decompress(Vector<Vector<RealType> >& dvec, RealType dMin, RealType dMax, const Vec_VecUInt16& compressedData);
template<class RealType> void        decompress(Vector<Vector<RealType> >& dvec, const DataBuffer& buffer, UInt nParticles = 0);
template<class RealType> UInt        loadBinaryAndDecompress(const String& fileName, Vector<RealType>& dvec);
template<Int N, class RealType> UInt loadBinaryAndDecompress(const String& fileName, Vector<VecX<N, RealType> >& positions, RealType& particleRadius);


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// non-template functions must be defined in separate .cpp file
void transform(Vec_Vec3r& particles, const Vec3r& translation, const Vec3r& rotation);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleTools/ParticleHelpers.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleHelpers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
