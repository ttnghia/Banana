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
template<Int N, class ScalarType>
void Grid<N, ScalarType >::setGrid(const VecX<N, ScalarType>& bMin, const VecX<N, ScalarType>& bMax, ScalarType cellSize)
{
    m_BMin = bMin;
    m_BMax = bMax;
    setCellSize(cellSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class ScalarType>
void Grid<N, ScalarType >::setCellSize(ScalarType cellSize)
{
    assert(cellSize > 0);
    m_CellSize     = cellSize;
    m_HalfCellSize = ScalarType(0.5) * m_CellSize;
    m_CellSizeSqr  = m_CellSize * m_CellSize;
    m_NTotalCells  = 1;

    for(Int i = 0; i < m_NCells.length(); ++i)
    {
        m_NCells[i] = static_cast<UInt>(ceil((m_BMax[i] - m_BMin[i]) / m_CellSize));
        m_NNodes[i] = m_NCells[i] + 1u;

        m_NTotalCells *= m_NCells[i];
        m_NTotalNodes *= m_NNodes[i];
    }

    m_bCellIdxNeedResize = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class ScalarType>
bool Grid<N, ScalarType >::isInsideGrid(const VecX<N, ScalarType>& ppos) const noexcept
{
    for(Int i = 0; i < N; ++i)
    {
        if(ppos[i] < m_BMin[i] || ppos[i] > m_BMax[i])
            return false;
    }

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class ScalarType>
void Grid<N, ScalarType >::constrainToGrid(Vector<VecX<N, ScalarType> >& positions)
{
    ParallelFuncs::parallel_for<size_t>(0, positions.size(),
                                        [&](size_t p)
                                        {
                                            VecX<N, ScalarType> pos = positions[p];
                                            bool dirty = false;

                                            for(Int i = 0; i < 3; ++i)
                                            {
                                                if(pos[i] < m_BMin[i] || pos[i] > m_BMax[i])
                                                {
                                                    dirty = true;
                                                    pos[i] = MathHelpers::clamp(pos[i], m_BMin[i],  m_BMax[i]);
                                                }
                                            }

                                            if(dirty)
                                                positions[p] = pos;
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class ScalarType>
void Grid<N, ScalarType >::collectIndexToCells(const Vector<VecX<N, ScalarType> >& positions)
{
    if(m_bCellIdxNeedResize)
    {
        m_ParticleIdxInCell.resize(getNCells());
        m_Lock.resize(getNCells());
        m_bCellIdxNeedResize = false;
    }

    for(auto& cell : m_ParticleIdxInCell.data())
        cell.resize(0);

    ParallelFuncs::parallel_for<UInt>(0, static_cast<UInt>(positions.size()),
                                      [&](UInt p)
                                      {
                                          auto cellIdx = getCellIdx<Int>(positions[p]);
                                          m_Lock(cellIdx).lock();
                                          m_ParticleIdxInCell(cellIdx).push_back(p);
                                          m_Lock(cellIdx).unlock();
                                      });

    ////////////////////////////////////////////////////////////////////////////////
    // reset particle index vector
    m_ParticleIdxSortedByCell.resize(0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class ScalarType>
void Grid<N, ScalarType >::collectIndexToCells(const Vector<VecX<N, ScalarType> >& positions, Vector<VecX<N, Int> >& particleCellIdx)
{
    if(m_bCellIdxNeedResize)
    {
        m_ParticleIdxInCell.resize(getNCells());
        m_Lock.resize(getNCells());
        m_bCellIdxNeedResize = false;
    }

    for(auto& cell : m_ParticleIdxInCell.data())
        cell.resize(0);

    ParallelFuncs::parallel_for<UInt>(0, static_cast<UInt>(positions.size()),
                                      [&](UInt p)
                                      {
                                          auto cellIdx = getCellIdx<Int>(positions[p]);
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
template<Int N, class ScalarType>
void Grid<N, ScalarType >::collectIndexToCells(const Vector<VecX<N, ScalarType> >& positions, Vector<VecX<N, ScalarType> >& particleCellPos)
{
    if(m_bCellIdxNeedResize)
    {
        m_ParticleIdxInCell.resize(getNCells());
        m_Lock.resize(getNCells());
        m_bCellIdxNeedResize = false;
    }

    for(auto& cell : m_ParticleIdxInCell.data())
        cell.resize(0);

    ParallelFuncs::parallel_for<UInt>(0, static_cast<UInt>(positions.size()),
                                      [&](UInt p)
                                      {
                                          auto cellPos = getCellIdx<ScalarType>(positions[p]);
                                          VecX<N, Int> cellIdx = NumberHelpers::convert<Int>(cellPos);
                                          particleCellPos[p] = cellPos;

                                          m_Lock(cellIdx).lock();
                                          m_ParticleIdxInCell(cellIdx).push_back(p);
                                          m_Lock(cellIdx).unlock();
                                      });

    ////////////////////////////////////////////////////////////////////////////////
    // reset particle index vector
    m_ParticleIdxSortedByCell.resize(0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class ScalarType>
void Grid<N, ScalarType >::getNeighborList(const Vector<VecX<N, ScalarType> >& positions, Vec_VecUInt& neighborList, Int cellSpan /*= 1*/)
{
    ParallelFuncs::parallel_for<size_t>(0, positions.size(), [&](size_t p) { getNeighborList(positions[p], neighborList[p], cellSpan); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class ScalarType>
void Grid<N, ScalarType >::getNeighborList(const Vec2<ScalarType>& ppos, Vec_UInt& neighborList, Int cellSpan /*= 1*/)
{
    neighborList.resize(0);
    Vec2i cellIdx = getCellIdx<Int>(ppos);

    for(Int lj = -cellSpan; lj <= cellSpan; ++lj)
    {
        for(Int li = -cellSpan; li <= cellSpan; ++li)
        {
            const Vec2i neighborCellIdx = cellIdx + Vec2i(li, lj);

            if(!isValidCell(neighborCellIdx))
                continue;

            const Vec_UInt& cell = m_ParticleIdxInCell(neighborCellIdx);

            if(cell.size() > 0)
                neighborList.insert(neighborList.end(), cell.begin(), cell.end());
        }
    }   // end loop over neighbor cells
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class ScalarType>
void Grid<N, ScalarType >::getNeighborList(const Vec3<ScalarType>& ppos, Vec_UInt& neighborList, Int cellSpan /*= 1*/)
{
    neighborList.resize(0);

    Vec3i cellIdx = getCellIdx<Int>(ppos);

    for(Int lk = -cellSpan; lk <= cellSpan; ++lk)
    {
        for(Int lj = -cellSpan; lj <= cellSpan; ++lj)
        {
            for(Int li = -cellSpan; li <= cellSpan; ++li)
            {
                const Vec3i neighborCellIdx = cellIdx + Vec3i(li, lj, lk);

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
template<Int N, class ScalarType>
void Grid<N, ScalarType >::getNeighborList(const Vector<VecX<N, ScalarType> >& positions, Vec_VecUInt& neighborList, ScalarType d2, Int cellSpan /*= 1*/)
{
    ParallelFuncs::parallel_for<size_t>(0, positions.size(), [&](size_t p) { getNeighborList(positions, positions[p], neighborList[p], d2, cellSpan); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class ScalarType>
void Grid<N, ScalarType >::getNeighborList(const Vec_Vec2<ScalarType>& positions, const Vec2<ScalarType>& ppos, Vec_UInt& neighborList, ScalarType d2, Int cellSpan /*= 1*/)
{
    neighborList.resize(0);

    Vec2i cellIdx = getCellIdx<Int>(ppos);

    for(Int lj = -cellSpan; lj <= cellSpan; ++lj)
    {
        for(Int li = -cellSpan; li <= cellSpan; ++li)
        {
            const Vec2i neighborCellIdx = cellIdx + Vec2i(li, lj);

            if(!isValidCell(neighborCellIdx))
                continue;

            const Vec_UInt& cell = m_ParticleIdxInCell(neighborCellIdx);

            if(cell.size() > 0)
            {
                for(unsigned Int q : cell)
                {
                    ScalarType pqd2 = glm::length2(ppos - positions[q]);

                    if(pqd2 > 0 && pqd2 < d2)
                        neighborList.push_back(q);
                }
            }
        }
    }   // end loop over neighbor cells
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class ScalarType>
void Grid<N, ScalarType >::getNeighborList(const Vec_Vec3<ScalarType>& positions, const Vec3<ScalarType>& ppos, Vec_UInt& neighborList, ScalarType d2, Int cellSpan /*= 1*/)
{
    neighborList.resize(0);

    Vec3i cellIdx = getCellIdx<Int>(ppos);

    for(Int lk = -cellSpan; lk <= cellSpan; ++lk)
    {
        for(Int lj = -cellSpan; lj <= cellSpan; ++lj)
        {
            for(Int li = -cellSpan; li <= cellSpan; ++li)
            {
                const Vec3i neighborCellIdx = cellIdx + Vec3i(li, lj, lk);

                if(!isValidCell(neighborCellIdx))
                    continue;

                const Vec_UInt& cell = m_ParticleIdxInCell(neighborCellIdx);

                if(cell.size() > 0)
                {
                    for(UInt q : cell)
                    {
                        ScalarType pqd2 = glm::length2(ppos - positions[q]);

                        if(pqd2 > 0 && pqd2 < d2)
                            neighborList.push_back(q);
                    }
                }
            }
        }
    }   // end loop over neighbor cells
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class ScalarType>
void Grid<N, ScalarType >::sortData(Vector<VecX<N, ScalarType> >& data)
{
    const auto& sortedIdx = getParticleIdxSortedByCell();
    assert(sortedIdx.size() == data.size());

    Vector<VecX<N, ScalarType> > tmp(data.begin(), data.end());
    std::transform(sortedIdx.cbegin(), sortedIdx.cend(),
#ifdef _MSC_VER
                   stdext::unchecked_array_iterator<VecX<N, ScalarType>*>(data.data()),
#else
                   data,
#endif
                   [&](UInt i) { return tmp[i]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class ScalarType>
const Vec_UInt& Grid<N, ScalarType >::getParticleIdxSortedByCell()
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
