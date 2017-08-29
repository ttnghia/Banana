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
#include <Banana/ParallelHelpers/ParallelFuncs.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Grid3D
{
public:
    Grid3D() = default;
    Grid3D(const Vec3r& bMin, const Vec3r& bMax, Real cellSize) : m_BMin(bMin), m_BMax(bMax)
    {
        setCellSize(cellSize);
    }

    void setGrid(const Vec3r& bMin, const Vec3r& bMax, Real cellSize);

    const Vec3r& getBMin() const noexcept { return m_BMin; }
    const Vec3r& getBMax() const noexcept { return m_BMax; }
    UInt getNumCellX() const noexcept { return m_NumCells[0]; }
    UInt getNumCellY() const noexcept { return m_NumCells[1]; }
    UInt getNumCellZ() const noexcept { return m_NumCells[2]; }
    UInt getNumTotalCells() const noexcept { return m_NumTotalCells; }
    const Vec3ui& getNumCells() const noexcept { return m_NumCells; }

    template<class IndexType>
    IndexType getLinearizedIndex(IndexType i, IndexType j, IndexType k) const
    {
        return (k * static_cast<IndexType>(getNumCellY()) + j) * static_cast<IndexType>(getNumCellX()) + i;
    }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void setCellSize(Real cellSize);
    Real getCellSize() const noexcept { return m_CellSize; }
    Real getHalfCellSize() const noexcept { return m_HalfCellSize; }
    Real getCellSizeSquared() const noexcept { return m_CellSizeSqr; }

    ////////////////////////////////////////////////////////////////////////////////
    bool isInsideGrid(const Vec3r& ppos) const noexcept;

    template<class IndexType>
    bool isValidCell(IndexType i, IndexType j, IndexType k)  const noexcept;

    template<class IndexType>
    bool isValidCell(const Vec3<IndexType>& index) const noexcept;

    ////////////////////////////////////////////////////////////////////////////////
    Vec3r getGridCoordinate(const Vec3r& ppos) const { return (ppos - m_BMin) / m_CellSize; }

    template<class IndexType>
    Vec3<IndexType> getCellIdx(const Vec3r& ppos) const noexcept;

    template<class IndexType>
    Vec3<IndexType> getValidCellIdx(const Vec3r& ppos) const noexcept;

    template<class IndexType>
    Vec3<IndexType> getNearestValidCellIdx(const Vec3<IndexType>& cellIdx) const noexcept;

    ////////////////////////////////////////////////////////////////////////////////
    // particle processing
    void constraintToGrid(Vec_Vec3r& particles);

protected:
    Vec3r  m_BMin          = Vec3r(0);
    Vec3r  m_BMax          = Vec3r(1);
    Vec3ui m_NumCells      = Vec3<UInt>(0);
    UInt   m_NumTotalCells = 1;
    Real   m_CellSize      = Real(1.0);
    Real   m_HalfCellSize  = Real(0.5);
    Real   m_CellSizeSqr   = Real(1.0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana