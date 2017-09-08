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
#include <Banana/Array/Array2.h>
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

    void collectIndexToCells(const Vec_Vec2r& particles);
    void collectIndexToCells(const Vec_Vec2r& particles, Vec_Vec2i& particleCellIdx);
    void getNeighborList(const Vec_Vec2r& particles, Vec_VecUInt& neighborList, int cellSpan = 1);
    void getNeighborList(const Vec2r& ppos, Vec_UInt& neighborList, int cellSpan = 1);
    void getNeighborList(const Vec_Vec2r& particles, Vec_VecUInt& neighborList, Real d2, int cellSpan = 1);
    void getNeighborList(const Vec_Vec2r& particles, const Vec2r& ppos, Vec_UInt& neighborList, Real d2, int cellSpan = 1);

    const Vec_UInt& getParticleIdxSortedByCell();
    void            sortData(Vec_Vec2r& data);

    template<class IndexType>
    const Vec_UInt& getParticleIdxInCell(const Vec2<IndexType>& cellIdx) const { return m_CellParticleIdx(cellIdx); }

private:
    Vec_UInt m_ParticleIdx;
    bool     m_bCellIdxNeedResize = false;            // to track and resize the m_CellParticleIdx array

    Array2_VecUInt                    m_CellParticleIdx;
    Array2<ParallelObjects::SpinLock> m_Lock;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana