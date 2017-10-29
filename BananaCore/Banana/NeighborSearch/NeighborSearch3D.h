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

#include <Banana/NeighborSearch/DataStructures.h>
#include <Banana/NeighborSearch/PointSet.h>
#include <Banana/NeighborSearch/Morton/Morton.h>

#include <unordered_map>
#include <exception>
#include <iostream>
#include <numeric>
#include <array>
#include <cstdint>
#include <limits>
#include <algorithm>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace NeighborSearch
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define INITIAL_NUMBER_OF_NEIGHBORS 50

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct NeighborhoodSearchNotInitialized : public std::exception
{
    virtual char const* what() const noexcept override { return "Neighborhood search was not initialized."; }
};

/**
 * @class NeighborhoodSearch
 * Stores point data multiple set of points in which neighborhood information for a fixed
 * radius r should be generated.
 */

class NeighborSearch3D
{
public:

    /**
     * Constructor.
     * Creates a new instance of the neighborhood search class.
     * @param r Search radius. If two points are closer to each other than a distance r they are considered neighbors.
     * @param erase_empty_cells If true. Empty cells in spatial hashing grid are erased if the points move.
     */
    NeighborSearch3D(Real r, bool erase_empty_cells = false);

    /**
     * Destructor.
     */
    virtual ~NeighborSearch3D() = default;

    /**
     * Get method to access a point set.
     * @param i Index of the point set to retrieve.
     */
    const PointSet& point_set(UInt i) const { return m_point_sets[i]; }

    /**
     * Get method to access a point set.
     * @param i Index of the point set to retrieve.
     */
    PointSet& point_set(UInt i) { return m_point_sets[i]; }


    /**
     * Returns the number of point sets contained in the search.
     */
    std::size_t n_point_sets()               const { return m_point_sets.size(); }

    /**
     * Get method to access the list of point sets.
     */
    Vector<PointSet> const& point_sets() const { return m_point_sets; }

    /**
     * Get method to access the list of point sets.
     */
    Vector<PointSet>& point_sets() { return m_point_sets; }

    /**
     * Increases the size of a point set under the assumption that the existing points remain at
     * the same position.
     * @param i Index of point set that will be resized.
     * @param x Pointer to the point position data. Must point to continguous data of 3 * n
     * real values.
     * @param n Number of points.
     */
    void resize_point_set(UInt i, Real const* x, std::size_t n);

    /**
     * Creates and adds a new set of points.
     * @param x Pointer to the point position data. Must point to continguous data of 3 * n
     * real values.
     * @param n Number of points.
     * @param is_dynamic Specifies whether the point positions will change for future queries.
     * @param search_neighbors If true, neighbors in all other point sets are searched.
     * @param find_neighbors If true, the new point set is activated in the neighborhood search of all other point sets.
     * @returns Returns unique identifier in form of an index assigned to the newly created point
     * set.
     */
    UInt add_point_set(Real const* x, std::size_t n, bool is_dynamic = true,
                       bool search_neighbors = true, bool find_neighbors = true)
    {
        m_point_sets.push_back({ x, n, is_dynamic });
        m_activation_table.add_point_set(search_neighbors, find_neighbors);
        return static_cast<UInt>(m_point_sets.size() - 1);
    }

    /**
     * Performs the actual query. This method will assign a list of neighboring points to each point
     * every added point set.
     */
    void find_neighbors(bool points_changed = true);

    /**
     * Performs the actual query for a single point. This method return a list of neighboring points. Note: That points_changed() must be called each time
     * when the positions of a point set changed.
     */
    void find_neighbors(UInt point_set_id, UInt point_index, Vec_VecUInt& neighbors);

    /**
     * Update neighborhood search data structures after a position change.
     * If general find_neighbors() function is called there is no requirement to manually update the point sets.
     * Otherwise, in case of using point-wise search (find_neighbors(i, j, neighbors)) the method must be called explicitly.
     * TODO: Implement function for individual pointset. -> update_point_set(i)
     */
    void update_point_sets();

    /**
     * Update neighborhood search data structures after changing the activation table.
     * If general find_neighbors() function is called there is no requirement to manually update the point sets.
     * Otherwise, in case of using point-wise search (find_neighbors(i, j, neighbors)) the method must be called explicitly.
     */
    void update_activation_table();


    /*
     * Generates a sort table according to a space-filling Z curve. Any array-based per point
     * information can then be reordered using the function sort_field of the PointSet class.
     * Please note that the position data will not be modified by this class, such that the user has
     * to invoke the sort_field function on the position array. Moreover, be aware the the grid has
     * be reinitialized after each sort. Therefore, the points should not be reordered too
     * frequently.
     */
    void z_sort();

    /*
     * @returns Returns the radius in which point neighbors are searched.
     */
    Real radius() const { return std::sqrt(m_r2); }

    /**
     * Sets the radius in which point point neighbors are searched.
     * @param r Search radius.
     */
    void set_radius(Real r)
    {
        m_r2            = r * r;
        m_inv_cell_size = static_cast<Real>(1.0 / r);
        m_initialized   = false;
    }

    /** Activate/deactivate that neighbors in point set j are found when searching for neighbors of point set i.
     *   @param i Index of searching point set.
     *   @param j Index of point set of which points should/shouldn't be found by point set i.
     *   @param active Flag in order to (de)activate that points in i find point in j.
     */
    void set_active(UInt i, UInt j, bool active)
    {
        m_activation_table.set_active(i, j, active);
    }

    /** Activate/Deactivate all point set pairs containing the given index. If search_neighbors is true, neighbors in all other point sets are searched.
     *   If find_neighbors is true, the new point set is activated in the neighborhood search of all other point sets.
     *   @param i Index of searching point set.
     *   @param search_neighbors If true/false enables/disables that point set i searches points in all other point sets.
     *   @param find_neighbors If true/false enable/disables that point set i is found by all other point sets.
     */
    void set_active(UInt i, bool search_neighbors = true, bool find_neighbors = true)
    {
        m_activation_table.set_active(i, search_neighbors, find_neighbors);
    }

    /** Activate/Deactivate all point set pairs.
     */
    void set_active(bool active)
    {
        m_activation_table.set_active(active);
    }

    /** Returns true if point set i searchs points in point set j.
     *   @param i Searching point set.
     *   @param j Set of points to be found by i.
     */
    bool is_active(UInt i, UInt j) const
    {
        return m_activation_table.is_active(i, j);
    }

private:
    void init();
    void update_hash_table(Vec_UInt& to_delete);
    void erase_empty_entries(Vec_UInt const& to_delete);
    void query();
    void query(UInt point_set_id, UInt point_index, Vec_VecUInt& neighbors);

    HashKey       cell_index(Real const* x) const;
    uint_fast64_t z_value(HashKey const& key);     // Determines Morten value according to z-curve

    ////////////////////////////////////////////////////////////////////////////////
    Vector<PointSet> m_point_sets;
    ActivationTable  m_activation_table, m_old_activation_table;

    Real m_inv_cell_size;
    Real m_r2;

    std::unordered_map<HashKey, UInt, SpatialHasher> m_map;
    Vector<HashEntry>                                m_entries;

    bool m_erase_empty_cells;
    bool m_initialized;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace NeighborSearch

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
