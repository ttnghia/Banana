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

#include <Banana/Grid/Grid3D.h>
#include <Banana/Array/Array.h>
#include <Banana/ParallelHelpers/ParallelObjects.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Grid3DHashing : public Grid3D
{
public:
    Grid3DHashing() = default;
    Grid3DHashing(const Vec3r& bMin, const Vec3r& bMax, Real cellSize) : Grid3D(bMin, bMax, cellSize), m_bCellIdxNeedResize(true) {}

    virtual void setCellSize(Real cellSize) override;

    void collectIndexToCells(const Vec_Vec3r& positions);
    void collectIndexToCells(const Vec_Vec3r& positions, Vec_Vec3i& particleCellIdx);
    void collectIndexToCells(const Vec_Vec3r& positions, Vec_Vec3r& particleCellPos);
    void getNeighborList(const Vec_Vec3r& positions, Vec_VecUInt& neighborList, int cellSpan = 1);
    void getNeighborList(const Vec3r& ppos, Vec_UInt& neighborList, int cellSpan = 1);
    void getNeighborList(const Vec_Vec3r& positions, Vec_VecUInt& neighborList, Real d2, int cellSpan = 1);
    void getNeighborList(const Vec_Vec3r& positions, const Vec3r& ppos, Vec_UInt& neighborList, Real d2, int cellSpan = 1);

    const Vec_UInt& getParticleIdxSortedByCell();
    void            sortData(Vec_Vec3r& data);

    template<class IndexType>
    const Vec_UInt& getParticleIdxInCell(const Vec3<IndexType>& cellIdx) const { return m_ParticleIdxInCell(cellIdx); }

private:
    Vec_UInt m_ParticleIdxSortedByCell;
    bool     m_bCellIdxNeedResize = false;                          // to track and resize the m_CellParticleIdx array

    Array3_VecUInt m_ParticleIdxInCell;
    Array3SpinLock m_Lock;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana