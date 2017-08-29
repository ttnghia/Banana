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
#include <Banana/Grid/Grid2D.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Banana::Grid2D::setGrid(const Vec2r& bMin, const Vec2r& bMax, Real cellSize)
{
    m_BMin = bMin;
    m_BMax = bMax;
    setCellSize(cellSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Banana::Grid2D::setCellSize(Real cellSize)
{
    assert(cellSize > 0);
    m_CellSize      = cellSize;
    m_HalfCellSize  = Real(0.5) * m_CellSize;
    m_CellSizeSqr   = m_CellSize * m_CellSize;
    m_NumTotalCells = 1;

    for(int i = 0; i < 3; ++i)
    {
        m_NumCells[i]    = static_cast<UInt>(ceil((m_BMax[i] - m_BMin[i]) / m_CellSize));
        m_NumTotalCells *= m_NumCells[i];
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool Banana::Grid2D::isInsideGrid(const Vec2r& ppos) const noexcept
{
    for(int i = 0; i < 3; ++i)
    {
        if(ppos[i] < m_BMin[i] || ppos[i] > m_BMax[i])
            return false;
    }

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IndexType>
bool Banana::Grid2D::isValidCell(IndexType i, IndexType j, IndexType k)  const noexcept
{
    return (i >= 0 &&
            j >= 0 &&
            k >= 0 &&
            static_cast<UInt>(i) < m_NumCells[0] &&
            static_cast<UInt>(j) < m_NumCells[1] &&
            static_cast<UInt>(k) < m_NumCells[2]);
}

template<class IndexType>
bool Banana::Grid2D::isValidCell(const Vec2<IndexType>& index)  const noexcept
{
    return isValidCell(index[0], index[1], index[2]);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IndexType>
Vec2<IndexType> Banana::Grid2D::getCellIdx(const Vec2r& ppos)  const noexcept
{
    return Vec2<IndexType>(static_cast<IndexType>((ppos[0] - m_BMin[0]) / m_CellSize),
                           static_cast<IndexType>((ppos[1] - m_BMin[1]) / m_CellSize));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IndexType>
Vec2<IndexType> Banana::Grid2D::getValidCellIdx(const Vec2r& ppos)  const noexcept
{
    return getNearestValidCellIdx<IndexType>(getCellIdx<IndexType>(ppos));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IndexType>
Vec2<IndexType> Banana::Grid2D::getNearestValidCellIdx(const Vec2<IndexType>& cellIdx) const noexcept
{
    Vec2<IndexType> nearestCellIdx;

    for(int i = 0; i < 3; ++i)
        nearestCellIdx[i] = std::max<IndexType>(0, std::min<IndexType>(cellIdx[i], static_cast<IndexType>(m_NumCells[i]) - 1));

    return nearestCellIdx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Banana::Banana::Grid2D::constraintToGrid(Vec_Vec2r& particles)
{
    const Real  epsilon = 1e-9;
    const Vec2r minPos  = m_BMin + Vec2r(epsilon);
    const Vec2r maxPos  = m_BMax - Vec2r(epsilon);

    ParallelFuncs::parallel_for<size_t>(0, particles.size(),
                                        [&](size_t p)
                                        {
                                            Vec2r pos = particles[p];
                                            bool dirty = false;

                                            for(int i = 0; i < 3; ++i)
                                            {
                                                if(pos[i] < minPos[i] || pos[i] > maxPos[i])
                                                {
                                                    dirty = true;
                                                    pos[i] = MathHelpers::max(minPos[i], MathHelpers::min(pos[i], maxPos[i]));
                                                }
                                            }

                                            if(dirty)
                                                particles[p] = pos;
                                        });
}
