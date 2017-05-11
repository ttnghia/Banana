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

#pragma once

#include <array>
#include <climits>
#include <memory>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
class Point
{
public:
    Point()
    {}

    Point(const std::array<T, 3>& pos_, unsigned int index_) : position(pos_), index(index_)
    {}

    Point(const std::initializer_list& pos_, unsigned int index_) : position(pos_), index(index_)
    {}

    std::array<T, 3> position = { { 0, 0, 0 } };
    unsigned int     index    = UINT_MAX;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
class KDNode
{
public:
    KDNode(const Point<T>* points_, const std::array<T, 3>& boxMin_, const std::array<T, 3>& boxMax_) : points(points_), boxMin(boxMin_), boxMax(boxMax_),
    {}

    Point<T>*        points;
    std::array<T, 3> boxMin;
    std::array<T, 3> boxMax;

    std::shared_ptr<KDNode<T> > leftNode  = nullptr;
    std::shared_ptr<KDNode<T> > rightNode = nullptr;

    T    split  = 0;
    char axis   = -1;         // 0==x, 1==y, 2==z
    bool isLeaf = true;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
class KDTree
{
public:
    KDTree(unsigned int maxItems) : m_MaxNodeIterm(maxItems)
    {}

    void buildTree(const std::shared_ptr<KDNode<T> >& treeNode);
    void printTree(const std::shared_ptr<KDNode<T> >& treeNode);
    void getNeighborList(Point& target, const std::shared_ptr<KDNode<T> >& treeNode, Real radius, Vec_UInt& result);

private:
    void findNeighbors(Point& target, const std::shared_ptr<KDNode<T> >& treeNode, Real radius, Vec_UInt& result);
    T    getMedian(Point* points, int size, int axis);

    unsigned int m_MaxNodeIterm;
};