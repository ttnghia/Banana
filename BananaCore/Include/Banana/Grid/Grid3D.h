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
#include <Banana/ParallelHelpers/ParallelFuncs.h>

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

    Vec3<RealType> getBMin() const noexcept { return m_BMin; }
    Vec3<RealType> getBMax() const noexcept { return m_BMax; }
    unsigned int getNumCellX() const noexcept { return m_NumCells[0]; }
    unsigned int getNumCellY() const noexcept { return m_NumCells[1]; }
    unsigned int getNumCellZ() const noexcept { return m_NumCells[2]; }
    unsigned int getNumTotalCells() const noexcept { return m_NumTotalCells; }
    Vec3<unsigned int> getNumCells() const noexcept { return m_NumCells; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void setCellSize(RealType cellSize);
    RealType getCellSize() const noexcept { return m_CellSize; }

    ////////////////////////////////////////////////////////////////////////////////
    bool isInsideGrid(const Vec3<RealType>& ppos) const noexcept;

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
    void constraintToGrid(Vec_Vec3<RealType>& particles);

protected:
    Vec3<RealType>     m_BMin          = Vec3<RealType>(0);
    Vec3<RealType>     m_BMax          = Vec3<RealType>(1);
    Vec3<unsigned int> m_NumCells      = Vec3<unsigned int>(0);
    unsigned int       m_NumTotalCells = 1;
    RealType           m_CellSize      = RealType(1.0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Grid3D<RealType>::setGrid(const Vec3<RealType>& bMin, const Vec3<RealType>& bMax, RealType cellSize)
{
    m_BMin = bMin;
    m_BMax = bMax;
    setCellSize(cellSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Grid3D<RealType>::setCellSize(RealType cellSize)
{
    assert(cellSize > 0);
    m_CellSize      = cellSize;
    m_NumTotalCells = 1;

    for(int i = 0; i < 3; ++i)
    {
        m_NumCells[i]    = static_cast<unsigned int>(ceil((m_BMax[i] - m_BMin[i]) / m_CellSize));
        m_NumTotalCells *= m_NumCells[i];
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
bool Grid3D<RealType>::isInsideGrid(const Vec3<RealType>& ppos) const noexcept
{
    for(int i = 0; i < 3; ++i)
    {
        if(ppos[i] < m_BMin[i] || ppos[i] > m_BMax[i])
            return false;
    }

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
template<class IndexType>
bool Grid3D<RealType>::isValidCell(IndexType i, IndexType j, IndexType k)  const noexcept
{
    return (i >= 0 &&
            j >= 0 &&
            k >= 0 &&
            static_cast<unsigned int>(i) < m_NumCells[0] &&
            static_cast<unsigned int>(j) < m_NumCells[1] &&
            static_cast<unsigned int>(k) < m_NumCells[2]);
}

template<class RealType>
template<class IndexType>
bool Grid3D<RealType>::isValidCell(const Vec3<IndexType>& index)  const noexcept
{
    return isValidCell(index[0], index[1], index[2]);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
template<class IndexType>
Vec3<IndexType> Grid3D<RealType>::getCellIdx(const Vec3<RealType>& ppos)  const noexcept
{
    return Vec3<IndexType>(static_cast<IndexType>((ppos[0] - m_BMin[0]) / m_CellSize),
                           static_cast<IndexType>((ppos[1] - m_BMin[1]) / m_CellSize),
                           static_cast<IndexType>((ppos[2] - m_BMin[2]) / m_CellSize));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
template<class IndexType>
Vec3<IndexType> Grid3D<RealType>::getValidCellIdx(const Vec3<RealType>& ppos)  const noexcept
{
    return getNearestValidCellIdx<IndexType>(getCellIdx<IndexType>(ppos));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
template<class IndexType>
Vec3<IndexType> Grid3D<RealType>::getNearestValidCellIdx(const Vec3<IndexType>& cellIdx) const noexcept
{
    Vec3<IndexType> nearestCellIdx;

    for(int i = 0; i < 3; ++i)
        nearestCellIdx[i] = std::max<IndexType>(0, std::min<IndexType>(cellIdx[i], static_cast<IndexType>(m_NumCells[i]) - 1));

    return nearestCellIdx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::Grid3D<RealType>::constraintToGrid(Vec_Vec3<RealType>& particles)
{
    const RealType       epsilon = 1e-9;
    const Vec3<RealType> minPos  = m_BMin + Vec3<RealType>(epsilon);
    const Vec3<RealType> maxPos  = m_BMax - Vec3<RealType>(epsilon);

    ParallelFuncs::parallel_for<size_t>(0, particles.size(),
                                        [&](size_t p)
                                        {
                                            Vec3<RealType> pos = particles[p];
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana