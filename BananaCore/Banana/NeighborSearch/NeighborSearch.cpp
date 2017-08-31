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

#include <Banana/NeighborSearch/NeighborSearch.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
NeighborSearch::NeighborSearch(Real r, bool erase_empty_cells) :
    m_r2(r * r), m_inv_cell_size(static_cast<Real>(1.0 / r)), m_erase_empty_cells(erase_empty_cells), m_initialized(false)
{
    if(r <= 0.0)
    {
        std::cerr << "WARNING: Neighborhood search may not be initialized with a zero or negative search radius."
                  << " This may lead to unexpected behavior." << std::endl;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Computes triple index to a world space position x.
HashKey NeighborSearch::cell_index(Real const* x) const
{
    HashKey ret;
    for(UInt i = 0; i < 3; ++i)
    {
        if(x[i] >= 0.0)
            ret.k[i] = static_cast<int>(m_inv_cell_size * x[i]);
        else
            ret.k[i] = static_cast<int>(m_inv_cell_size * x[i]) - 1;
    }
    return ret;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Determines permutation table for point array.
void NeighborSearch::z_sort()
{
    for(PointSet& d : m_point_sets)
    {
        d.m_sort_table.resize(d.n_points());
        std::iota(d.m_sort_table.begin(), d.m_sort_table.end(), 0);

        std::sort(d.m_sort_table.begin(), d.m_sort_table.end(),
                  [&](UInt a, UInt b)
                  {
                      return z_value(cell_index(d.point(a))) < z_value(cell_index(d.point(b)));
                  });
    }

    m_initialized = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Build hash table and entry array from scratch.
void NeighborSearch::init()
{
    m_entries.clear();
    m_map.clear();

    // Determine existing entries.
    Vector<HashKey> temp_keys;
    for(UInt j = 0; j < m_point_sets.size(); ++j)
    {
        PointSet& d = m_point_sets[j];
        d.m_locks.resize(m_point_sets.size());
        for(auto& l : d.m_locks)
            l.resize(d.n_points());

        for(UInt i = 0; i < d.n_points(); i++)
        {
            HashKey const& key = cell_index(d.point(i));
            d.m_keys[i] = d.m_old_keys[i] = key;

            auto it = m_map.find(key);
            if(it == m_map.end())
            {
                m_entries.push_back({ { j, i } });
                if(m_activation_table.is_searching_neighbors(j))
                    m_entries.back().n_searching_points++;
                temp_keys.push_back(key);
                m_map[key] = static_cast<UInt>(m_entries.size() - 1);
            }
            else
            {
                m_entries[it->second].add({ j, i });
                if(m_activation_table.is_searching_neighbors(j))
                    m_entries[it->second].n_searching_points++;
            }
        }
    }

    m_map.clear();
    for(UInt i = 0; i < m_entries.size(); ++i)
    {
        m_map.emplace(temp_keys[i], i);
    }

    m_initialized = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void NeighborSearch::resize_point_set(UInt index, Real const* x, std::size_t size)
{
    PointSet&   point_set = m_point_sets[index];
    std::size_t old_size  = point_set.n_points();

    if(!m_initialized)
    {
        throw NeighborhoodSearchNotInitialized {};
    }

    // Delete old entries. (Shrink)
    if(old_size > size)
    {
        Vec_UInt to_delete;
        if(m_erase_empty_cells)
        {
            to_delete.reserve(m_entries.size());
        }

        for(UInt i = static_cast<UInt>(size); i < old_size; i++)
        {
            HashKey const& key = point_set.m_keys[i];
            auto           it  = m_map.find(key);
            m_entries[it->second].erase({ index, i });

            if(m_activation_table.is_searching_neighbors(index))
                m_entries[it->second].n_searching_points--;

            if(m_erase_empty_cells)
            {
                if(m_entries[it->second].n_indices() == 0)
                {
                    to_delete.push_back(it->second);
                }
            }
        }
        if(m_erase_empty_cells)
        {
            erase_empty_entries(to_delete);
        }
    }

    point_set.resize(x, size);

    // Insert new entries. (Grow)
    for(UInt i = static_cast<UInt>(old_size); i < point_set.n_points(); i++)
    {
        HashKey key = cell_index(point_set.point(i));
        point_set.m_keys[i] = point_set.m_old_keys[i] = key;
        auto it = m_map.find(key);
        if(it == m_map.end())
        {
            m_entries.push_back({ { index, i } });
            if(m_activation_table.is_searching_neighbors(index))
                m_entries.back().n_searching_points++;
            m_map[key] = static_cast<UInt>(m_entries.size() - 1);
        }
        else
        {
            m_entries[it->second].add({ index, i });
            if(m_activation_table.is_searching_neighbors(index))
                m_entries.back().n_searching_points++;
        }
    }

    // Resize spinlock arrays.
    for(auto& l : point_set.m_locks)
        l.resize(point_set.n_points());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void NeighborSearch::update_activation_table()
{
    if(m_activation_table != m_old_activation_table)
    {
        for(auto& entry : m_entries)
        {
            auto& n = entry.n_searching_points;
            n = 0u;
            for(auto const& idx : entry.indices)
            {
                if(m_activation_table.is_searching_neighbors(idx.point_set_id))
                    ++n;
            }
        }
        m_old_activation_table = m_activation_table;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void NeighborSearch::find_neighbors(bool points_changed_)
{
    if(points_changed_)
    {
        update_point_sets();
    }
    update_activation_table();
    query();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void NeighborSearch::update_point_sets()
{
    if(!m_initialized)
    {
        init();
        m_initialized = true;
    }

    // Precompute cell indices.
    for(PointSet& d : m_point_sets)
    {
        if(!d.is_dynamic())
            continue;
        d.m_keys.swap(d.m_old_keys);

        for(UInt i = 0; i < d.n_points(); ++i)
        {
            d.m_keys[i] = cell_index(d.point(i));
        }
    }

    Vec_UInt to_delete;
    if(m_erase_empty_cells)
    {
        to_delete.reserve(m_entries.size());
    }

    update_hash_table(to_delete);

    if(m_erase_empty_cells)
    {
        erase_empty_entries(to_delete);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void NeighborSearch::find_neighbors(UInt point_set_id, UInt point_index, Vec_VecUInt& neighbors)
{
    query(point_set_id, point_index, neighbors);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void NeighborSearch::erase_empty_entries(Vec_UInt const& to_delete)
{
    if(to_delete.empty())
        return;

    // Indicated empty cells.
    m_entries.erase(std::remove_if(m_entries.begin(), m_entries.end(), [](HashEntry const& entry) { return entry.indices.empty(); }), m_entries.end());

    {
        auto it = m_map.begin();
        while(it != m_map.end())
        {
            auto& kvp = *it;

            if(kvp.second <= to_delete.front() && kvp.second >= to_delete.back() &&
               std::binary_search(to_delete.rbegin(), to_delete.rend(), kvp.second))
            {
                it = m_map.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    Vector<std::pair<HashKey const, UInt>*> kvps(m_map.size());
    std::transform(m_map.begin(), m_map.end(), kvps.begin(), [](std::pair<HashKey const, UInt>& kvp) { return &kvp; });

    ParallelFuncs::parallel_for<size_t>(0, kvps.size(),
                                        [&](size_t it)
                                        {
                                            std::pair<HashKey const, UInt>* kvp_ = kvps[it];
                                            auto& kvp = *kvp_;

                                            for(UInt i = 0; i < to_delete.size(); ++i)
                                            {
                                                if(kvp.second >= to_delete[i])
                                                {
                                                    kvp.second -= static_cast<UInt>(to_delete.size() - i);
                                                    break;
                                                }
                                            }
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void NeighborSearch::update_hash_table(Vec_UInt& to_delete)
{
    // Indicate points changing inheriting cell.
    for(UInt j = 0; j < m_point_sets.size(); ++j)
    {
        PointSet& d = m_point_sets[j];
        for(UInt i = 0; i < d.n_points(); ++i)
        {
            if(d.m_keys[i] == d.m_old_keys[i])
                continue;

            HashKey const& key = d.m_keys[i];
            auto           it  = m_map.find(key);
            if(it == m_map.end())
            {
                m_entries.push_back({ { j, i } });
                if(m_activation_table.is_searching_neighbors(j))
                    m_entries.back().n_searching_points++;
                m_map.insert({ key, static_cast<UInt>(m_entries.size() - 1) });
            }
            else
            {
                HashEntry& entry = m_entries[it->second];
                entry.add({ j, i });
                if(m_activation_table.is_searching_neighbors(j))
                    entry.n_searching_points++;
            }

            UInt entry_index = m_map[d.m_old_keys[i]];
            m_entries[entry_index].erase({ j, i });
            if(m_activation_table.is_searching_neighbors(j))
                m_entries[entry_index].n_searching_points--;
            if(m_erase_empty_cells)
            {
                if(m_entries[entry_index].n_indices() == 0)
                {
                    to_delete.push_back(entry_index);
                }
            }
        }
    }

    to_delete.erase(std::remove_if(to_delete.begin(), to_delete.end(), [&](UInt index) { return m_entries[index].n_indices() != 0; }), to_delete.end());
    std::sort(to_delete.begin(), to_delete.end(), std::greater<UInt>());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void NeighborSearch::query()
{
    for(UInt i = 0; i < m_point_sets.size(); i++)
    {
        PointSet& d = m_point_sets[i];
        d.m_neighbors.resize(m_point_sets.size());
        for(UInt j = 0; j < d.m_neighbors.size(); j++)
        {
            auto& n = d.m_neighbors[j];
            n.resize(d.n_points());
            for(auto& n_ : n)
            {
                n_.clear();
                if(m_activation_table.is_active(i, j))
                    n_.reserve(INITIAL_NUMBER_OF_NEIGHBORS);
            }
        }
    }

    Vector<std::pair<HashKey const, UInt> const*> kvps(m_map.size());
    std::transform(m_map.begin(), m_map.end(), kvps.begin(), [](std::pair<HashKey const, UInt> const& kvp) { return &kvp; });

    // Perform neighborhood search.
    ParallelFuncs::parallel_for<size_t>(0, kvps.size(),
                                        [&](size_t it)
                                        {
                                            std::pair<HashKey const, UInt> const* kvp_ = kvps[it];
                                            auto const& kvp = *kvp_;
                                            HashEntry const& entry = m_entries[kvp.second];
//                                            HashKey const& key = kvp.first;

                                            if(entry.n_searching_points == 0u)
                                            {
                                                return;
                                            }

                                            for(UInt a = 0; a < entry.n_indices(); ++a)
                                            {
                                                PointID const& va = entry.indices[a];
                                                PointSet& da = m_point_sets[va.point_set_id];
                                                for(UInt b = a + 1; b < entry.n_indices(); ++b)
                                                {
                                                    PointID const& vb = entry.indices[b];
                                                    PointSet& db = m_point_sets[vb.point_set_id];

                                                    if(!m_activation_table.is_active(va.point_set_id, vb.point_set_id) &&
                                                       !m_activation_table.is_active(vb.point_set_id, va.point_set_id))
                                                    {
                                                        continue;
                                                    }

                                                    Real const* xa = da.point(va.point_id);
                                                    Real const* xb = db.point(vb.point_id);
                                                    Real tmp = xa[0] - xb[0];
                                                    Real l2 = tmp * tmp;
                                                    tmp = xa[1] - xb[1];
                                                    l2 += tmp * tmp;
                                                    tmp = xa[2] - xb[2];
                                                    l2 += tmp * tmp;

                                                    if(l2 < m_r2)
                                                    {
                                                        if(m_activation_table.is_active(va.point_set_id, vb.point_set_id))
                                                        {
                                                            da.m_neighbors[vb.point_set_id][va.point_id].push_back(vb.point_id);
                                                        }
                                                        if(m_activation_table.is_active(vb.point_set_id, va.point_set_id))
                                                        {
                                                            db.m_neighbors[va.point_set_id][vb.point_id].push_back(va.point_id);
                                                        }
                                                    }
                                                }
                                            }
                                        });


    Vector<std::array<bool, 27> > visited(m_entries.size(), { false });
    Vector<Spinlock>              entry_locks(m_entries.size());

    ParallelFuncs::parallel_for<size_t>(0, kvps.size(),
                                        [&](size_t it)
                                        {
                                            std::pair<HashKey const, UInt> const* kvp_ = kvps[it];
                                            auto const& kvp = *kvp_;
                                            HashEntry const& entry = m_entries[kvp.second];

                                            if(entry.n_searching_points == 0u)
                                                return;
                                            HashKey const& key = kvp.first;

                                            for(int dj = -1; dj <= 1; dj++)
                                                for(int dk = -1; dk <= 1; dk++)
                                                    for(int dl = -1; dl <= 1; dl++)
                                                    {
                                                        int l_ind = 9 * (dj + 1) + 3 * (dk + 1) + (dl + 1);
                                                        if(l_ind == 13)
                                                        {
                                                            continue;
                                                        }
                                                        entry_locks[kvp.second].lock();
                                                        if(visited[kvp.second][l_ind])
                                                        {
                                                            entry_locks[kvp.second].unlock();
                                                            continue;
                                                        }
                                                        entry_locks[kvp.second].unlock();



                                                        auto it = m_map.find({ key.k[0] + dj, key.k[1] + dk, key.k[2] + dl });
                                                        if(it == m_map.end())
                                                            continue;

                                                        std::array<UInt, 2> entry_ids {{ kvp.second, it->second } };
                                                        if(entry_ids[0] > entry_ids[1])
                                                            std::swap(entry_ids[0], entry_ids[1]);
                                                        entry_locks[entry_ids[0]].lock();
                                                        entry_locks[entry_ids[1]].lock();

                                                        if(visited[kvp.second][l_ind])
                                                        {
                                                            entry_locks[entry_ids[1]].unlock();
                                                            entry_locks[entry_ids[0]].unlock();
                                                            continue;
                                                        }

                                                        visited[kvp.second][l_ind] = true;
                                                        visited[it->second][26 - l_ind] = true;

                                                        entry_locks[entry_ids[1]].unlock();
                                                        entry_locks[entry_ids[0]].unlock();

                                                        for(UInt i = 0; i < entry.n_indices(); ++i)
                                                        {
                                                            PointID const& va = entry.indices[i];
                                                            HashEntry const& entry_ = m_entries[it->second];
                                                            UInt n_ind = entry_.n_indices();
                                                            for(UInt j = 0; j < n_ind; ++j)
                                                            {
                                                                PointID const& vb = entry_.indices[j];
                                                                PointSet& db = m_point_sets[vb.point_set_id];

                                                                PointSet& da = m_point_sets[va.point_set_id];

                                                                if(!m_activation_table.is_active(va.point_set_id, vb.point_set_id) &&
                                                                   !m_activation_table.is_active(vb.point_set_id, va.point_set_id))
                                                                {
                                                                    continue;
                                                                }

                                                                Real const* xa = da.point(va.point_id);
                                                                Real const* xb = db.point(vb.point_id);
                                                                Real tmp = xa[0] - xb[0];
                                                                Real l2 = tmp * tmp;
                                                                tmp = xa[1] - xb[1];
                                                                l2 += tmp * tmp;
                                                                tmp = xa[2] - xb[2];
                                                                l2 += tmp * tmp;
                                                                if(l2 < m_r2)
                                                                {
                                                                    if(m_activation_table.is_active(va.point_set_id, vb.point_set_id))
                                                                    {
                                                                        da.m_locks[vb.point_set_id][va.point_id].lock();
                                                                        da.m_neighbors[vb.point_set_id][va.point_id].push_back(vb.point_id);
                                                                        da.m_locks[vb.point_set_id][va.point_id].unlock();
                                                                    }
                                                                    if(m_activation_table.is_active(vb.point_set_id, va.point_set_id))
                                                                    {
                                                                        db.m_locks[va.point_set_id][vb.point_id].lock();
                                                                        db.m_neighbors[va.point_set_id][vb.point_id].push_back(va.point_id);
                                                                        db.m_locks[va.point_set_id][vb.point_id].unlock();
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void NeighborSearch::query(UInt point_set_id, UInt point_index, Vec_VecUInt& neighbors)
{
    neighbors.resize(m_point_sets.size());
    PointSet& d = m_point_sets[point_set_id];
    for(UInt j = 0; j < m_point_sets.size(); j++)
    {
        auto& n = neighbors[j];
        n.clear();
        if(m_activation_table.is_active(point_set_id, j))
            n.reserve(INITIAL_NUMBER_OF_NEIGHBORS);
    }

    Real const* xa       = d.point(point_index);
    HashKey     hash_key = cell_index(xa);

    auto                            it    = m_map.find(hash_key);
    std::pair<HashKey const, UInt>& kvp   = *it;
    HashEntry const&                entry = m_entries[kvp.second];

    // Perform neighborhood search.
    for(UInt b = 0; b < entry.n_indices(); ++b)
    {
        PointID const& vb = entry.indices[b];
        if((point_set_id != vb.point_set_id) || (point_index != vb.point_id))
        {
            if(!m_activation_table.is_active(point_set_id, vb.point_set_id))
            {
                continue;
            }

            PointSet&   db  = m_point_sets[vb.point_set_id];
            Real const* xb  = db.point(vb.point_id);
            Real        tmp = xa[0] - xb[0];
            Real        l2  = tmp * tmp;
            tmp = xa[1] - xb[1];
            l2 += tmp * tmp;
            tmp = xa[2] - xb[2];
            l2 += tmp * tmp;

            if(l2 < m_r2)
            {
                neighbors[vb.point_set_id].push_back(vb.point_id);
            }
        }
    }

    for(int dj = -1; dj <= 1; dj++)
    {
        for(int dk = -1; dk <= 1; dk++)
        {
            for(int dl = -1; dl <= 1; dl++)
            {
                int l_ind = 9 * (dj + 1) + 3 * (dk + 1) + (dl + 1);
                if(l_ind == 13)
                {
                    continue;
                }

                auto it = m_map.find({ hash_key.k[0] + dj, hash_key.k[1] + dk, hash_key.k[2] + dl });
                if(it == m_map.end())
                    continue;

                std::array<UInt, 2> entry_ids {{ kvp.second, it->second } };
                if(entry_ids[0] > entry_ids[1])
                    std::swap(entry_ids[0], entry_ids[1]);

                HashEntry const& entry_ = m_entries[it->second];
                UInt             n_ind  = entry_.n_indices();
                for(UInt j = 0; j < n_ind; ++j)
                {
                    PointID const& vb = entry_.indices[j];
                    if(!m_activation_table.is_active(point_set_id, vb.point_set_id))
                    {
                        continue;
                    }
                    PointSet& db = m_point_sets[vb.point_set_id];

                    Real const* xb  = db.point(vb.point_id);
                    Real        tmp = xa[0] - xb[0];
                    Real        l2  = tmp * tmp;
                    tmp = xa[1] - xb[1];
                    l2 += tmp * tmp;
                    tmp = xa[2] - xb[2];
                    l2 += tmp * tmp;

                    if(l2 < m_r2)
                    {
                        neighbors[vb.point_set_id].push_back(vb.point_id);
                    }
                }
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
