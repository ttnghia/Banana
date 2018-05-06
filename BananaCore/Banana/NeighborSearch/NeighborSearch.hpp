//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::NeighborSearch
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
NeighborSearch<N, RealType>::NeighborSearch(RealType r, bool erase_empty_cells) :
    m_r2(r * r), m_inv_cell_size(static_cast<RealType>(1.0 / r)), m_erase_empty_cells(erase_empty_cells), m_initialized(false)
{
    if(r <= 0.0) {
        std::cerr << "WARNING: Neighborhood search may not be initialized with a zero or negative search radius."
                  << " This may lead to unexpected behavior." << std::endl;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Computes index to a world space position x.
template<Int N, class RealType>
HashKey<N> NeighborSearch<N, RealType>::cell_index(const RealType* x) const
{
    HashKey<N> ret;
    for(Int d = 0; d < N; ++d) {
        RealType tmp = x[d] - RealType(SHIFT_POSITION);
        ret.k[d] = tmp >= 0 ? static_cast<int>(m_inv_cell_size * tmp) : static_cast<int>(m_inv_cell_size * tmp) - 1;
    }
    return ret;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Determines Morten value according to z-curve.
template<Int N, class RealType>
uint_fast64_t NeighborSearch<N, RealType>::z_value(const HashKey<N>& key)
{
    if constexpr(N == 2) {
        return morton2D_64_encode(static_cast<uint_fast32_t>(static_cast<int64_t>(key.k[0]) - (std::numeric_limits<int>::lowest() + 1)),
                                  static_cast<uint_fast32_t>(static_cast<int64_t>(key.k[1]) - (std::numeric_limits<int>::lowest() + 1)));
    } else {
        return morton3D_64_encode(static_cast<uint_fast32_t>(static_cast<int64_t>(key.k[0]) - (std::numeric_limits<int>::lowest() + 1)),
                                  static_cast<uint_fast32_t>(static_cast<int64_t>(key.k[1]) - (std::numeric_limits<int>::lowest() + 1)),
                                  static_cast<uint_fast32_t>(static_cast<int64_t>(key.k[2]) - (std::numeric_limits<int>::lowest() + 1)));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Determines permutation table for point array.
template<Int N, class RealType>
void NeighborSearch<N, RealType>::z_sort()
{
    for(PointSet<N, RealType>& d : m_point_sets) {
        d.m_sort_table.resize(d.n_points());
        std::iota(d.m_sort_table.begin(), d.m_sort_table.end(), 0);

        //std::sort(d.m_sort_table.begin(), d.m_sort_table.end(),
        tbb::parallel_sort(d.m_sort_table.begin(), d.m_sort_table.end(),
                           [&](UInt a, UInt b)
                           {
                               return z_value(cell_index(d.point(a))) < z_value(cell_index(d.point(b)));
                           });
    }
    m_initialized = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Build hash table and entry array from scratch.
template<Int N, class RealType>
void NeighborSearch<N, RealType>::init()
{
    m_entries.clear();
    m_map.clear();

    // Determine existing entries.
    Vector<HashKey<N>> temp_keys;
    for(UInt j = 0, jend = static_cast<UInt>(m_point_sets.size()); j < jend; ++j) {
        PointSet<N, RealType>& d = m_point_sets[j];
        d.m_locks.resize(m_point_sets.size());
        for(auto& l : d.m_locks) {
            l.resize(d.n_points());
        }

        for(UInt i = 0; i < d.n_points(); ++i) {
            const HashKey<N>& key = cell_index(d.point(i));
            d.m_keys[i] = d.m_old_keys[i] = key;

            auto it = m_map.find(key);
            if(it == m_map.end()) {
                m_entries.push_back({{ j, i } });
                if(m_activation_table.is_searching_neighbors(j)) {
                    m_entries.back().n_searching_points++;
                }
                temp_keys.push_back(key);
                m_map[key] = static_cast<UInt>(m_entries.size() - 1);
            } else {
                m_entries[it->second].add({ j, i });
                if(m_activation_table.is_searching_neighbors(j)) {
                    m_entries[it->second].n_searching_points++;
                }
            }
        }
    }

    m_map.clear();
    for(UInt i = 0; i < m_entries.size(); ++i) {
        m_map.emplace(temp_keys[i], i);
    }

    m_initialized = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void NeighborSearch<N, RealType>::resize_point_set(UInt index, const RealType* x, UInt size)
{
    PointSet<N, RealType>& point_set = m_point_sets[index];
    UInt                   old_size  = point_set.n_points();

    if(!m_initialized) {
        throw NeighborhoodSearchNotInitialized {};
    }

    // Delete old entries. (Shrink)
    if(old_size > size) {
        Vec_UInt to_delete;
        if(m_erase_empty_cells) {
            to_delete.reserve(m_entries.size());
        }

        for(UInt i = size; i < old_size; ++i) {
            const HashKey<N>& key = point_set.m_keys[i];
            auto              it  = m_map.find(key);
            m_entries[it->second].erase({ index, i });

            if(m_activation_table.is_searching_neighbors(index)) {
                m_entries[it->second].n_searching_points--;
            }

            if(m_erase_empty_cells) {
                if(m_entries[it->second].n_indices() == 0) {
                    to_delete.push_back(it->second);
                }
            }
        }
        if(m_erase_empty_cells) {
            erase_empty_entries(to_delete);
        }
    }

    point_set.resize(x, size);

    // Insert new entries. (Grow)
    for(UInt i = old_size; i < point_set.n_points(); ++i) {
        HashKey<N> key = cell_index(point_set.point(i));
        point_set.m_keys[i] = point_set.m_old_keys[i] = key;
        auto it = m_map.find(key);
        if(it == m_map.end()) {
            m_entries.push_back({{ index, i } });
            if(m_activation_table.is_searching_neighbors(index)) {
                m_entries.back().n_searching_points++;
            }
            m_map[key] = static_cast<UInt>(m_entries.size() - 1);
        } else {
            m_entries[it->second].add({ index, i });
            if(m_activation_table.is_searching_neighbors(index)) {
                m_entries.back().n_searching_points++;
            }
        }
    }

    // Resize spinlock arrays.
    for(auto& l : point_set.m_locks) {
        l.resize(point_set.n_points());
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void NeighborSearch<N, RealType>::update_activation_table()
{
    if(m_activation_table != m_old_activation_table) {
        for(auto& entry : m_entries) {
            auto& n = entry.n_searching_points;
            n = 0u;
            for(auto const& idx : entry.indices) {
                if(m_activation_table.is_searching_neighbors(idx.point_set_id)) {
                    ++n;
                }
            }
        }
        m_old_activation_table = m_activation_table;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void NeighborSearch<N, RealType>::find_neighbors(bool points_changed_)
{
    if(points_changed_) {
        update_point_sets();
    }
    update_activation_table();
    query();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void NeighborSearch<N, RealType>::update_point_sets()
{
    if(!m_initialized) {
        init();
        m_initialized = true;
    }

    // Pre-compute cell indices.
    for(PointSet<N, RealType>& d : m_point_sets) {
        if(!d.is_dynamic()) {
            continue;
        }
        d.m_keys.swap(d.m_old_keys);

        for(UInt i = 0; i < d.n_points(); ++i) {
            d.m_keys[i] = cell_index(d.point(i));
        }
    }

    Vec_UInt to_delete;
    if(m_erase_empty_cells) {
        to_delete.reserve(m_entries.size());
    }

    update_hash_table(to_delete);

    if(m_erase_empty_cells) {
        erase_empty_entries(to_delete);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void NeighborSearch<N, RealType>::find_neighbors(UInt point_set_id, UInt point_index, Vec_VecUInt& neighbors)
{
    query(point_set_id, point_index, neighbors);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void NeighborSearch<N, RealType>::erase_empty_entries(const Vec_UInt& to_delete)
{
    if(to_delete.empty()) {
        return;
    }

    // Indicated empty cells.
    m_entries.erase(std::remove_if(m_entries.begin(), m_entries.end(), [](HashEntry const& entry) { return entry.indices.empty(); }), m_entries.end());

    {
        auto it = m_map.begin();
        while(it != m_map.end()) {
            auto& kvp = *it;

            if(kvp.second <= to_delete.front() && kvp.second >= to_delete.back() &&
               std::binary_search(to_delete.rbegin(), to_delete.rend(), kvp.second)) {
                it = m_map.erase(it);
            } else {
                ++it;
            }
        }
    }

    Vector<std::pair<const HashKey<N>, UInt>*> kvps(m_map.size());
    std::transform(m_map.begin(), m_map.end(), kvps.begin(), [](std::pair<const HashKey<N>, UInt>& kvp) { return &kvp; });

    Scheduler::parallel_for(kvps.size(),
                            [&](size_t it)
                            {
                                std::pair<const HashKey<N>, UInt>* kvp_ = kvps[it];
                                auto& kvp                               = *kvp_;

                                for(UInt i = 0; i < to_delete.size(); ++i) {
                                    if(kvp.second >= to_delete[i]) {
                                        kvp.second -= static_cast<UInt>(to_delete.size() - i);
                                        break;
                                    }
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void NeighborSearch<N, RealType>::update_hash_table(Vec_UInt& to_delete)
{
    // Indicate points changing inheriting cell.
    for(UInt j = 0; j < m_point_sets.size(); ++j) {
        PointSet<N, RealType>& d = m_point_sets[j];
        for(UInt i = 0; i < d.n_points(); ++i) {
            if(d.m_keys[i] == d.m_old_keys[i]) {
                continue;
            }

            const HashKey<N>& key = d.m_keys[i];
            auto              it  = m_map.find(key);
            if(it == m_map.end()) {
                m_entries.push_back({{ j, i } });
                if(m_activation_table.is_searching_neighbors(j)) {
                    m_entries.back().n_searching_points++;
                }
                m_map.insert({ key, static_cast<UInt>(m_entries.size() - 1) });
            } else {
                HashEntry& entry = m_entries[it->second];
                entry.add({ j, i });
                if(m_activation_table.is_searching_neighbors(j)) {
                    entry.n_searching_points++;
                }
            }

            UInt entry_index = m_map[d.m_old_keys[i]];
            m_entries[entry_index].erase({ j, i });
            if(m_activation_table.is_searching_neighbors(j)) {
                m_entries[entry_index].n_searching_points--;
            }
            if(m_erase_empty_cells) {
                if(m_entries[entry_index].n_indices() == 0) {
                    to_delete.push_back(entry_index);
                }
            }
        }
    }

    to_delete.erase(std::remove_if(to_delete.begin(), to_delete.end(), [&](UInt index) { return m_entries[index].n_indices() != 0; }), to_delete.end());
    std::sort(to_delete.begin(), to_delete.end(), std::greater<UInt>());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void NeighborSearch<N, RealType>::query()
{
    if constexpr(N == 2) {
        query2D();
    } else {
        query3D();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void NeighborSearch<N, RealType>::query2D()
{
    for(UInt i = 0, iend = static_cast<UInt>(m_point_sets.size()); i < iend; ++i) {
        PointSet<N, RealType>& d = m_point_sets[i];
        d.m_neighbors.resize(m_point_sets.size());
        for(UInt j = 0, jend = static_cast<UInt>(d.m_neighbors.size()); j < jend; ++j) {
            auto& n = d.m_neighbors[j];
            n.resize(d.n_points());
            for(auto& n_ : n) {
                n_.clear();
                if(m_activation_table.is_active(i, j)) {
                    n_.reserve(INITIAL_NUMBER_OF_NEIGHBORS);
                }
            }
        }
    }

    Vector<const std::pair<const HashKey<N>, UInt>*> kvps(m_map.size());
    std::transform(m_map.begin(), m_map.end(), kvps.begin(), [](std::pair<const HashKey<N>, UInt> const& kvp) { return &kvp; });

    // Perform neighborhood search.
    Scheduler::parallel_for(kvps.size(),
                            [&](size_t it)
                            {
                                std::pair<const HashKey<N>, UInt> const* kvp_ = kvps[it];
                                auto const& kvp                               = *kvp_;
                                HashEntry const& entry                        = m_entries[kvp.second];
                                //const HashKey<N>& key                         = kvp.first;

                                if(entry.n_searching_points == 0u) {
                                    return;
                                }

                                for(UInt a = 0; a < entry.n_indices(); ++a) {
                                    const PointID& va         = entry.indices[a];
                                    PointSet<N, RealType>& da = m_point_sets[va.point_set_id];
                                    for(UInt b = a + 1; b < entry.n_indices(); ++b) {
                                        const PointID& vb         = entry.indices[b];
                                        PointSet<N, RealType>& db = m_point_sets[vb.point_set_id];

                                        if(!m_activation_table.is_active(va.point_set_id, vb.point_set_id) &&
                                           !m_activation_table.is_active(vb.point_set_id, va.point_set_id)) {
                                            continue;
                                        }

                                        const RealType* xa = da.point(va.point_id);
                                        const RealType* xb = db.point(vb.point_id);

                                        auto tmp0 = xa[0] - xb[0];
                                        auto tmp1 = xa[1] - xb[1];
                                        auto l2   = tmp0 * tmp0 + tmp1 * tmp1;

                                        if(l2 < m_r2) {
                                            if(m_activation_table.is_active(va.point_set_id, vb.point_set_id)) {
                                                da.m_neighbors[vb.point_set_id][va.point_id].push_back(vb.point_id);
                                            }
                                            if(m_activation_table.is_active(vb.point_set_id, va.point_set_id)) {
                                                db.m_neighbors[va.point_set_id][vb.point_id].push_back(va.point_id);
                                            }
                                        }
                                    }
                                }
                            });

    Vector<std::array<bool, 9>>       visited(m_entries.size(), { false });
    Vector<ParallelObjects::SpinLock> entry_locks(m_entries.size());

    Scheduler::parallel_for(kvps.size(),
                            [&](size_t it)
                            {
                                std::pair<const HashKey<N>, UInt> const* kvp_ = kvps[it];
                                auto const& kvp                               = *kvp_;
                                HashEntry const& entry                        = m_entries[kvp.second];

                                if(entry.n_searching_points == 0u) {
                                    return;
                                }
                                const HashKey<N>& key = kvp.first;

                                for(int dk = -1; dk <= 1; dk++) {
                                    for(int dl = -1; dl <= 1; dl++) {
                                        int l_ind = 3 * (dk + 1) + (dl + 1);
                                        if(l_ind == 4) {
                                            continue;
                                        }
                                        entry_locks[kvp.second].lock();
                                        if(visited[kvp.second][l_ind]) {
                                            entry_locks[kvp.second].unlock();
                                            continue;
                                        }
                                        entry_locks[kvp.second].unlock();

                                        auto it = m_map.find({ key.k[0] + dk, key.k[1] + dl });
                                        if(it == m_map.end()) {
                                            continue;
                                        }

                                        std::array<UInt, 2> entry_ids {{ kvp.second, it->second } };
                                        if(entry_ids[0] > entry_ids[1]) {
                                            std::swap(entry_ids[0], entry_ids[1]);
                                        }
                                        entry_locks[entry_ids[0]].lock();
                                        entry_locks[entry_ids[1]].lock();

                                        if(visited[kvp.second][l_ind]) {
                                            entry_locks[entry_ids[1]].unlock();
                                            entry_locks[entry_ids[0]].unlock();
                                            continue;
                                        }

                                        visited[kvp.second][l_ind]     = true;
                                        visited[it->second][8 - l_ind] = true;

                                        entry_locks[entry_ids[1]].unlock();
                                        entry_locks[entry_ids[0]].unlock();

                                        for(UInt i = 0; i < entry.n_indices(); ++i) {
                                            const PointID& va       = entry.indices[i];
                                            HashEntry const& entry_ = m_entries[it->second];
                                            UInt n_ind              = entry_.n_indices();
                                            for(UInt j = 0; j < n_ind; ++j) {
                                                const PointID& vb         = entry_.indices[j];
                                                PointSet<N, RealType>& db = m_point_sets[vb.point_set_id];

                                                PointSet<N, RealType>& da = m_point_sets[va.point_set_id];

                                                if(!m_activation_table.is_active(va.point_set_id, vb.point_set_id) &&
                                                   !m_activation_table.is_active(vb.point_set_id, va.point_set_id)) {
                                                    continue;
                                                }

                                                const RealType* xa = da.point(va.point_id);
                                                const RealType* xb = db.point(vb.point_id);

                                                auto tmp0 = xa[0] - xb[0];
                                                auto tmp1 = xa[1] - xb[1];
                                                auto l2   = tmp0 * tmp0 + tmp1 * tmp1;

                                                if(l2 < m_r2) {
                                                    if(m_activation_table.is_active(va.point_set_id, vb.point_set_id)) {
                                                        da.m_locks[vb.point_set_id][va.point_id].lock();
                                                        da.m_neighbors[vb.point_set_id][va.point_id].push_back(vb.point_id);
                                                        da.m_locks[vb.point_set_id][va.point_id].unlock();
                                                    }
                                                    if(m_activation_table.is_active(vb.point_set_id, va.point_set_id)) {
                                                        db.m_locks[va.point_set_id][vb.point_id].lock();
                                                        db.m_neighbors[va.point_set_id][vb.point_id].push_back(va.point_id);
                                                        db.m_locks[va.point_set_id][vb.point_id].unlock();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void NeighborSearch<N, RealType>::query3D()
{
    for(UInt i = 0, iend = static_cast<UInt>(m_point_sets.size()); i < iend; ++i) {
        PointSet<N, RealType>& d = m_point_sets[i];
        d.m_neighbors.resize(m_point_sets.size());

        for(UInt j = 0, jend = static_cast<UInt>(d.m_neighbors.size()); j < jend; ++j) {
            auto& n = d.m_neighbors[j];
            n.resize(d.n_points());
            for(auto& n_ : n) {
                n_.clear();
                if(m_activation_table.is_active(i, j)) {
                    n_.reserve(INITIAL_NUMBER_OF_NEIGHBORS);
                }
            }
        }
    }

    Vector<const std::pair<const HashKey<N>, UInt>*> kvps(m_map.size());
    std::transform(m_map.begin(), m_map.end(), kvps.begin(), [](std::pair<const HashKey<N>, UInt> const& kvp) { return &kvp; });

    // Perform neighborhood search.
    Scheduler::parallel_for(kvps.size(),
                            [&](size_t it)
                            {
                                std::pair<const HashKey<N>, UInt> const* kvp_ = kvps[it];
                                auto const& kvp                               = *kvp_;
                                HashEntry const& entry                        = m_entries[kvp.second];
                                //const HashKey<N>& key                         = kvp.first;

                                if(entry.n_searching_points == 0u) {
                                    return;
                                }

                                for(UInt a = 0; a < entry.n_indices(); ++a) {
                                    const PointID& va         = entry.indices[a];
                                    PointSet<N, RealType>& da = m_point_sets[va.point_set_id];
                                    for(UInt b = a + 1; b < entry.n_indices(); ++b) {
                                        const PointID& vb         = entry.indices[b];
                                        PointSet<N, RealType>& db = m_point_sets[vb.point_set_id];

                                        if(!m_activation_table.is_active(va.point_set_id, vb.point_set_id) &&
                                           !m_activation_table.is_active(vb.point_set_id, va.point_set_id)) {
                                            continue;
                                        }

                                        const RealType* xa = da.point(va.point_id);
                                        const RealType* xb = db.point(vb.point_id);

                                        auto tmp0 = xa[0] - xb[0];
                                        auto tmp1 = xa[1] - xb[1];
                                        auto tmp2 = xa[2] - xb[2];
                                        auto l2   = tmp0 * tmp0 + tmp1 * tmp1 + tmp2 * tmp2;

                                        if(l2 < m_r2) {
                                            if(m_activation_table.is_active(va.point_set_id, vb.point_set_id)) {
                                                da.m_neighbors[vb.point_set_id][va.point_id].push_back(vb.point_id);
                                            }
                                            if(m_activation_table.is_active(vb.point_set_id, va.point_set_id)) {
                                                db.m_neighbors[va.point_set_id][vb.point_id].push_back(va.point_id);
                                            }
                                        }
                                    }
                                }
                            });

    Vector<std::array<bool, 27>>      visited(m_entries.size(), { false });
    Vector<ParallelObjects::SpinLock> entry_locks(m_entries.size());

    Scheduler::parallel_for(kvps.size(),
                            [&](size_t it)
                            {
                                std::pair<const HashKey<N>, UInt> const* kvp_ = kvps[it];
                                auto const& kvp                               = *kvp_;
                                HashEntry const& entry                        = m_entries[kvp.second];

                                if(entry.n_searching_points == 0u) {
                                    return;
                                }
                                const HashKey<N>& key = kvp.first;

                                for(int dj = -1; dj <= 1; dj++) {
                                    for(int dk = -1; dk <= 1; dk++) {
                                        for(int dl = -1; dl <= 1; dl++) {
                                            int l_ind = 9 * (dj + 1) + 3 * (dk + 1) + (dl + 1);
                                            if(l_ind == 13) {
                                                continue;
                                            }
                                            entry_locks[kvp.second].lock();
                                            if(visited[kvp.second][l_ind]) {
                                                entry_locks[kvp.second].unlock();
                                                continue;
                                            }
                                            entry_locks[kvp.second].unlock();

                                            auto it = m_map.find({ key.k[0] + dj, key.k[1] + dk, key.k[2] + dl });
                                            if(it == m_map.end()) {
                                                continue;
                                            }

                                            std::array<UInt, 2> entry_ids {{ kvp.second, it->second } };
                                            if(entry_ids[0] > entry_ids[1]) {
                                                std::swap(entry_ids[0], entry_ids[1]);
                                            }
                                            entry_locks[entry_ids[0]].lock();
                                            entry_locks[entry_ids[1]].lock();

                                            if(visited[kvp.second][l_ind]) {
                                                entry_locks[entry_ids[1]].unlock();
                                                entry_locks[entry_ids[0]].unlock();
                                                continue;
                                            }

                                            visited[kvp.second][l_ind]      = true;
                                            visited[it->second][26 - l_ind] = true;

                                            entry_locks[entry_ids[1]].unlock();
                                            entry_locks[entry_ids[0]].unlock();

                                            for(UInt i = 0; i < entry.n_indices(); ++i) {
                                                const PointID& va       = entry.indices[i];
                                                HashEntry const& entry_ = m_entries[it->second];
                                                UInt n_ind              = entry_.n_indices();
                                                for(UInt j = 0; j < n_ind; ++j) {
                                                    const PointID& vb         = entry_.indices[j];
                                                    PointSet<N, RealType>& db = m_point_sets[vb.point_set_id];

                                                    PointSet<N, RealType>& da = m_point_sets[va.point_set_id];

                                                    if(!m_activation_table.is_active(va.point_set_id, vb.point_set_id) &&
                                                       !m_activation_table.is_active(vb.point_set_id, va.point_set_id)) {
                                                        continue;
                                                    }

                                                    const RealType* xa = da.point(va.point_id);
                                                    const RealType* xb = db.point(vb.point_id);

                                                    auto tmp0 = xa[0] - xb[0];
                                                    auto tmp1 = xa[1] - xb[1];
                                                    auto tmp2 = xa[2] - xb[2];
                                                    auto l2   = tmp0 * tmp0 + tmp1 * tmp1 + tmp2 * tmp2;

                                                    if(l2 < m_r2) {
                                                        if(m_activation_table.is_active(va.point_set_id, vb.point_set_id)) {
                                                            da.m_locks[vb.point_set_id][va.point_id].lock();
                                                            da.m_neighbors[vb.point_set_id][va.point_id].push_back(vb.point_id);
                                                            da.m_locks[vb.point_set_id][va.point_id].unlock();
                                                        }
                                                        if(m_activation_table.is_active(vb.point_set_id, va.point_set_id)) {
                                                            db.m_locks[va.point_set_id][vb.point_id].lock();
                                                            db.m_neighbors[va.point_set_id][vb.point_id].push_back(va.point_id);
                                                            db.m_locks[va.point_set_id][vb.point_id].unlock();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void NeighborSearch<N, RealType>::query(UInt point_set_id, UInt point_index, Vec_VecUInt& neighbors)
{
    if constexpr(N == 2) {
        query2D(point_set_id, point_index, neighbors);
    } else {
        query3D(point_set_id, point_index, neighbors);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void NeighborSearch<N, RealType>::query2D(UInt point_set_id, UInt point_index, Vec_VecUInt& neighbors)
{
    neighbors.resize(m_point_sets.size());
    PointSet<N, RealType>& d = m_point_sets[point_set_id];
    for(UInt j = 0; j < m_point_sets.size(); j++) {
        auto& n = neighbors[j];
        n.clear();
        if(m_activation_table.is_active(point_set_id, j)) {
            n.reserve(INITIAL_NUMBER_OF_NEIGHBORS);
        }
    }

    const RealType* xa       = d.point(point_index);
    HashKey<N>      hash_key = cell_index(xa);

    auto                               it    = m_map.find(hash_key);
    std::pair<const HashKey<N>, UInt>& kvp   = *it;
    HashEntry const&                   entry = m_entries[kvp.second];

    // Perform neighborhood search.
    for(UInt b = 0; b < entry.n_indices(); ++b) {
        const PointID& vb = entry.indices[b];
        if((point_set_id != vb.point_set_id) || (point_index != vb.point_id)) {
            if(!m_activation_table.is_active(point_set_id, vb.point_set_id)) {
                continue;
            }

            PointSet<N, RealType>& db = m_point_sets[vb.point_set_id];
            const RealType*        xb = db.point(vb.point_id);

            auto tmp0 = xa[0] - xb[0];
            auto tmp1 = xa[1] - xb[1];
            auto l2   = tmp0 * tmp0 + tmp1 * tmp1;

            if(l2 < m_r2) {
                neighbors[vb.point_set_id].push_back(vb.point_id);
            }
        }
    }

    for(int dk = -1; dk <= 1; dk++) {
        for(int dl = -1; dl <= 1; dl++) {
            int l_ind = 3 * (dk + 1) + (dl + 1);
            if(l_ind == 4) {
                continue;
            }

            auto it = m_map.find({ hash_key.k[0] + dk, hash_key.k[1] + dl });
            if(it == m_map.end()) {
                continue;
            }

            std::array<UInt, 2> entry_ids {{ kvp.second, it->second } };
            if(entry_ids[0] > entry_ids[1]) {
                std::swap(entry_ids[0], entry_ids[1]);
            }

            HashEntry const& entry_ = m_entries[it->second];
            UInt             n_ind  = entry_.n_indices();
            for(UInt j = 0; j < n_ind; ++j) {
                const PointID& vb = entry_.indices[j];
                if(!m_activation_table.is_active(point_set_id, vb.point_set_id)) {
                    continue;
                }
                PointSet<N, RealType>& db = m_point_sets[vb.point_set_id];

                const RealType* xb = db.point(vb.point_id);

                auto tmp0 = xa[0] - xb[0];
                auto tmp1 = xa[1] - xb[1];
                auto l2   = tmp0 * tmp0 + tmp1 * tmp1;

                if(l2 < m_r2) {
                    neighbors[vb.point_set_id].push_back(vb.point_id);
                }
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void NeighborSearch<N, RealType>::query3D(UInt point_set_id, UInt point_index, Vec_VecUInt& neighbors)
{
    neighbors.resize(m_point_sets.size());
    PointSet<N, RealType>& d = m_point_sets[point_set_id];
    for(UInt j = 0; j < m_point_sets.size(); j++) {
        auto& n = neighbors[j];
        n.clear();
        if(m_activation_table.is_active(point_set_id, j)) {
            n.reserve(INITIAL_NUMBER_OF_NEIGHBORS);
        }
    }

    const RealType* xa       = d.point(point_index);
    HashKey<N>      hash_key = cell_index(xa);

    auto                               it    = m_map.find(hash_key);
    std::pair<const HashKey<N>, UInt>& kvp   = *it;
    HashEntry const&                   entry = m_entries[kvp.second];

    // Perform neighborhood search.
    for(UInt b = 0; b < entry.n_indices(); ++b) {
        const PointID& vb = entry.indices[b];
        if((point_set_id != vb.point_set_id) || (point_index != vb.point_id)) {
            if(!m_activation_table.is_active(point_set_id, vb.point_set_id)) {
                continue;
            }

            PointSet<N, RealType>& db = m_point_sets[vb.point_set_id];
            const RealType*        xb = db.point(vb.point_id);

            auto tmp0 = xa[0] - xb[0];
            auto tmp1 = xa[1] - xb[1];
            auto tmp2 = xa[2] - xb[2];
            auto l2   = tmp0 * tmp0 + tmp1 * tmp1 + tmp2 * tmp2;

            if(l2 < m_r2) {
                neighbors[vb.point_set_id].push_back(vb.point_id);
            }
        }
    }

    for(int dj = -1; dj <= 1; dj++) {
        for(int dk = -1; dk <= 1; dk++) {
            for(int dl = -1; dl <= 1; dl++) {
                int l_ind = 9 * (dj + 1) + 3 * (dk + 1) + (dl + 1);
                if(l_ind == 13) {
                    continue;
                }

                auto it = m_map.find({ hash_key.k[0] + dj, hash_key.k[1] + dk, hash_key.k[2] + dl });
                if(it == m_map.end()) {
                    continue;
                }

                std::array<UInt, 2> entry_ids {{ kvp.second, it->second } };
                if(entry_ids[0] > entry_ids[1]) {
                    std::swap(entry_ids[0], entry_ids[1]);
                }

                HashEntry const& entry_ = m_entries[it->second];
                UInt             n_ind  = entry_.n_indices();
                for(UInt j = 0; j < n_ind; ++j) {
                    const PointID& vb = entry_.indices[j];
                    if(!m_activation_table.is_active(point_set_id, vb.point_set_id)) {
                        continue;
                    }
                    PointSet<N, RealType>& db = m_point_sets[vb.point_set_id];

                    const RealType* xb = db.point(vb.point_id);

                    auto tmp0 = xa[0] - xb[0];
                    auto tmp1 = xa[1] - xb[1];
                    auto tmp2 = xa[2] - xb[2];
                    auto l2   = tmp0 * tmp0 + tmp1 * tmp1 + tmp2 * tmp2;

                    if(l2 < m_r2) {
                        neighbors[vb.point_set_id].push_back(vb.point_id);
                    }
                }
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::NeighborSearch
