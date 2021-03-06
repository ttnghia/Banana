//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
//    /                    Created: 2017 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <Banana/Grid/Grid.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/ParallelHelpers/Scheduler.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana {
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Grid<N, RealType>::setGrid(const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax, RealType cellSize) {
    m_BMin = bMin;
    m_BMax = bMax;
    setCellSize(cellSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Grid<N, RealType>::setCellSize(RealType cellSize) {
    assert(cellSize > 0);
    m_CellSize     = cellSize;
    m_InvCellSize  = RealType(1.0) / m_CellSize;
    m_HalfCellSize = RealType(0.5) * m_CellSize;
    m_CellSizeSqr  = m_CellSize * m_CellSize;
    m_NTotalCells  = 1;

    for(Int i = 0; i < m_NCells.length(); ++i) {
        m_NCells[i] = static_cast<UInt>(ceil((m_BMax[i] - m_BMin[i]) / m_CellSize));
        m_NNodes[i] = m_NCells[i] + 1u;

        m_NTotalCells *= m_NCells[i];
        m_NTotalNodes *= m_NNodes[i];
    }

    m_bCellIdxNeedResize = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool Grid<N, RealType>::isInsideGrid(const VecX<N, RealType>& ppos) const noexcept{
    for(Int d = 0; d < N; ++d) {
        if(ppos[d] < m_BMin[d] || ppos[d] > m_BMax[d]) {
            return false;
        }
    }

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Grid<N, RealType>::constrainToGrid(Vector<VecX<N, RealType>>& positions) {
    Scheduler::parallel_for(positions.size(),
                            [&](size_t p) {
                                VecX<N, RealType> pos = positions[p];
                                bool dirty = false;

                                for(Int d = 0; d < N; ++d) {
                                    if(pos[d] < m_BMin[d] || pos[d] > m_BMax[d]) {
                                        dirty  = true;
                                        pos[d] = MathHelpers::clamp(pos[d], m_BMin[d], m_BMax[d]);
                                    }
                                }

                                if(dirty) {
                                    positions[p] = pos;
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Grid<N, RealType>::collectIndexToCells(const Vector<VecX<N, RealType>>& positions) {
    if(m_bCellIdxNeedResize) {
        m_ParticleIdxInCell.resize(getNCells());
        m_Lock.resize(getNCells());
        m_bCellIdxNeedResize = false;
    }

    for(auto& cell : m_ParticleIdxInCell.data()) {
        cell.resize(0);
    }

    Scheduler::parallel_for(static_cast<UInt>(positions.size()),
                            [&](UInt p) {
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
template<Int N, class RealType>
void Grid<N, RealType>::collectIndexToCells(const Vector<VecX<N, RealType>>& positions, Vector<VecX<N, Int>>& particleCellIdx) {
    if(m_bCellIdxNeedResize) {
        m_ParticleIdxInCell.resize(getNCells());
        m_Lock.resize(getNCells());
        m_bCellIdxNeedResize = false;
    }

    for(auto& cell : m_ParticleIdxInCell.data()) {
        cell.resize(0);
    }

    Scheduler::parallel_for(static_cast<UInt>(positions.size()),
                            [&](UInt p) {
                                auto cellIdx       = getCellIdx<Int>(positions[p]);
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
template<Int N, class RealType>
void Grid<N, RealType>::collectIndexToCells(const Vector<VecX<N, RealType>>& positions, Vector<VecX<N, RealType>>& particleCellPos) {
    if(m_bCellIdxNeedResize) {
        m_ParticleIdxInCell.resize(getNCells());
        m_Lock.resize(getNCells());
        m_bCellIdxNeedResize = false;
    }

    for(auto& cell : m_ParticleIdxInCell.data()) {
        cell.resize(0);
    }

    Scheduler::parallel_for(static_cast<UInt>(positions.size()),
                            [&](UInt p) {
                                auto cellPos       = getCellIdx<RealType>(positions[p]);
                                auto cellIdx       = VecX<N, Int>(cellPos);
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
template<Int N, class RealType>
void Grid<N, RealType>::getNeighborList(const Vector<VecX<N, RealType>>& positions, Vec_VecUInt& neighborList, Int cellSpan /*= 1*/) {
    Scheduler::parallel_for(positions.size(), [&](size_t p) { getNeighborList(positions[p], neighborList[p], cellSpan); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Grid<N, RealType>::getNeighborList(const VecX<N, RealType>& ppos, Vec_UInt& neighborList, Int cellSpan /*= 1*/) {
    neighborList.resize(0);
    if constexpr(N == 2)
    {
        Vec2i cellIdx = getCellIdx<Int>(ppos);
        for(Int lj = -cellSpan; lj <= cellSpan; ++lj) {
            for(Int li = -cellSpan; li <= cellSpan; ++li) {
                const auto neighborCellIdx = cellIdx + Vec2i(li, lj);

                if(!isValidCell(neighborCellIdx)) {
                    continue;
                }

                const auto& cell = m_ParticleIdxInCell(neighborCellIdx);
                if(cell.size() > 0) {
                    neighborList.insert(neighborList.end(), cell.begin(), cell.end());
                }
            }
        } // end loop over neighbor cells
    } else {
        Vec3i cellIdx = getCellIdx<Int>(ppos);
        for(Int lk = -cellSpan; lk <= cellSpan; ++lk) {
            for(Int lj = -cellSpan; lj <= cellSpan; ++lj) {
                for(Int li = -cellSpan; li <= cellSpan; ++li) {
                    const auto neighborCellIdx = cellIdx + Vec3i(li, lj, lk);

                    if(!isValidCell(neighborCellIdx)) {
                        continue;
                    }

                    const auto& cell = m_ParticleIdxInCell(neighborCellIdx);
                    if(cell.size() > 0) {
                        neighborList.insert(neighborList.end(), cell.begin(), cell.end());
                    }
                }
            }
        } // end loop over neighbor cells
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Grid<N, RealType>::getNeighborList(const Vector<VecX<N, RealType>>& positions, Vec_VecUInt& neighborList, RealType d2, Int cellSpan /*= 1*/) {
    Scheduler::parallel_for(positions.size(), [&](size_t p) { getNeighborList(positions, positions[p], neighborList[p], d2, cellSpan); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Grid<N, RealType>::getNeighborList(const Vec_VecX<N, RealType>& positions, const VecX<N, RealType>& ppos, Vec_UInt& neighborList, RealType d2, Int cellSpan /*= 1*/) {
    neighborList.resize(0);

    if constexpr(N == 2)
    {
        Vec2i cellIdx = getCellIdx<Int>(ppos);
        for(Int lj = -cellSpan; lj <= cellSpan; ++lj) {
            for(Int li = -cellSpan; li <= cellSpan; ++li) {
                const auto neighborCellIdx = cellIdx + Vec2i(li, lj);

                if(!isValidCell(neighborCellIdx)) {
                    continue;
                }

                const auto& cell = m_ParticleIdxInCell(neighborCellIdx);
                if(cell.size() > 0) {
                    for(UInt q : cell) {
                        const auto pqd2 = glm::length2(ppos - positions[q]);
                        if(pqd2 > 0 && pqd2 < d2) {
                            neighborList.push_back(q);
                        }
                    }
                }
            }
        } // end loop over neighbor cells
    } else {
        Vec3i cellIdx = getCellIdx<Int>(ppos);
        for(Int lk = -cellSpan; lk <= cellSpan; ++lk) {
            for(Int lj = -cellSpan; lj <= cellSpan; ++lj) {
                for(Int li = -cellSpan; li <= cellSpan; ++li) {
                    const auto neighborCellIdx = cellIdx + Vec3i(li, lj, lk);

                    if(!isValidCell(neighborCellIdx)) {
                        continue;
                    }

                    const auto& cell = m_ParticleIdxInCell(neighborCellIdx);

                    if(cell.size() > 0) {
                        for(UInt q : cell) {
                            const auto pqd2 = glm::length2(ppos - positions[q]);
                            if(pqd2 > 0 && pqd2 < d2) {
                                neighborList.push_back(q);
                            }
                        }
                    }
                }
            }
        } // end loop over neighbor cells
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Grid<N, RealType>::sortData(Vector<VecX<N, RealType>>& data) {
    const auto& sortedIdx = getParticleIdxSortedByCell();
    assert(sortedIdx.size() == data.size());

    Vector<VecX<N, RealType>> tmp(data.begin(), data.end());
    std::transform(sortedIdx.cbegin(), sortedIdx.cend(),
#ifdef _MSC_VER
                   stdext::unchecked_array_iterator<VecX<N, RealType>*>(data.data()),
#else
                   data.begin(),
#endif
                   [&](UInt i) { return tmp[i]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
const Vec_UInt& Grid<N, RealType>::getParticleIdxSortedByCell() {
    if(m_ParticleIdxSortedByCell.size() > 0) {
        return m_ParticleIdxSortedByCell;
    }
    for(auto& cell : m_ParticleIdxInCell.data()) {
        if(cell.size() > 0) {
            m_ParticleIdxSortedByCell.insert(m_ParticleIdxSortedByCell.end(), cell.begin(), cell.end());
        }
    }
    return m_ParticleIdxSortedByCell;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template class Grid<2, Real>;
template class Grid<3, Real>;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
