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

#include <Banana/Grid/Grid3DHashing.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Grid3DHashing::setCellSize(Real cellSize)
{
    Grid3D::setCellSize(cellSize);
    m_bCellIdxNeedResize = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Grid3DHashing::collectIndexToCells(const Vec_Vec3r& particles)
{
    if(m_bCellIdxNeedResize)
    {
        m_CellParticleIdx.resize(getNumCells());
        m_bCellIdxNeedResize = false;
    }

    for(auto& cell : m_CellParticleIdx.vec_data())
        cell.resize(0);

    // cannot run in parallel....
    for(UInt p = 0, p_end = static_cast<UInt>(particles.size()); p < p_end; ++p)
    {
        auto cellIdx = getCellIdx<int>(particles[p]);
        m_CellParticleIdx(cellIdx).push_back(p);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // reset particle index vector
    m_ParticleIdx.resize(0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Banana::Grid3DHashing::getNeighborList(const Vec_Vec3r& particles, Vec_VecUInt& neighborList, int cellSpan /*= 1*/)
{
    ParallelFuncs::parallel_for<size_t>(0, particles.size(), [&](size_t p) { getNeighborList(particles[p], neighborList[p], cellSpan); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Banana::Grid3DHashing::getNeighborList(const Vec3r& ppos, Vec_UInt& neighborList, int cellSpan /*= 1*/)
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
void Banana::Grid3DHashing::getNeighborList(const Vec_Vec3r& particles, Vec_VecUInt& neighborList, Real d2, int cellSpan /*= 1*/)
{
    ParallelFuncs::parallel_for<size_t>(0, particles.size(), [&](size_t p) { getNeighborList(particles, particles[p], neighborList[p], d2, cellSpan); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Banana::Grid3DHashing::getNeighborList(const Vec_Vec3r& particles, const Vec3r& ppos, Vec_UInt& neighborList, Real d2, int cellSpan /*= 1*/)
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
                        Real pqd2 = glm::length2(ppos - particles[q]);

                        if(pqd2 > 0 && pqd2 < d2)
                            neighborList.push_back(q);
                    }
                }
            }
        }
    }   // end loop over neighbor cells
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const Vec_UInt& Banana::Grid3DHashing::getParticleIdxSortedByCell()
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
void Banana::Grid3DHashing::sortData(Vec_Vec3r& data)
{
    const auto& sortedIdx = getParticleIdxSortedByCell();
    assert(sortedIdx.size() == data.size());

    Vec_Vec3r tmp(data.begin(), data.end());
    std::transform(sortedIdx.cbegin(), sortedIdx.cend(),
#ifdef _MSC_VER
                   stdext::unchecked_array_iterator<Vec3r*>(data.data()),
#else
                   data,
#endif
                   [&](UInt i) { return tmp[i]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana