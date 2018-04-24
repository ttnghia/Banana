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

#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include "Common.cuh"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
rtDeclareVariable(int, has_envmap, , );
rtDeclareVariable(float3, background_light, , ); // horizon color
rtDeclareVariable(float3, background_dark, , );  // zenith color
rtDeclareVariable(float3, up, , );               // global up vector

rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(PerRayData_Radiance, prd_radiance, rtPayload, );

rtTextureSampler<float4, 2> envmap;

// -----------------------------------------------------------------------------
RT_PROGRAM void miss()
{
    if(has_envmap > 0)
    {
          float theta = atan2f( ray.direction.x, ray.direction.z );
          float phi   = M_PIf * 0.5f -  acosf( ray.direction.y );
          float u     = (theta + M_PIf) * (0.5f * M_1_PIf);
          float v     = 0.5f * ( 1.0f + sin(phi) );
          optix::float3 result = optix::make_float3(tex2D(envmap, u, v));

          prd_radiance.radiance = result;
          prd_radiance.done = true;
    }
    else
    {
        const float t = optix::max(optix::dot(ray.direction, up), 0.0f);
        const float3 result = optix::lerp(background_light, background_dark, t);

        prd_radiance.radiance = result;
        prd_radiance.done = true;
    }
}
