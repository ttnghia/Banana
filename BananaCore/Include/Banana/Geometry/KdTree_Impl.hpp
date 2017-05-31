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
template<class T>
void KdTree<T>::buildTree(const std::shared_ptr<KDNode<T> >& treeNode)
{
    if(treeNode->isLeaf)
    {
        unsigned int nodeCount = treeNode->count;

        // this is the only way out
        if(nodeCount <= m_MaxItermsPerNode)
        {
            return;
        }

        Vec3<T> upCorner, dnCorner;

        unsigned int longestAxis = 0;
        T            maxLength   = -1e20;

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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void KdTree<T>::printTree(const std::shared_ptr<KDNode<T> >& treeNode)
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void KdTree<T>::getNeighborList(const Point<T>& target, const std::shared_ptr<KDNode<T> >& treeNode, T radius, Vec_UInt& result)
{
    result.resize(0);
    findNeighbors(target, treeNode, radius, result);
    tbb::parallel_sort(result.begin(), result.end());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void KdTree<T>::findNeighbors(const Point<T>& target, const std::shared_ptr<KDNode<T> >& treeNode, T radius, Vec_UInt& result)
{
    if(fmax(SignDistanceField::distanceToBox(target.position, treeNode->boxMin, treeNode->boxMax), 0) > radius)
    {
        return;
    }

    if(treeNode->isLeaf)
    {
        T r2 = radius * radius;

        for(unsigned int i = 0; i < treeNode->count; ++i)
        {
            if((target.index != treeNode->points[i].index) &&
               (glm::length2(target.position - treeNode->points[i].position) < r2))
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T KdTree<T>::getMedian(Point<T>* points, unsigned int size, unsigned int axis)
{
    unsigned int k = (size & 1) ?  size / 2 : size / 2 - 1;

    tbb::parallel_sort(points, points + size, [axis](const Point<T>& a, const Point<T>& b) { return a.position[axis] < b.position[axis]; });

    return (points[k].position[axis]);
}
