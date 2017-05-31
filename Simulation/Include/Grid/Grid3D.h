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

#include <limits>
#include <array>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ArrayType, class ScalarType>
class Grid3D
{
public:
    Grid3D() = default;
    Grid3D(const ArrayType& bMin, const ArrayType& bMax, ScalarType cellSize) : m_BMin(bMin), m_BMax(bMax)
    {
        setCellSize(cellSize);
    }

    void setGrid(const ArrayType& bMin, const ArrayType& bMax, ScalarType cellSize);

    unsigned int getNumCellX() const noexcept { return m_NumCells[0]; }
    unsigned int getNumCellY() const noexcept { return m_NumCells[1]; }
    unsigned int getNumCellZ() const noexcept { return m_NumCells[2]; }
    unsigned int getNumCells() const noexcept { return m_NumTotalCells; }
    unsigned int getNumTotalCells() const noexcept { return m_NumTotalCells; }

    template<class IndexType>
    unsigned int getNumCells(IndexType dimention) const noexcept { return m_NumCells[dimention]; }

    ////////////////////////////////////////////////////////////////////////////////
    void setCellSize(ScalarType cellSize);
    void getCellSize() const noexcept { return m_CellSize; }

    ////////////////////////////////////////////////////////////////////////////////
    template<class IndexType>
    bool isValidCell(IndexType i, IndexType j, IndexType k)  const noexcept;

    template<class VectorType>
    bool isValidCell(const VectorType& index) const noexcept;

private:
    ArrayType    m_BMin;
    ArrayType    m_BMax;
    unsigned int m_NumCells[3];
    unsigned int m_NumTotalCells = 0;
    ScalarType   m_CellSize      = std::numeric_limits<ScalarType>::max();;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Grid/Grid3D_Impl.hpp>