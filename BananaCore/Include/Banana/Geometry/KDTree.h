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

#include <Banana/TypeNames.h>
#include <Banana/Geometry/SignDistanceField.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
class Point
{
public:
    Point() {}

    Point(const Vec3<T>& pos_, unsigned int index_) : position(pos_), index(index_) {}

    Point(const std::initializer_list& pos_, unsigned int index_) : position(pos_), index(index_) {}

    Vec3<T>      position = { { 0, 0, 0 } };
    unsigned int index    = UINT_MAX;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
class KDNode
{
public:
    KDNode(Point<T>* points_, const Vec3<T>& boxMin_, const Vec3<T>& boxMax_) : points(points_), boxMin(boxMin_), boxMax(boxMax_), {}

    Point<T>* points;
    Vec3<T>   boxMin;
    Vec3<T>   boxMax;

    std::shared_ptr<KDNode<T> > leftNode  = nullptr;
    std::shared_ptr<KDNode<T> > rightNode = nullptr;

    unsigned int count  = 0;
    unsigned int axis   = 0;         // 0==x, 1==y, 2==z
    T            split  = 0;
    bool         isLeaf = true;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
class KDTree
{
public:
    KDTree(unsigned int maxItems) : m_MaxNodeIterm(maxItems) {}

    ////////////////////////////////////////////////////////////////////////////////
    void buildTree(const std::shared_ptr<KDNode<T> >& treeNode)
    {
        if(treeNode->isLeaf)
        {
            unsigned int nodeCount = treeNode->count;

            // this is the only way out
            if(nodeCount <= m_MaxNodeIterm)
            {
                return;
            }
            Vec3<T> upCorner, dnCorner;

            unsigned int longestAxis = 0;
            T            maxLength   = -1e10;

            for(unsigned int j = 0; j < 3; ++j)
            {
                upCorner[j] = treeNode->boxMax[j];
                dnCorner[j] = treeNode->boxMin[j];

                T length = upCorner[j] - dnCorner[j];
                if(length > maxLength)
                {
                    longestAxis = j;
                    maxLength   = length;
                }
            }

            T median = getMedian(treeNode->points, nodeCount, longestAxis);
            treeNode->split  = median;
            treeNode->axis   = longestAxis;
            treeNode->isLeaf = false;

            unsigned int leftArrayCount = nodeCount / 2;
            Point<T>*    leftArray      = treeNode->points;
            Point<T>*    rightArray     = &(treeNode->points[leftArrayCount]);

            upCorner[longestAxis] = dnCorner[longestAxis] = median;
            treeNode->leftNode    = std::make_shared<KDNode<T> >(leftArray, leftArrayCount, treeNode->boxMin, upCorner);
            treeNode->rightNode   = std::make_shared<KDNode<T> >(rightArray, nodeCount - leftArrayCount, dnCorner, treeNode->boxMax);
        }

        buildTree(treeNode->leftNode);
        buildTree(treeNode->rightNode);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void printTree(const std::shared_ptr<KDNode<T> >& treeNode)
    {
        if(treeNode->isLeaf && treeNode->count >= 1)
        {
            auto boxMin = treeNode->boxMin;
            auto boxMax = treeNode->boxMax;

            printf("Found leaf node with count = %u, bMin=[%f,%f,%f], bMax=[%f,%f,%f]\n     ", treeNode->count, boxMin[0], boxMin[1], boxMin[2], boxMax[0], boxMax[1], boxMax[2]);

            for(unsigned int i = 0; i < treeNode->count; ++i)
            {
                auto p = treeNode->points[i].position;

                if(p[0] < boxMin[0] || p[1] < boxMin[1] || p[2] < boxMin[2] ||
                   p[0] > boxMax[0] || p[1] > boxMax[1] || p[2] > boxMax[2])
                {
                    printf("Invalid element : index = %u, position = [%f,%f,%f], bMin= [%f,%f,%f], bMax = [%f,%f,%f]\n",
                            treeNode->points[i].index, p[0], p[1], p[2], boxMin[0], boxMin[1], boxMin[2], boxMax[0], boxMax[1], boxMax[2]);
                }
            }


            for(unsigned int i = 0; i < treeNode->count - 1; ++i)
            {
                printf("%u, ", treeNode->points[i].index);
            }

            printf("%u", treeNode->points[treeNode->count - 1].index);


            printf("\n\n\n");
        }
        else
        {
            printf("Interior node; split value %f on axis %u\n", treeNode->split, treeNode->axis);

            printTree(treeNode->leftNode);
            printTree(treeNode->rightNode);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    void getNeighborList(const Point& target, const std::shared_ptr<KDNode<T> >& treeNode, T radius, Vec_UInt& result)
    {
        result.clear();
        findNeighbors(target, treeNode, radius, result);
        tbb::parallel_sort(result.begin(), result.end());
    }

private:
    void findNeighbors(const Point& target, const std::shared_ptr<KDNode<T> >& treeNode, T radius, Vec_UInt& result)
    {
        if(fmax(SignDistanceField::distanceToBox(target.position, treeNode->boxMin, treeNode->boxMax), 0) > radius)
        {
            return;
        }

        if(treeNode->isLeaf)
        {
            for(unsigned int i = 0; i < treeNode->count; ++i)
            {
                if((target.index != treeNode->points[i].index) && (glm::length(target.position - treeNode->points[i].position) < radius))
                {
                    result.push_back(treeNode->points[i].index);
                }
            }
        }
        else
        {
            findNeighbors(target, treeNode->leftNode,  radius, result);
            findNeighbors(target, treeNode->rightNode, radius, result);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    T getMedian(Point* points, unsigned int size, unsigned int axis)
    {
        unsigned int k = (size & 1) ?  size / 2 : size / 2 - 1;

        tbb::parallel_sort(points, points + size, [axis](const Point& a, const Point& b)
        {
            return a.position[axis] < b.position[axis];
        });

        return (points[k].position[axis]);
    }

    ////////////////////////////////////////////////////////////////////////////////
    unsigned int m_MaxNodeIterm;
};