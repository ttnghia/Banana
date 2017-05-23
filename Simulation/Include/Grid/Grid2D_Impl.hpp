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
template<class ArrayType, class ScalarType>
void Grid2D<ArrayType, ScalarType>::setGrid(const ArrayType& bMin, const ArrayType& bMax, ScalarType cellSize)
{
    m_BMin = bMin;
    m_BMax = bMax;
    setCellSize(cellSize)
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ArrayType, class ScalarType>
void Grid2D<ArrayType, ScalarType>::setCellSize(ScalarType cellSize)
{
    assert(cellSize > 0);
    m_CellSize = cellSize;
    m_NumTotalCells = 1;

    for(int i = 0; i < 2; ++i)
    {
        m_NumCells[i] = static_cast<unsigned int>(ceil((m_BMax[i] - m_BMin[i]) / m_CellSize));
        m_NumTotalCells *= m_NumCells[i];
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ArrayType, class ScalarType>
template<class IndexType>
bool Grid2D<ArrayType, ScalarType>::isValidCell(IndexType i, IndexType j)  const noexcept
{
    return (i >= 0 &&
            j >= 0 &&
            static_cast<unsigned int>(i) < m_NumCells[0] &&
            static_cast<unsigned int>(j) < m_NumCells[1]);
}

template<class ArrayType, class ScalarType>
template<class VectorType>
bool Grid2D<ArrayType, ScalarType>::isValidCell(const VectorType& index)  const noexcept
{
    return isValidCell(index[0], index[1]);
}
