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

#include <Banana/Grid/Grid2D.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class Grid2DHashing : public Grid2D<RealType>
{
public:
    Grid2DHashing() = default;
    Grid2DHashing(const Vec2<RealType>& bMin, const Vec2<RealType>& bMax, RealType cellSize) : Grid2D(bMin, bMax, cellSize), m_bCellIdxNeedResize(true) {}

    virtual void setCellSize(RealType cellSize) override;

    void collectIndexToCells(const Vec_Vec2<RealType>& particles);
    void getNeighborList(const Vec_Vec2<RealType>& particles, Vec_VecUInt& neighborList, int cellSpan = 1);
    void getNeighborList(const Vec2<RealType>& ppos, Vec_UInt& neighborList, int cellSpan = 1);
    void getNeighborList(const Vec_Vec2<RealType>& particles, Vec_VecUInt& neighborList, RealType d2, int cellSpan = 1);
    void getNeighborList(const Vec_Vec2<RealType>& particles, const Vec2<RealType>& ppos, Vec_UInt& neighborList, RealType d2, int cellSpan = 1);

    const Vec_UInt& getParticleIdxSortedByCell();
    void            sortData(Vec_Vec2<RealType>& data);

    template<class IndexType>
    const Vec_UInt& getParticleIdxInCell(const Vec2<IndexType>& cellIdx) const { return m_CellParticleIdx(cellIdx); }

private:
    Array2_VecUInt m_CellParticleIdx;
    Vec_UInt       m_ParticleIdx;
    bool           m_bCellIdxNeedResize = false;      // to track and resize the m_CellParticleIdx array
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Grid2DHashing<RealType>::setCellSize(RealType cellSize)
{
    Grid2D<RealType>::setCellSize(cellSize);
    m_bCellIdxNeedResize = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Grid2DHashing<RealType>::collectIndexToCells(const Vec_Vec2<RealType>& particles)
{
    if(m_bCellIdxNeedResize)
    {
        m_CellParticleIdx.resize(getNumCells());
        m_bCellIdxNeedResize = false;
    }

    for(auto& cell : m_CellParticleIdx.vec_data())
        cell.resize(0);

    // cannot run in parallel....
    for(UInt32 p = 0, p_end = static_cast<UInt32>(particles.size()); p < p_end; ++p)
    {
        auto cellIdx = getCellIdx<int>(particles[p]);
        m_CellParticleIdx(cellIdx).push_back(p);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // reset particle index vector
    m_ParticleIdx.resize(0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::Grid2DHashing<RealType>::getNeighborList(const Vec_Vec2<RealType>& particles, Vec_VecUInt& neighborList, int cellSpan /*= 1*/)
{
    ParallelFuncs::parallel_for<size_t>(0, particles.size(), [&](size_t p) { getNeighborList(particles[p], neighborList[p], cellSpan); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::Grid2DHashing<RealType>::getNeighborList(const Vec2<RealType>& ppos, Vec_UInt& neighborList, int cellSpan /*= 1*/)
{
    neighborList.resize(0);

    Vec2i cellIdx = getCellIdx<int>(ppos);

    for(int lj = -cellSpan; lj <= cellSpan; ++lj)
    {
        for(int li = -cellSpan; li <= cellSpan; ++li)
        {
            const Vec2i neighborCellIdx = Vec2i(cellIdx[0] + li, cellIdx[1] + lj);

            if(!isValidCell(neighborCellIdx))
                continue;

            const Vec_UInt& cell = m_CellParticleIdx(neighborCellIdx);

            if(cell.size() > 0)
                neighborList.insert(neighborList.end(), cell.begin(), cell.end());
        }
    }   // end loop over neighbor cells
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::Grid2DHashing<RealType>::getNeighborList(const Vec_Vec2<RealType>& particles, Vec_VecUInt& neighborList, RealType d2, int cellSpan /*= 1*/)
{
    ParallelFuncs::parallel_for<size_t>(0, particles.size(), [&](size_t p) { getNeighborList(particles, particles[p], neighborList[p], d2, cellSpan); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::Grid2DHashing<RealType>::getNeighborList(const Vec_Vec2<RealType>& particles, const Vec2<RealType>& ppos, Vec_UInt& neighborList, RealType d2, int cellSpan /*= 1*/)
{
    neighborList.resize(0);

    Vec2i cellIdx = getCellIdx<int>(ppos);

    for(int lj = -cellSpan; lj <= cellSpan; ++lj)
    {
        for(int li = -cellSpan; li <= cellSpan; ++li)
        {
            const Vec2i neighborCellIdx = Vec2i(cellIdx[0] + li, cellIdx[1] + lj);

            if(!isValidCell(neighborCellIdx))
                continue;

            const Vec_UInt& cell = m_CellParticleIdx(neighborCellIdx);

            if(cell.size() > 0)
            {
                for(unsigned int q : cell)
                {
                    RealType pqd2 = glm::length2(ppos - particles[q]);

                    if(pqd2 > 0 && pqd2 < d2)
                        neighborList.push_back(q);
                }
            }
        }
    }   // end loop over neighbor cells
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
const Vec_UInt& Banana::Grid2DHashing<RealType>::getParticleIdxSortedByCell()
{
    if(m_ParticleIdx.size() > 0)
        return m_ParticleIdx;

    for(auto& cell : m_CellParticleIdx.vec_data())
    {
        if(cell.size() > 0)
            m_ParticleIdx.insert(m_ParticleIdx.end(), cell.begin(), cell.end());
    }

    return m_ParticleIdx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::Grid2DHashing<RealType>::sortData(Vec_Vec2<RealType>& data)
{
    const auto& sortedIdx = getParticleIdxSortedByCell();
    assert(sortedIdx.size() == data.size());

    Vec_Vec2<RealType> tmp(data.begin(), data.end());
    std::transform(sortedIdx.cbegin(), sortedIdx.cend(),
#ifdef _MSC_VER
                   stdext::unchecked_array_iterator<Vec2<RealType>*>(data.data()),
#else
                   data,
#endif
                   [&](UInt32 i) { return tmp[i]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana