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

#include <Banana/Grid/Grid3D.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class Grid3DHashing : public Grid3D<RealType>
{
public:
    Grid3DHashing() = default;
    Grid3DHashing(const Vec3<RealType>& bMin, const Vec3<RealType>& bMax, RealType cellSize) : Grid3D(bMin, bMax, cellSize) {}

    virtual void setCellSize(RealType cellSize) override;

    void            collectIndexToCells(Vec_Vec3<RealType>& particles);
    void            getNeighborList(const Vec_Vec3<RealType>& particles, Vec_VecUInt& neighborList, int cellSpan = 1);
    void            getNeighborList(const Vec3<RealType>& ppos, Vec_UInt& neighborList, int cellSpan = 1);
    void            getNeighborList(const Vec_Vec3<RealType>& particles, Vec_VecUInt& neighborList, RealType d2, int cellSpan = 1);
    void            getNeighborList(const Vec_Vec3<RealType>& particles, const Vec3<RealType>& ppos, Vec_UInt& neighborList, RealType d2, int cellSpan = 1);
    const Vec_UInt& getParticleIdxSortedByCell();

private:
    Array3_VecUInt m_CellParticleIdx;
    bool           m_bCellIdxNeedResize = false;      // to track and resize the m_CellParticleIdx array
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Grid3DHashing<RealType>::setCellSize(RealType cellSize)
{
    Grid3D<RealType>::setCellSize(cellSize);
    m_bCellIdxNeedResize = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Grid3DHashing<RealType>::collectIndexToCells(Vec_Vec3<RealType>& particles)
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
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::Grid3DHashing<RealType>::getNeighborList(const Vec_Vec3<RealType>& particles, Vec_VecUInt& neighborList, int cellSpan /*= 1*/)
{
    ParallelFuncs::parallel_for<size_t>(0, particles.size(), [&](size_t p) { getNeighborList(particles[p], neighborList[p]); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::Grid3DHashing<RealType>::getNeighborList(const Vec3<RealType>& ppos, Vec_UInt& neighborList, int cellSpan /*= 1*/)
{
    neighborList.resize(0);

    Vec3i cellIdx = getCellIdx<int>(ppos);

    for(int lk = -cellSpan; lk <= cellSpan; ++lk)
    {
        for(int lj = -cellSpan; lj <= cellSpan; ++lj)
        {
            for(int li = -cellSpan; li <= cellSpan; ++li)
            {
                const Vec3i neighborCellIdx = Vec3i(cellIdx[0] + li, cellIdx[1] + lj, cellIdx[2] + lk);

                if(!isValidCell(neighborCellIdx))
                    continue;

                const Vec_UInt& cell = m_CellParticleIdx(neighborCellIdx);

                if(cell.size() > 0)
                    neighborList.insert(neighborList.end(), cell.begin(), cell.end());
            }
        }
    }   // end loop over neighbor cells
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::Grid3DHashing<RealType>::getNeighborList(const Vec_Vec3<RealType>& particles, Vec_VecUInt& neighborList, RealType d2, int cellSpan /*= 1*/)
{
    ParallelFuncs::parallel_for<size_t>(0, particles.size(), [&](size_t p) { getNeighborList(particles, particles[p], neighborList[p], d2); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::Grid3DHashing<RealType>::getNeighborList(const Vec_Vec3<RealType>& particles, const Vec3<RealType>& ppos, Vec_UInt& neighborList, RealType d2, int cellSpan /*= 1*/)
{
    neighborList.resize(0);

    Vec3i cellIdx = getCellIdx<int>(ppos);

    for(int lk = -cellSpan; lk <= cellSpan; ++lk)
    {
        for(int lj = -cellSpan; lj <= cellSpan; ++lj)
        {
            for(int li = -cellSpan; li <= cellSpan; ++li)
            {
                const Vec3i neighborCellIdx = Vec3i(cellIdx[0] + li, cellIdx[1] + lj, cellIdx[2] + lk);

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
        }
    }   // end loop over neighbor cells
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
const Vec_UInt& Banana::Grid3DHashing<RealType>::getParticleIdxSortedByCell()
{
    static Vec_UInt particleIdx;
    particleIdx.resize(0);

    for(auto& cell : m_CellParticleIdx.vec_data())
    {
        if(cell.size() > 0)
            particleIdx.insert(particleIdx.end(), cell.begin(), cell.end());
    }

    return particleIdx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana