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

#pragma once

#include <Banana/Setup.h>
#include <Banana/Array/Array.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/ParallelHelpers/Scheduler.h>
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
    void setGrid(const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax, RealType cellSize);
    void setCellSize(RealType cellSize);

    ////////////////////////////////////////////////////////////////////////////////
    // Getters
    const auto& getBMin() const noexcept { return m_BMin; }
    const auto& getBMax() const noexcept { return m_BMax; }

    const auto& getNCells() const noexcept { return m_NCells; }
    const auto& getNNodes() const noexcept { return m_NNodes; }
    auto        getNTotalCells() const noexcept { return m_NTotalCells; }
    auto        getNTotalNodes() const noexcept { return m_NTotalNodes; }

    ////////////////////////////////////////////////////////////////////////////////
    auto getCellSize() const noexcept { return m_CellSize; }
    auto getInvCellSize() const noexcept { return m_InvCellSize; }
    auto getHalfCellSize() const noexcept { return m_HalfCellSize; }
    auto getCellSizeSquared() const noexcept { return m_CellSizeSqr; }

    ////////////////////////////////////////////////////////////////////////////////
    // Grid 2D =>
    template<class IndexType>
    IndexType getCellLinearizedIndex(IndexType i, IndexType j) const
    {
        static_assert(N == 2, "Array dimension != 2");
        return j * static_cast<IndexType>(getNCells()[0]) + i;
    }

    template<class IndexType>
    IndexType getNodeLinearizedIndex(IndexType i, IndexType j) const
    {
        static_assert(N == 2, "Array dimension != 2");
        return j * static_cast<IndexType>(getNNodes()[0]()) + i;
    }

    template<class IndexType>
    bool isValidCell(IndexType i, IndexType j)  const noexcept
    {
        static_assert(N == 2, "Array dimension != 2");
        return (i >= 0 &&
                j >= 0 &&
                static_cast<UInt>(i) < m_NCells[0] &&
                static_cast<UInt>(j) < m_NCells[1]);
    }

    template<class IndexType>
    bool isValidCell(const Vec2<IndexType>& index) const noexcept
    {
        return isValidCell(index[0], index[1]);
    }

    template<class IndexType>
    bool isValidNode(IndexType i, IndexType j)  const noexcept
    {
        static_assert(N == 2, "Array dimension != 2");
        return (i >= 0 &&
                j >= 0 &&
                static_cast<UInt>(i) < m_NNodes[0] &&
                static_cast<UInt>(j) < m_NNodes[1]);
    }

    template<class IndexType>
    bool isValidNode(const Vec2<IndexType>& index) const noexcept
    {
        return isValidNode(index[0], index[1]);
    }

    // => Grid 2D
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // Grid 3D =>
    template<class IndexType>
    IndexType getCellLinearizedIndex(IndexType i, IndexType j, IndexType k) const
    {
        static_assert(N == 3, "Array dimension != 3");
        return (k * static_cast<IndexType>(getNCells()[1]) + j) * static_cast<IndexType>(getNCells()[0]) + i;
    }

    template<class IndexType>
    IndexType getNodeLinearizedIndex(IndexType i, IndexType j, IndexType k) const
    {
        static_assert(N == 3, "Array dimension != 3");
        return (k * static_cast<IndexType>(getNNodes()[1]()) + j) * static_cast<IndexType>(getNNodes()[0]) + i;
    }

    template<class IndexType>
    bool isValidCell(IndexType i, IndexType j, IndexType k)  const noexcept
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
    bool isValidCell(const Vec3<IndexType>& index) const noexcept
    {
        return isValidCell(index[0], index[1], index[2]);
    }

    template<class IndexType>
    bool isValidNode(IndexType i, IndexType j, IndexType k)  const noexcept
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
    bool isValidNode(const Vec3<IndexType>& index) const noexcept
    {
        return isValidNode(index[0], index[1], index[2]);
    }

    // => Grid 3D
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    template<class IndexType>
    auto getCellIdx(const VecX<N, RealType>& ppos) const noexcept
    {
        VecX<N, IndexType> cellIdx;
        for(Int d = 0; d < N; ++d) {
            cellIdx[d] = static_cast<IndexType>((ppos[d] - m_BMin[d]) / m_CellSize);
        }
        return cellIdx;
    }

    template<class IndexType>
    auto getNearestValidCellIdx(const VecX<N, IndexType>& cellIdx) const noexcept
    {
        VecX<N, IndexType> validCellIdx;
        for(Int d = 0; d < N; ++d) {
            validCellIdx[d] = MathHelpers::clamp<IndexType>(cellIdx[d], 0, static_cast<IndexType>(m_NCells[d]) - 1);
        }
        return validCellIdx;
    }

    template<class IndexType>
    auto getValidCellIdx(const VecX<N, RealType>& ppos) const noexcept
    {
        return getNearestValidCellIdx<IndexType>(getCellIdx<IndexType>(ppos));
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Particle processing
    template<class IndexType>
    auto getWorldCoordinate(const VecX<N, IndexType>& nodeIdx) const
    {
        return VecX<N, RealType>(nodeIdx) * m_CellSize + m_BMin;
    }

    template<class IndexType>
    auto getWorldCoordinate(IndexType i, IndexType j) const
    {
        static_assert(N == 2, "Array dimension != 2");
        return Vec2<RealType>(i, j) * m_CellSize + m_BMin;
    }

    template<class IndexType>
    auto getWorldCoordinate(IndexType i, IndexType j, IndexType k) const
    {
        static_assert(N == 3, "Array dimension != 3");
        return Vec3<RealType>(i, j, k) * m_CellSize + m_BMin;
    }

    auto getGridCoordinate(const VecX<N, RealType>& ppos) const { return (ppos - m_BMin) / m_CellSize; }
    ////////////////////////////////////////////////////////////////////////////////
    bool isInsideGrid(const VecX<N, RealType>& ppos) const noexcept;

    void constrainToGrid(Vector<VecX<N, RealType>>& positions);
    void collectIndexToCells(const Vector<VecX<N, RealType>>& positions);
    void collectIndexToCells(const Vector<VecX<N, RealType>>& positions, Vector<VecX<N, Int>>& particleCellIdx);
    void collectIndexToCells(const Vector<VecX<N, RealType>>& positions, Vector<VecX<N, RealType>>& particleCellPos);
    void getNeighborList(const Vector<VecX<N, RealType>>& positions, Vec_VecUInt& neighborList, Int cellSpan = 1);
    void getNeighborList(const Vec2<RealType>& ppos, Vec_UInt& neighborList, Int cellSpan = 1);
    void getNeighborList(const Vec3<RealType>& ppos, Vec_UInt& neighborList, Int cellSpan = 1);
    void getNeighborList(const Vector<VecX<N, RealType>>& positions, Vec_VecUInt& neighborList, RealType d2, Int cellSpan = 1);
    void getNeighborList(const Vec_Vec2<RealType>& positions, const Vec2<RealType>& ppos, Vec_UInt& neighborList, RealType d2, Int cellSpan = 1);
    void getNeighborList(const Vec_Vec3<RealType>& positions, const Vec3<RealType>& ppos, Vec_UInt& neighborList, RealType d2, Int cellSpan = 1);
    void sortData(Vector<VecX<N, RealType>>& data);

    const Vec_UInt& getParticleIdxSortedByCell();

    template<class IndexType>
    const auto& getParticleIdxInCell(const VecX<N, IndexType>& cellIdx) const { return m_ParticleIdxInCell(cellIdx); }
protected:
    VecX<N, RealType> m_BMin         = VecX<N, RealType>(-1.0);
    VecX<N, RealType> m_BMax         = VecX<N, RealType>(1.0);
    VecX<N, UInt>     m_NCells       = VecX<N, UInt>(0);
    VecX<N, UInt>     m_NNodes       = VecX<N, UInt>(0);
    UInt              m_NTotalCells  = 1u;
    UInt              m_NTotalNodes  = 1u;
    RealType          m_CellSize     = RealType(1);
    RealType          m_InvCellSize  = RealType(1);
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
using Grid2f = Grid<2, float>;
using Grid2d = Grid<2, double>;
using Grid2r = Grid<2, Real>;

using Grid3f = Grid<3, float>;
using Grid3d = Grid<3, double>;
using Grid3r = Grid<3, Real>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana