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

#include <Banana/TypeNames.h>
#include <Banana/Array/Array3.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>

#include <limits>
#include <array>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class Grid3D
{
public:
    Grid3D() = default;
    Grid3D(const Vec3<RealType>& bMin, const Vec3<RealType>& bMax, RealType cellSize) : m_BMin(bMin), m_BMax(bMax)
    {
        setCellSize(cellSize);
    }

    void setGrid(const Vec3<RealType>& bMin, const Vec3<RealType>& bMax, RealType cellSize);

    unsigned int getNumCellX() const noexcept { return m_NumCells[0]; }
    unsigned int getNumCellY() const noexcept { return m_NumCells[1]; }
    unsigned int getNumCellZ() const noexcept { return m_NumCells[2]; }
    unsigned int getNumTotalCells() const noexcept { return m_NumTotalCells; }
    Vec3<unsigned int> getNumCells() const noexcept { return m_NumCells; }

    ////////////////////////////////////////////////////////////////////////////////
    void setCellSize(RealType cellSize);
    RealType getCellSize() const noexcept { return m_CellSize; }

    ////////////////////////////////////////////////////////////////////////////////
    template<class IndexType>
    bool isValidCell(IndexType i, IndexType j, IndexType k)  const noexcept;

    template<class IndexType>
    bool isValidCell(const Vec3<IndexType>& index) const noexcept;

    ////////////////////////////////////////////////////////////////////////////////
    template<class IndexType>
    Vec3<IndexType> getCellIdx(const Vec3<RealType>& ppos) const noexcept;

    template<class IndexType>
    Vec3<IndexType> getValidCellIdx(const Vec3<RealType>& ppos) const noexcept;

    template<class IndexType>
    Vec3<IndexType> getNearestValidCellIdx(const Vec3<IndexType>& cellIdx) const noexcept;

    ////////////////////////////////////////////////////////////////////////////////
    // particle processing
    void            constraintToGrid(Vec_Vec3<RealType>& particles);
    void            collectIndexToCells(Vec_Vec3<RealType>& particles);
    void            findNeighborList(const Vec_Vec3<RealType>& particles, Vec_VecUInt& neighborList, int cellSpan = 1);
    void            findNeighborList(const Vec3<RealType>& ppos, Vec_UInt& neighborList, int cellSpan = 1);
    const Vec_UInt& getParticleIdxSortedByCell();

private:
    Vec3<RealType>     m_BMin          = Vec3<RealType>(0);
    Vec3<RealType>     m_BMax          = Vec3<RealType>(1);
    Vec3<unsigned int> m_NumCells      = Vec3<unsigned int>(0);
    unsigned int       m_NumTotalCells = 1;
    RealType           m_CellSize      = RealType(1.0);
    Array3_VecUInt     m_CellParticleIdx;
    bool               m_bCellIdxNeedResize = false;  // to track and resize the m_CellParticleIdx array
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Grid/Grid3D.Impl.hpp>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana