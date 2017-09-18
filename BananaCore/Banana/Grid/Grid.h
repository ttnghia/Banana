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
#include <Banana/Array/Array.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>
#include <Banana/ParallelHelpers/ParallelObjects.h>

#include <cassert>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class Grid
{
public:
    Grid() = default;
    Grid(const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax, RealType cellSize) : m_BMin(bMin), m_BMax(bMax) { setCellSize(cellSize); }

    ////////////////////////////////////////////////////////////////////////////////
    // Setters
    __BNN_INLINE void         setGrid(const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax, RealType cellSize);
    __BNN_INLINE virtual void setCellSize(RealType cellSize);

    ////////////////////////////////////////////////////////////////////////////////
    // Getters
    __BNN_INLINE const VecX<N, RealType>& getBMin() const noexcept { return m_BMin; }
    __BNN_INLINE const VecX<N, RealType>& getBMax() const noexcept { return m_BMax; }

    __BNN_INLINE const VecX<N, UInt>& getNCells() const noexcept { return m_NCells; }
    __BNN_INLINE const VecX<N, UInt>& getNNodes() const noexcept { return m_NNodes; }
    __BNN_INLINE UInt                 getNTotalCells() const noexcept { return m_NTotalCells; }
    __BNN_INLINE UInt                 getNTotalNodes() const noexcept { return m_NTotalNodes; }

    ////////////////////////////////////////////////////////////////////////////////
    __BNN_INLINE RealType getCellSize() const noexcept { return m_CellSize; }
    __BNN_INLINE RealType getHalfCellSize() const noexcept { return m_HalfCellSize; }
    __BNN_INLINE RealType getCellSizeSquared() const noexcept { return m_CellSizeSqr; }

    ////////////////////////////////////////////////////////////////////////////////
    // Grid 2D =>
    template<class IndexType>
    __BNN_INLINE IndexType getCellLinearizedIndex(IndexType i, IndexType j) const
    {
        static_assert(N == 2, "Array dimension != 2");
        return j * static_cast<IndexType>(getNCells()[0]) + i;
    }

    template<class IndexType>
    __BNN_INLINE IndexType getNodeLinearizedIndex(IndexType i, IndexType j) const
    {
        static_assert(N == 2, "Array dimension != 2");
        return j * static_cast<IndexType>(getNumNodeX()) + i;
    }

    template<class IndexType>
    __BNN_INLINE bool isValidCell(IndexType i, IndexType j)  const noexcept
    {
        static_assert(N == 2, "Array dimension != 2");
        return (i >= 0 &&
                j >= 0 &&
                static_cast<UInt>(i) < m_NCells[0] &&
                static_cast<UInt>(j) < m_NCells[1]);
    }

    template<class IndexType>
    __BNN_INLINE bool isValidCell(const Vec2<IndexType>& index) const noexcept
    {
        return isValidCell(index[0], index[1]);
    }

    template<class IndexType>
    __BNN_INLINE bool isValidNode(IndexType i, IndexType j)  const noexcept
    {
        static_assert(N == 2, "Array dimension != 2");
        return (i >= 0 &&
                j >= 0 &&
                static_cast<UInt>(i) < m_NNodes[0] &&
                static_cast<UInt>(j) < m_NNodes[1]);
    }

    template<class IndexType>
    __BNN_INLINE bool isValidNode(const Vec2<IndexType>& index) const noexcept
    {
        return isValidNode(index[0], index[1]);
    }

    // => Grid 2D
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // Grid 3D =>
    template<class IndexType>
    __BNN_INLINE IndexType getCellLinearizedIndex(IndexType i, IndexType j, IndexType k) const
    {
        static_assert(N == 3, "Array dimension != 3");
        return (k * static_cast<IndexType>(getNCells()[1]) + j) * static_cast<IndexType>(getNCells()[0]) + i;
    }

    template<class IndexType>
    __BNN_INLINE IndexType getNodeLinearizedIndex(IndexType i, IndexType j, IndexType k) const
    {
        static_assert(N == 3, "Array dimension != 3");
        return (k * static_cast<IndexType>(getNNodes()[1]()) + j) * static_cast<IndexType>(getNNodes()[0]) + i;
    }

    template<class IndexType>
    __BNN_INLINE bool isValidCell(IndexType i, IndexType j, IndexType k)  const noexcept
    {
        static_assert(N == 3, "Array dimension != 3");
        return (i >= 0 &&
                j >= 0 &&
                k >= 0 &&
                static_cast<UInt>(i) < m_NCells[0] &&
                static_cast<UInt>(j) < m_NCells[1] &&
                static_cast<UInt>(k) < m_NCells[2]);
    }

    template<class IndexType>
    __BNN_INLINE bool isValidCell(const Vec3<IndexType>& index) const noexcept
    {
        return isValidCell(index[0], index[1], index[2]);
    }

    template<class IndexType>
    __BNN_INLINE bool isValidNode(IndexType i, IndexType j, IndexType k)  const noexcept
    {
        static_assert(N == 3, "Array dimension != 3");
        return (i >= 0 &&
                j >= 0 &&
                k >= 0 &&
                static_cast<UInt>(i) < m_NNodes[0] &&
                static_cast<UInt>(j) < m_NNodes[1] &&
                static_cast<UInt>(k) < m_NNodes[2]);
    }

    template<class IndexType>
    __BNN_INLINE bool isValidNode(const Vec3<IndexType>& index) const noexcept
    {
        return isValidNode(index[0], index[1], index[2]);
    }

    // => Grid 3D
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    template<class IndexType>
    __BNN_INLINE VecX<N, IndexType> getCellIdx(const VecX<N, RealType>& ppos) const noexcept
    {
        VecX<N, IndexType> cellIdx;
        for(Int i = 0; i < N; ++i)
            cellIdx[i] = static_cast<IndexType>((ppos[i] - m_BMin[i]) / m_CellSize);
        return cellIdx;
    }

    template<class IndexType>
    __BNN_INLINE VecX<N, IndexType> getNearestValidCellIdx(const VecX<N, IndexType>& cellIdx) const noexcept
    {
        VecX<N, IndexType> validCellIdx;

        for(Int i = 0; i < N; ++i)
            validCellIdx[i] = MathHelpers::clamp<IndexType>(cellIdx[i], 0, static_cast<IndexType>(m_NCells[i]) - 1);

        return validCellIdx;
    }

    template<class IndexType>
    __BNN_INLINE VecX<N, IndexType> getValidCellIdx(const VecX<N, RealType>& ppos) const noexcept
    {
        return getNearestValidCellIdx<IndexType>(getCellIdx<IndexType>(ppos));
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Particle processing
    template<class IndexType>
    __BNN_INLINE VecX<N, RealType> getWorldCoordinate(const VecX<N, IndexType>& cellIdx) const
    {
        return NumberHelpers::convert<RealType>(cellIdx) * m_CellSize + m_BMin;
    }

    template<class IndexType>
    __BNN_INLINE Vec2<RealType> getWorldCoordinate(IndexType i, IndexType j) const
    {
        static_assert(N == 2, "Array dimension != 2");
        return Vec2<RealType>(i, j) * m_CellSize + m_BMin;
    }

    template<class IndexType>
    __BNN_INLINE Vec3<RealType> getWorldCoordinate(IndexType i, IndexType j, IndexType k) const
    {
        static_assert(N == 3, "Array dimension != 3");
        return Vec3<RealType>(i, j, k) * m_CellSize + m_BMin;
    }

    __BNN_INLINE VecX<N, RealType> getGridCoordinate(const VecX<N, RealType>& ppos) const { return (ppos - m_BMin) / m_CellSize; }

    __BNN_INLINE bool isInsideGrid(const VecX<N, RealType>& ppos) const noexcept;

    void constrainToGrid(Vector<VecX<N, RealType> >& positions);
    void collectIndexToCells(const Vector<VecX<N, RealType> >& positions);
    void collectIndexToCells(const Vector<VecX<N, RealType> >& positions, Vector<VecX<N, Int> >& particleCellIdx);
    void collectIndexToCells(const Vector<VecX<N, RealType> >& positions, Vector<VecX<N, RealType> >& particleCellPos);
    void getNeighborList(const Vector<VecX<N, RealType> >& positions, Vec_VecUInt& neighborList, Int cellSpan = 1);
    void getNeighborList(const Vec2<RealType>& ppos, Vec_UInt& neighborList, Int cellSpan = 1);
    void getNeighborList(const Vec3<RealType>& ppos, Vec_UInt& neighborList, Int cellSpan = 1);
    void getNeighborList(const Vector<VecX<N, RealType> >& positions, Vec_VecUInt& neighborList, RealType d2, Int cellSpan = 1);
    void getNeighborList(const Vec_Vec2<RealType>& positions, const Vec2<RealType>& ppos, Vec_UInt& neighborList, RealType d2, Int cellSpan = 1);
    void getNeighborList(const Vec_Vec3<RealType>& positions, const Vec3<RealType>& ppos, Vec_UInt& neighborList, RealType d2, Int cellSpan = 1);
    void sortData(Vector<VecX<N, RealType> >& data);

    const Vec_UInt& getParticleIdxSortedByCell();

    template<class IndexType>
    const Vec_UInt& getParticleIdxInCell(const VecX<N, IndexType>& cellIdx) const { return m_ParticleIdxInCell(cellIdx); }
protected:
    VecX<N, RealType> m_BMin         = VecX<N, RealType>(-1.0);
    VecX<N, RealType> m_BMax         = VecX<N, RealType>(1.0);
    VecX<N, UInt>     m_NCells       = VecX<N, UInt>(0);
    VecX<N, UInt>     m_NNodes       = VecX<N, UInt>(0);
    UInt              m_NTotalCells  = 1;
    UInt              m_NTotalNodes  = 1;
    RealType          m_CellSize     = RealType(1);
    RealType          m_HalfCellSize = RealType(0.5);
    RealType          m_CellSizeSqr  = RealType(1);

    Vec_UInt m_ParticleIdxSortedByCell;
    bool     m_bCellIdxNeedResize = false;  // to track and resize the m_CellParticleIdx array

    Array<N, Vec_UInt>                  m_ParticleIdxInCell;
    Array<N, ParallelObjects::SpinLock> m_Lock;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/Grid/Grid.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
using Grid2f        = Grid<2, float>;
using Grid2d        = Grid<2, double>;
using Grid2r        = Grid<2, Real>;
using Grid2SpinLock = Grid<2, ParallelObjects::SpinLock>;

using Grid3f        = Grid<3, float>;
using Grid3d        = Grid<3, double>;
using Grid3r        = Grid<3, Real>;
using Grid3SpinLock = Grid<3, ParallelObjects::SpinLock>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana