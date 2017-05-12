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

#include <BananaCore/TypeNames.h>

namespace SignDistanceField
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T distanceToBox(const Vec3<T>& pos, const Vec3<T>& bMin, const Vec3<T>& bMax)
{
    if(pos[0] >= bMin[0] && pos[0] <= bMax[0] &&
       pos[1] >= bMin[1] && pos[1] <= bMax[1] &&
       pos[2] >= bMin[2] && pos[2] <= bMax[2])
    {
        T dists[6] = { pos[0] - bMin[0], bMax[0] - pos[0],
                       pos[1] - bMin[1], bMax[1] - pos[1],
                       pos[2] - bMin[2], bMax[2] - pos[2] };

        T minDist = dists[0];

        for(int i = 1; i < 6; ++i)
        {
            if(minDist > dists[i])
            {
                minDist = dists[i];
            }
        }

        return -minDist;
    }
    else
    {
        Vec3<T> cp = Vec3<T>(fmax(fmin(pos[0], bMax[0]), bMin[0]), fmax(fmin(pos[1], bMax[1]), bMin[1]), fmax(fmin(pos[2], bMax[2]), bMin[2]));

        return glm::length(pos - cp);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T distanceToSphere(const Vec3<T>& pos, const Vec3<T>& center, T radius)
{
    return (glm::length(pos - center) - radius);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T distanceToHollowSphere(const Vec3<T>& pos, const Vec3<T>& center, T innerRadius, T outerRadius)
{
    T d1 = (glm::length(pos - center) - outerRadius);
    T d2 = (innerRadius - glm::length(pos - center));

    return (d1 > d2 ? d1 : d2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T distanceToBowl(const Vec3<T>& pos, const Vec3<T>& center, T innerRadius, T outerRadius, T thickness, T max_y)
{
    return fmax(fmax(fmax(
                            fmin(-distanceToSphere(pos, center, innerRadius), pos[1] - thickness),
                            distanceToSphere(pos, center, outerRadius)),
                    pos[1] - max_y),
            -pos[1]);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T distanceToCylinder(const Vec3<T>& pos, const Vec3<T>& cylinderBase, const Vec3<T>& cylinderDirection, T cylinderRadius, T cylinderLength)
{
    Vec3<T> u     = cylinderDirection;
    Vec3<T> c     = cylinderBase + u * lhalf;
    Vec3<T> cp = c - pos;

    T       l     = cylinderLength;
    T       lhalf = static_cast<T>(0.5) * l;
    T       x  = glm::dot(cp, u);
    T       n2 = glm::dot(cp, cp);
    T       y2 = n2 - x * x;

    assert(y2 >= 0);
    T y = sqrt(y2);


    if(fabs(x) < lhalf)
    {
        if(y < cylinderRadius)    // inside cylinder, distance < 0
        {
            Vec3<T> h = c + x * u;
            return -fmin(fmin(cylinderRadius - y, glm::length(h - cylinderBase)), glm::length(h - cylinderBase - l * u));
        }
        else
        {
            return (y - cylinderRadius);
        }
    }
    else
    {
        if(y < cylinderRadius)
            return (fabs(x) - lhalf);
        else
            return sqrt((y - cylinderRadius) * (y - cylinderRadius) + (fabs(x) - lhalf) * (fabs(x) - lhalf));
    }
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace SignDistanceField