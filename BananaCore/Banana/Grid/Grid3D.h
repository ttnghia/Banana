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

    ////////////////////////////////////////////////////////////////////////////////
    virtual void setCellSize(Real cellSize);
    Real getCellSize() const noexcept { return m_CellSize; }
    Real getHalfCellSize() const noexcept { return m_HalfCellSize; }
    Real getCellSizeSquared() const noexcept { return m_CellSizeSqr; }

    ////////////////////////////////////////////////////////////////////////////////
    template<class IndexType>
    IndexType getLinearizedIndex(IndexType i, IndexType j, IndexType k) const
    {
        return (k * static_cast<IndexType>(getNumCellY()) + j) * static_cast<IndexType>(getNumCellX()) + i;
    }

    template<class IndexType>
    bool isValidCell(IndexType i, IndexType j, IndexType k)  const noexcept
    {
        return (i >= 0 &&
                j >= 0 &&
                k >= 0 &&
                static_cast<UInt>(i) < m_NumCells[0] &&
                static_cast<UInt>(j) < m_NumCells[1] &&
                static_cast<UInt>(k) < m_NumCells[2]);
    }

    template<class IndexType>
    bool isValidCell(const Vec3<IndexType>& index) const noexcept
    {
        return isValidCell(index[0], index[1], index[2]);
    }

    template<class IndexType>
    Vec3<IndexType> getCellIdx(const Vec3r& ppos) const noexcept
    {
        return Vec3<IndexType>(static_cast<IndexType>((ppos[0] - m_BMin[0]) / m_CellSize),
                               static_cast<IndexType>((ppos[1] - m_BMin[1]) / m_CellSize),
                               static_cast<IndexType>((ppos[2] - m_BMin[2]) / m_CellSize));
    }

    template<class IndexType>
    Vec3<IndexType> getValidCellIdx(const Vec3r& ppos) const noexcept
    {
        return getNearestValidCellIdx<IndexType>(getCellIdx<IndexType>(ppos));
    }

    template<class IndexType>
    Vec3<IndexType> getNearestValidCellIdx(const Vec3<IndexType>& cellIdx) const noexcept
    {
        Vec3<IndexType> nearestCellIdx;

        for(int i = 0; i < 3; ++i)
            nearestCellIdx[i] = std::max<IndexType>(0, std::min<IndexType>(cellIdx[i], static_cast<IndexType>(m_NumCells[i]) - 1));

        return nearestCellIdx;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // particle processing
    void constraintToGrid(Vec_Vec3r& particles);
    bool isInsideGrid(const Vec3r& ppos) const noexcept;
    Vec3r getGridCoordinate(const Vec3r& ppos) const { return (ppos - m_BMin) / m_CellSize; }

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