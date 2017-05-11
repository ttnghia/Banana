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

#include <algorithm>

#include <tbb/tbb.h>

#include <Noodle/Core/Trees/KDTree.h>
#include <Noodle/Core/LevelSet/SignDistanceField.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void KDTree::buildTree(KDNode* treeNode)
{
    int  longAxis = 0, lc;
    Real maxLength, length, median;
    Vec3 upCorner, dnCorner;

    Point* leftArray;
    Point* rightArray;
    int    leftArrayCount, j;

    if(treeNode->isLeaf)
    {
        lc = treeNode->count;

        // this is the only way out
        if(lc <= m_MaxNodeIterm)
        {
            return;
        }

        maxLength = -1e10;

        for(j = 0; j < 3; j++)
        {
            upCorner[j] = treeNode->boxMax[j];
            dnCorner[j] = treeNode->boxMin[j];

            if((length = (upCorner[j] - dnCorner[j])) > maxLength)
            {
                longAxis  = j;
                maxLength = length;
            }
        }

        median           = getMedian(treeNode->points, lc, longAxis);
        treeNode->split  = median;
        treeNode->axis   = longAxis;
        treeNode->isLeaf = 0;

        if(lc % 2 == 0)
        {
            leftArrayCount = lc / 2;
        }
        else
        {
            leftArrayCount = lc / 2 + 1;
        }

        leftArray  = treeNode->points;
        rightArray = &(treeNode->points[leftArrayCount]);

        upCorner[longAxis] = dnCorner[longAxis] = median;
        treeNode->leftNode = new KDNode(&leftArray[0], leftArrayCount, treeNode->boxMin, upCorner);
        treeNode->right    = new KDNode(&rightArray[0], lc / 2, dnCorner, treeNode->boxMax);
    }

    tbb::parallel_invoke([&]
    {
        buildTree(treeNode->leftNode);
    },
            [&]
    {
        buildTree(treeNode->right);
    });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void KDTree::printTree(KDNode* treeNode)
{
    if(treeNode->isLeaf)
    {
        printf("Found leaf node with count %d, bMin=%f,%f,%f, bMax=%f,%f,%f\n     ", treeNode->count,
                treeNode->boxMin[0], treeNode->boxMin[1], treeNode->boxMin[2],
                treeNode->boxMax[0], treeNode->boxMax[1], treeNode->boxMax[2]);

        for(int i = 0; i < treeNode->count; ++i)
        {
            Vec3 p = treeNode->points[i].position;

            if(p[0] < treeNode->boxMin[0] ||
               p[1] < treeNode->boxMin[1] ||
               p[2] < treeNode->boxMin[2] ||
               p[0] > treeNode->boxMax[0] ||
               p[1] > treeNode->boxMax[1] ||
               p[2] > treeNode->boxMax[2])
            {
                Vec3 p1 = p - treeNode->boxMin;
                Vec3 p2 = p - treeNode->boxMax;
                printf("invad : %d,   %f,%f,%f,    %f,%f,%f,    %f,%f,%f\n", treeNode->points[i].index, p[0], p[1], p[2], p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);
            }
        }

        for(int i = 0; i < treeNode->count - 1; ++i)
        {
            printf("%d, ", treeNode->points[i].index);
        }

        if(treeNode->count - 1 >= 0)
        {
            printf("%d", treeNode->points[treeNode->count - 1].index);
        }

        printf("\n\n\n");

        return;
    }

    printf("Interior node; split value %f on axis %d\n", treeNode->split, treeNode->axis);

    printTree(treeNode->leftNode);
    printTree(treeNode->right);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void KDTree::getNeighborList(Point& target, KDNode* treeNode, Real radius, Vec_UInt& result)
{
    result.clear();
    findNeighbors(target, treeNode, radius, result);
    std::sort(result.begin(), result.end());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void KDTree::findNeighbors(Point& target, KDNode* treeNode, Real radius, Vec_UInt& result)
{
    if(fmax(SignDistanceField::sign_distance_box(target.position, treeNode->boxMin, treeNode->boxMax), 0) > radius)
    {
        return;
    }

    if(treeNode->isLeaf)
    {
        for(int i = 0; i < treeNode->count; ++i)
        {
            if((target.index != treeNode->points[i].index) && (glm::length(target.position - treeNode->points[i].position) < radius))
            {
                result.push_back(treeNode->points[i].index);
            }
        }
    }
    else
    {
        findNeighbors(target, treeNode->leftNode, radius, result);
        findNeighbors(target, treeNode->right,    radius, result);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real KDTree::getMedian(Point* points, int size, int axis)
{
    int k;

    if(size % 2 == 0)
    {
        k = size / 2 - 1;
    }
    else
    {
        k = size / 2;
    }

    //    std::stable_sort(points, points + size, [axis](Point a, Point b)
    tbb::parallel_sort(points, points + size, [axis](Point a, Point b)
    {
        return a.position[axis] < b.position[axis];
    });

    return (points[k].position[axis]);
}
