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
template<class ScalarType>
void Grid3D<ScalarType>::setGrid(const Vec3<ScalarType>& bMin, const Vec3<ScalarType>& bMax, ScalarType cellSize)
{
    m_BMin = bMin;
    m_BMax = bMax;
    setCellSize(cellSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
void Grid3D<ScalarType>::setCellSize(ScalarType cellSize)
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
template<class ScalarType>
template<class IndexType>
bool Grid3D<ScalarType>::isValidCell(IndexType i, IndexType j, IndexType k)  const noexcept
{
    return (i >= 0 &&
            j >= 0 &&
            k >= 0 &&
            static_cast<unsigned int>(i) < m_NumCells[0] &&
            static_cast<unsigned int>(j) < m_NumCells[1] &&
            static_cast<unsigned int>(k) < m_NumCells[2]);
}

template<class ScalarType>
template<class IndexType>
bool Grid3D<ScalarType>::isValidCell(const Vec3<IndexType>& index)  const noexcept
{
    return isValidCell(index[0], index[1], index[2]);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
template<class IndexType>
Vec3<IndexType> Grid3D<ScalarType>::getCellIdx(const Vec3<ScalarType>& position)  const noexcept
{
    return Vec3<IndexType>(static_cast<IndexType>((position[0] - m_BMin[0]) / m_CellSize),
                           static_cast<IndexType>((position[1] - m_BMin[1]) / m_CellSize),
                           static_cast<IndexType>((position[2] - m_BMin[2]) / m_CellSize));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
template<class IndexType>
Vec3<IndexType> Grid3D<ScalarType>::getValidCellIdx(const Vec3<ScalarType>& position)  const noexcept
{
    return getNearestValidCellIdx<IndexType>(getCellIdx<IndexType>(position));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
template<class IndexType>
Vec3<IndexType> Grid3D<ScalarType>::getNearestValidCellIdx(const Vec3<IndexType>& cellIdx) const noexcept
{
    Vec3<IndexType> nearestCellIdx;

    for(int i = 0; i < 3; ++i)
        nearestCellIdx[i] = std::max<IndexType>(0, std::min<IndexType>(cellIdx[i], static_cast<IndexType>(m_NumCells[i]) - 1));

    return nearestCellIdx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
void Grid3D<ScalarType>::enableCellParticleIdx(bool bEnable /* = true */)
{
    if(!bEnable)
        m_CellParticleIdx.clear();
    else
        m_CellParticleIdx.resize(getNumCells());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
void Grid3D<ScalarType>::collectParticlesToCells(Vec_Vec3<ScalarType>& particles)
{
    for(auto& cell : m_CellParticleIdx.vec_data())
        cell.resize(0);

    // cannot run in parallel....
    for(UInt32 p = 0, p_end = static_cast<UInt32>(m_CellParticleIdx.size()); p < p_end; ++p)
    {
        auto cellIdx = getValidCellIdx<int>(particles[p]);
        m_CellParticleIdx(cellIdx).push_back(p);
    }

    if(m_bSortParticles)
    {
        static unsigned int collectionCount = 0;
        ++collectionCount;

        if(collectionCount == m_SortFrequency)
        {
            collectionCount = 0;
            sortParticleByCell(particles);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
void Grid3D<ScalarType>::enableSortParticles(bool bEnable /* = true */)
{
    m_bSortParticles = bEnable;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
void Grid3D<ScalarType>::sortParticleByCell(Vec_Vec3<ScalarType>& particles)
{
    static Vec_Vec3<ScalarType>& tmpParticles;
    tmpParticles.resize(particles.size());

    size_t p = 0;
    for(auto& cell : m_CellParticleIdx.vec_data())
    {
        if(cell.size() > 0)
        {
            for(unsigned int q : cell)
                tmpParticles[p++] = particles[q];
        }
    }

    assert(p == particles.size());
    std::copy(tmpParticles.begin(), tmpParticles.end(), particles.begin());
}
