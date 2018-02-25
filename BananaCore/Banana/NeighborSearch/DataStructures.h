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

#pragma once

#include <vector>
#include <Banana/Setup.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::NeighborSearch
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define INITIAL_NUMBER_OF_INDICES 50
#define SHIFT_POSITION            12345.6789

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct PointID
{
    UInt point_set_id;
    UInt point_id;

    bool operator ==(PointID const& other) const
    {
        return point_id == other.point_id && point_set_id == other.point_set_id;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N>
struct HashKey;

template<>
struct HashKey<2>
{
    HashKey() = default;
    HashKey(int i, int j)
    {
        this->k[0] = i, this->k[1] = j;
    }

    HashKey& operator =(HashKey const& other)
    {
        k[0] = other.k[0];
        k[1] = other.k[1];
        return *this;
    }

    bool operator ==(HashKey const& other) const
    {
        return (k[0] == other.k[0] &&
                k[1] == other.k[1]);
    }

    bool operator !=(HashKey const& other) const
    {
        return !(*this == other);
    }

    ////////////////////////////////////////////////////////////////////////////////
    int k[2];
};

template<>
struct HashKey<3>
{
    HashKey() = default;
    HashKey(int i, int j, int k)
    {
        this->k[0] = i, this->k[1] = j, this->k[2] = k;
    }

    HashKey& operator =(HashKey const& other)
    {
        k[0] = other.k[0];
        k[1] = other.k[1];
        k[2] = other.k[2];
        return *this;
    }

    bool operator ==(HashKey const& other) const
    {
        return (k[0] == other.k[0] &&
                k[1] == other.k[1] &&
                k[2] == other.k[2]);
    }

    bool operator !=(HashKey const& other) const
    {
        return !(*this == other);
    }

    ////////////////////////////////////////////////////////////////////////////////
    int k[3];
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct HashEntry
{
    HashEntry() : n_searching_points(0u)
    {
        indices.reserve(INITIAL_NUMBER_OF_INDICES);
    }

    HashEntry(PointID const& id) : n_searching_points(0u)
    {
        add(id);
    }

    void add(PointID const& id)
    {
        indices.push_back(id);
    }

    void erase(PointID const& id)
    {
        auto it = std::find(indices.begin(), indices.end(), id);
        if(it != indices.end()) {
            indices.erase(it);
        }
    }

    UInt n_indices() const
    {
        return static_cast<UInt>(indices.size());
    }

    ////////////////////////////////////////////////////////////////////////////////
    std::vector<PointID> indices;
    UInt                 n_searching_points;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N>
struct SpatialHasher;

template<>
struct SpatialHasher<2>
{
    std::size_t operator ()(HashKey<2> const& k) const
    {
        return (73856093 * k.k[0] ^
                19349663 * k.k[1]);
    }
};

template<>
struct SpatialHasher<3>
{
    std::size_t operator ()(HashKey<3> const& k) const
    {
        return (73856093 * k.k[0] ^
                19349663 * k.k[1] ^
                83492791 * k.k[2]);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ActivationTable
{
private:
    std::vector<std::vector<unsigned char>> m_table;

public:

    bool operator ==(ActivationTable const& other) const
    {
        return m_table == other.m_table;
    }

    bool operator !=(ActivationTable const& other) const
    {
        return !(m_table == other.m_table);
    }

    /** Add point set. If search_neighbors is true, neighbors in all other point sets are searched.
     * If find_neighbors is true, the new point set is activated in the neighborhood search of all other point sets.
     */
    void add_point_set(bool search_neighbors = true, bool find_neighbors = true)
    {
        // add column to each row
        auto size = m_table.size();
        for(auto i = 0u; i < size; i++) {
            m_table[i].resize(size + 1);
            m_table[i][size] = static_cast<unsigned char>(find_neighbors);
        }

        // add new row
        m_table.resize(size + 1);
        m_table[size].resize(size + 1);
        for(auto i = 0u; i < size + 1; i++) {
            m_table[size][i] = static_cast<unsigned char>(search_neighbors);
        }
    }

    /** Activate/Deactivate that neighbors in point set index2 are found when searching for neighbors of point set index1.
     */
    void set_active(UInt index1, UInt index2, bool active)
    {
        m_table[index1][index2] = static_cast<unsigned char>(active);
    }

    /** Activate/Deactivate all point set pairs containing the given index. If search_neighbors is true, neighbors in all other point sets are searched.
     * If find_neighbors is true, the new point set is activated in the neighborhood search of all other point sets.
     */
    void set_active(UInt index, bool search_neighbors = true, bool find_neighbors = true)
    {
        auto size = m_table.size();
        for(auto i = 0u; i < size; i++) {
            m_table[i][index] = static_cast<unsigned char>(find_neighbors);
            m_table[index][i] = static_cast<unsigned char>(search_neighbors);
        }
        m_table[index][index] = static_cast<unsigned char>(search_neighbors && find_neighbors);
    }

    /** Activate/Deactivate all point set pairs.
     */
    void set_active(bool active)
    {
        auto size = m_table.size();
        for(auto i = 0u; i < size; i++) {
            for(auto j = 0u; j < size; j++) {
                m_table[i][j] = static_cast<unsigned char>(active);
            }
        }
    }

    bool is_active(UInt index1, UInt index2) const
    {
        return m_table[index1][index2] != 0;
    }

    bool is_searching_neighbors(UInt const index) const
    {
        for(auto i = 0u; i < m_table[index].size(); i++) {
            if(m_table[index][i]) {
                return true;
            }
        }
        return false;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::NeighborSearch
