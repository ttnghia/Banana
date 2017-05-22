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

#include <array>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ArrayType, class ScalarType, int GridDimension>
class Grid
{
public:
    Grid() = default;
    Grid(const ArrayType& bMin, const ArrayType& bMax, ScalarType cellSize) : m_BMin(bMin), m_BMax(bMax)
    {
        setCellSize(cellSize);
    }

    void setGrid(const std::array<ScalarType, GridDimension>& bMin, const std::array<ScalarType, GridDimension>& bMax, ScalarType cellSize);

    unsigned int getNumCellX() const noexcept { return m_NumCells[0]; }
    unsigned int getNumCellY() const noexcept { return m_NumCells[1]; }
    unsigned int getNumCellZ() const noexcept { return m_NumCells[2]; }
    unsigned int getNumTotalCells() const noexcept { return m_NumTotalCells; }

    template<class IndexType>
    unsigned int getNumCells(IndexType dimention) const noexcept { return m_NumCells[dimention]; }

    ////////////////////////////////////////////////////////////////////////////////
    void setCellSize(ScalarType cellSize);
    void getCellSize() const noexcept { return m_CellSize; }

    ////////////////////////////////////////////////////////////////////////////////
    template<class IndexType>
    bool isValidCell(IndexType i, IndexType j)  const noexcept;

    template<class IndexType>
    bool isValidCell(IndexType i, IndexType j, IndexType k)  const noexcept;

    template<class VectorType>
    bool isValidCell(const VectorType& index) const noexcept;

private:
    ArrayType    m_BMin;
    ArrayType    m_BMax;
    unsigned int m_NumCells[GridDimension];
    unsigned int m_NumTotalCells = 0;
    ScalarType   m_CellSize      = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Grid/Grid_Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ArrayType, class ScalarType>
using Grid2D = Grid<ArrayType, ScalarType, 2>;

template<class ArrayType, class ScalarType>
using Grid3D = Grid<ArrayType, ScalarType, 3>;
