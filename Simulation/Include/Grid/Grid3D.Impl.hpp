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

    m_bCellIdxNeedResize = true;
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

    for(size_t p = 0, pend = particles.size(); p < pend; ++p)
    {
        Vec3<RealType> pos   = particles[p];
        bool           dirty = false;

        for(int i = 0; i < 3; ++i)
        {
            if(pos[i] < minPos[i] || pos[i] > maxPos[i])
            {
                dirty  = true;
                pos[i] = MathHelpers::max(minPos[i], MathHelpers::min(pos[i], maxPos[i]));
            }
        }

        if(dirty)
            particles[p] = pos;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Grid3D<RealType>::collectIndexToCells(Vec_Vec3<RealType>& particles)
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
void Banana::Grid3D<RealType>::getNeighborList(const Vec3<RealType>& ppos, Vec_UInt& neighborList, int cellSpan /*= 1*/)
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
                {
                    continue;
                }

                const Vec_UInt& cell = m_CellParticleIdx(neighborCellIdx);

                if(cell.size() > 0)
                    neighborList.insert(neighborList.end(), cell.begin(), cell.end());
            }
        }
    } // end loop over neighbor cells
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
const Vec_UInt& Banana::Grid3D<RealType>::getParticleIdxSortedByCell()
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
