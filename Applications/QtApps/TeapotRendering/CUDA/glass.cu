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
#include "Random.cuh"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
rtDeclareVariable(float3, shading_normal, attribute shading_normal, );
rtDeclareVariable(float3, front_hit_point, attribute front_hit_point, );
rtDeclareVariable(float3, back_hit_point, attribute back_hit_point, );

rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(float, t_hit, rtIntersectionDistance, );

rtDeclareVariable(float,        refraction_index, , );
rtDeclareVariable(float3,       refraction_color, , );
rtDeclareVariable(float3,       reflection_color, , );

// Transmittance at unit distance ( a color with each channel in [0,1] range)
rtDeclareVariable(float3,       unit_transmittance, , );

rtDeclareVariable(PerRayData_Radiance, prd_radiance, rtPayload, );

// -----------------------------------------------------------------------------

static __device__ __inline__ float fresnel( float cos_theta_i, float cos_theta_t, float eta )
{
    const float rs = ( cos_theta_i - cos_theta_t*eta ) /
                     ( cos_theta_i + eta*cos_theta_t );
    const float rp = ( cos_theta_i*eta - cos_theta_t ) /
                     ( cos_theta_i*eta + cos_theta_t );

    return 0.5f * ( rs*rs + rp*rp );
}

static __device__ __inline__ float3 logf( float3 v )
{
    return make_float3( logf(v.x), logf(v.y), logf(v.z) );
}

// -----------------------------------------------------------------------------

RT_PROGRAM void closest_hit_radiance()
{
    const float3 w_out = -ray.direction;
    float3 normal = optix::normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, shading_normal));
    float cos_theta_i = optix::dot( w_out, normal );

    float eta;
    float3 transmittance = make_float3( 1.0f );
    if( cos_theta_i > 0.0f ) {
        // Ray is entering
        eta = refraction_index;  // Note: does not handle nested dielectrics
    } else {
        // Ray is exiting; apply Beer's Law.
        // This is derived in Shirley's Fundamentals of Graphics book.
        // The "unit_transmittance" is transmittance at unit distance and must
        // be between 0 and 1, so that log(...) is negative.
        transmittance = optix::expf( logf(unit_transmittance) * t_hit );
        eta         = 1.0f / refraction_index;
        cos_theta_i = -cos_theta_i;
        normal      = -normal;
    }

    float3 w_t;
    const bool tir           = !optix::refract( w_t, -w_out, normal, eta );

    const float cos_theta_t  = -optix::dot( normal, w_t );
    const float R            = tir  ?
                               1.0f :
                               fresnel( cos_theta_i, cos_theta_t, eta );

    // Importance sample the Fresnel term
    const float z = rnd( prd_radiance.seed );
    if( z <= R ) {
        // Reflect
        const float3 w_in = optix::reflect( -w_out, normal );
        const float3 fhp = rtTransformPoint(RT_OBJECT_TO_WORLD, front_hit_point);
        prd_radiance.origin = fhp;
        prd_radiance.direction = w_in;
        prd_radiance.reflectance *= reflection_color*transmittance;
    } else {
        // Refract
        const float3 w_in = w_t;
        const float3 bhp = rtTransformPoint(RT_OBJECT_TO_WORLD, back_hit_point);
        prd_radiance.origin = bhp;
        prd_radiance.direction = w_in;
        prd_radiance.reflectance *= refraction_color*transmittance;
    }

    // Note: we do not trace the ray for the next bounce here, we just set it up for
    // the ray-gen program using per-ray data.

}


