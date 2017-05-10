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

#include <Noodle/Core/Global/TypeNames.h>
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Point
{
public:
    Point() : position(Vec3(0, 0, 0)), index(-1)
    {}
    Point(const Vec3& pos_, UInt32 index_) : position(pos_), index(index_)
    {}

    Vec3   position;
    UInt32 index;
};

class KDNode
{
public:
    KDNode(Point* points_, int count_, const Vec3& boxMin_, const Vec3& boxMax_) :
        left(nullptr), right(nullptr), axis(-1), split(0.0), isLeaf(1),
        points(points_), count(count_), boxMin(boxMin_), boxMax(boxMax_)
    {}

    KDNode* left;
    KDNode* right;
    Point*  points;

    int  axis;          // 0==x, 1==y, 2==z
    int  isLeaf;
    int  count;
    Real split;

    Vec3 boxMin;
    Vec3 boxMax;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class KDTree
{
public:
    KDTree(int maxItems) : m_MaxItermPerNode(maxItems)
    {}

    void buildTree(KDNode* treeNode);
    void printTree(KDNode* treeNode);
    void getNeighborList(Point& target, KDNode* treeNode, Real radius, Vec_UInt& result);

private:
    void findNeighbors(Point& target, KDNode* treeNode, Real radius, Vec_UInt& result);
    Real getMedian(Point* points, int size, int axis);

    int m_MaxItermPerNode;
};