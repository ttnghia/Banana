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

#include <Banana/Grid/Grid2D.h>
#include <Banana/Array/Array.h>
#include <Banana/ParallelHelpers/ParallelObjects.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Grid2DHashing : public Grid2D
{
public:
    Grid2DHashing() = default;
    Grid2DHashing(const Vec2r& bMin, const Vec2r& bMax, Real cellSize) : Grid2D(bMin, bMax, cellSize), m_bCellIdxNeedResize(true) {}

    virtual void setCellSize(Real cellSize) override;

    void collectIndexToCells(const Vec_Vec2r& positions);
    void collectIndexToCells(const Vec_Vec2r& positions, Vec_Vec2i& particleCellIdx);
    void collectIndexToCells(const Vec_Vec2r& positions, Vec_Vec2r& particleCellPos);
    void getNeighborList(const Vec_Vec2r& positions, Vec_VecUInt& neighborList, int cellSpan = 1);
    void getNeighborList(const Vec2r& ppos, Vec_UInt& neighborList, int cellSpan = 1);
    void getNeighborList(const Vec_Vec2r& positions, Vec_VecUInt& neighborList, Real d2, int cellSpan = 1);
    void getNeighborList(const Vec_Vec2r& positions, const Vec2r& ppos, Vec_UInt& neighborList, Real d2, int cellSpan = 1);

    const Vec_UInt& getParticleIdxSortedByCell();
    void            sortData(Vec_Vec2r& data);

    template<class IndexType>
    const Vec_UInt& getParticleIdxInCell(const Vec2<IndexType>& cellIdx) const { return m_ParticleIdxInCell(cellIdx); }

private:
    Vec_UInt m_ParticleIdxSortedByCell;
    bool     m_bCellIdxNeedResize = false;            // to track and resize the m_CellParticleIdx array

    Array2_VecUInt m_ParticleIdxInCell;
    Array2SpinLock m_Lock;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana