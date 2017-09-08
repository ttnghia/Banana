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
#include <Banana/ParallelHelpers/ParallelFuncs.h>

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
void Grid3DHashing::collectIndexToCells(const Vec_Vec3r& positions)
{
    if(m_bCellIdxNeedResize)
    {
        m_ParticleIdxInCell.resize(getNumCells());
        m_Lock.resize(getNumCells());
        m_bCellIdxNeedResize = false;
    }

    for(auto& cell : m_ParticleIdxInCell.data())
        cell.resize(0);

    ParallelFuncs::parallel_for<UInt>(0, static_cast<UInt>(positions.size()),
                                      [&](UInt p)
                                      {
                                          auto cellIdx = getCellIdx<int>(positions[p]);
                                          m_Lock(cellIdx).lock();
                                          m_ParticleIdxInCell(cellIdx).push_back(p);
                                          m_Lock(cellIdx).unlock();
                                      });

    ////////////////////////////////////////////////////////////////////////////////
    // reset particle index vector
    m_ParticleIdxSortedByCell.resize(0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Grid3DHashing::collectIndexToCells(const Vec_Vec3r& positions, Vec_Vec3i& particleCellIdx)
{
    if(m_bCellIdxNeedResize)
    {
        m_ParticleIdxInCell.resize(getNumCells());
        m_Lock.resize(getNumCells());
        m_bCellIdxNeedResize = false;
    }

    for(auto& cell : m_ParticleIdxInCell.data())
        cell.resize(0);

    ParallelFuncs::parallel_for<UInt>(0, static_cast<UInt>(positions.size()),
                                      [&](UInt p)
                                      {
                                          auto cellIdx = getCellIdx<int>(positions[p]);
                                          particleCellIdx[p] = cellIdx;

                                          m_Lock(cellIdx).lock();
                                          m_ParticleIdxInCell(cellIdx).push_back(p);
                                          m_Lock(cellIdx).unlock();
                                      });

    ////////////////////////////////////////////////////////////////////////////////
    // reset particle index vector
    m_ParticleIdxSortedByCell.resize(0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Grid3DHashing::getNeighborList(const Vec_Vec3r& positions, Vec_VecUInt& neighborList, int cellSpan /*= 1*/)
{
    ParallelFuncs::parallel_for<size_t>(0, positions.size(), [&](size_t p) { getNeighborList(positions[p], neighborList[p], cellSpan); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Grid3DHashing::getNeighborList(const Vec3r& ppos, Vec_UInt& neighborList, int cellSpan /*= 1*/)
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

                const Vec_UInt& cell = m_ParticleIdxInCell(neighborCellIdx);

                if(cell.size() > 0)
                    neighborList.insert(neighborList.end(), cell.begin(), cell.end());
            }
        }
    }   // end loop over neighbor cells
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Grid3DHashing::getNeighborList(const Vec_Vec3r& positions, Vec_VecUInt& neighborList, Real d2, int cellSpan /*= 1*/)
{
    ParallelFuncs::parallel_for<size_t>(0, positions.size(), [&](size_t p) { getNeighborList(positions, positions[p], neighborList[p], d2, cellSpan); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Grid3DHashing::getNeighborList(const Vec_Vec3r& positions, const Vec3r& ppos, Vec_UInt& neighborList, Real d2, int cellSpan /*= 1*/)
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

                const Vec_UInt& cell = m_ParticleIdxInCell(neighborCellIdx);

                if(cell.size() > 0)
                {
                    for(UInt q : cell)
                    {
                        Real pqd2 = glm::length2(ppos - positions[q]);

                        if(pqd2 > 0 && pqd2 < d2)
                            neighborList.push_back(q);
                    }
                }
            }
        }
    }   // end loop over neighbor cells
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const Vec_UInt& Grid3DHashing::getParticleIdxSortedByCell()
{
    if(m_ParticleIdxSortedByCell.size() > 0)
        return m_ParticleIdxSortedByCell;

    for(auto& cell : m_ParticleIdxInCell.data())
    {
        if(cell.size() > 0)
            m_ParticleIdxSortedByCell.insert(m_ParticleIdxSortedByCell.end(), cell.begin(), cell.end());
    }

    return m_ParticleIdxSortedByCell;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Grid3DHashing::sortData(Vec_Vec3r& data)
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