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

#include <limits>
#include <array>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class Grid3D
{
public:
    Grid3D() = default;
    Grid3D(const Vec3<ScalarType>& bMin, const Vec3<ScalarType>& bMax, ScalarType cellSize, bool bEnableCellParticleIdx = true) : m_BMin(bMin), m_BMax(bMax)
    {
        setCellSize(cellSize);
        enableCellParticleIdx(bEnableCellParticleIdx);
    }

    void setGrid(const Vec3<ScalarType>& bMin, const Vec3<ScalarType>& bMax, ScalarType cellSize);

    unsigned int getNumCellX() const noexcept { return m_NumCells[0]; }
    unsigned int getNumCellY() const noexcept { return m_NumCells[1]; }
    unsigned int getNumCellZ() const noexcept { return m_NumCells[2]; }
    unsigned int getNumTotalCells() const noexcept { return m_NumTotalCells; }
    Vec3<unsigned int> getNumCells() const noexcept { return m_NumCells; }

    ////////////////////////////////////////////////////////////////////////////////
    void setCellSize(ScalarType cellSize);
    ScalarType getCellSize() const noexcept { return m_CellSize; }

    ////////////////////////////////////////////////////////////////////////////////
    template<class IndexType>
    bool isValidCell(IndexType i, IndexType j, IndexType k)  const noexcept;

    template<class IndexType>
    bool isValidCell(const Vec3<IndexType>& index) const noexcept;

    ////////////////////////////////////////////////////////////////////////////////
    template<class IndexType>
    Vec3<IndexType> getCellIdx(const Vec3<ScalarType>& position) const noexcept;

    template<class IndexType>
    Vec3<IndexType> getValidCellIdx(const Vec3<ScalarType>& position) const noexcept;

    template<class IndexType>
    Vec3<IndexType> getNearestValidCellIdx(const Vec3<IndexType>& cellIdx) const noexcept;

    ////////////////////////////////////////////////////////////////////////////////
    void enableCellParticleIdx(bool bEnable = true);
    void collectParticlesToCells(Vec_Vec3<ScalarType>& particles);

    void enableSortParticles(bool bEnable = true);
    void sortParticleByCell(Vec_Vec3<ScalarType>& particles);

private:
    Vec3<ScalarType>   m_BMin          = Vec3<ScalarType>(0);
    Vec3<ScalarType>   m_BMax          = Vec3<ScalarType>(1);
    Vec3<unsigned int> m_NumCells      = Vec3<unsigned int>(0);
    unsigned int       m_NumTotalCells = 1;
    ScalarType         m_CellSize      = ScalarType(1.0);

    bool           m_bSortParticles = false;
    unsigned int   m_SortFrequency  = 100;
    Array3_VecUInt m_CellParticleIdx;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Grid/Grid3D_Impl.hpp>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana