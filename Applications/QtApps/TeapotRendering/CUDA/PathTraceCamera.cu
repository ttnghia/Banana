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
#include "Helpers.cuh"
#include "Common.cuh"
#include "Random.cuh"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
rtDeclareVariable(float3,        eye, , );
rtDeclareVariable(float3,        U, , );
rtDeclareVariable(float3,        V, , );
rtDeclareVariable(float3,        W, , );
rtDeclareVariable(float3,        bad_color, , );
rtDeclareVariable(float,         scene_epsilon, , );
rtDeclareVariable(float3,        cutoff_color, , );
rtDeclareVariable(int,           max_depth, , );
rtBuffer<uchar4, 2>              output_buffer;
rtBuffer<float4, 2>              accum_buffer;
rtDeclareVariable(rtObject,      top_object, , );
rtDeclareVariable(unsigned int,  frame, , );
rtDeclareVariable(uint2,         launch_index, rtLaunchIndex, );


RT_PROGRAM void pinhole_camera()
{

  optix::size_t2 screen = output_buffer.size();
  unsigned int seed = tea<16>(screen.x*launch_index.y+launch_index.x, frame);

  // Subpixel jitter: send the ray through a different position inside the pixel each time,
  // to provide antialiasing.
  float2 subpixel_jitter = frame == 0 ? make_float2( 0.0f ) : make_float2(rnd( seed ) - 0.5f, rnd( seed ) - 0.5f);

  float2 d = (make_float2(launch_index) + subpixel_jitter) / make_float2(screen) * 2.f - 1.f;
  float3 ray_origin = eye;
  float3 ray_direction = optix::normalize(d.x*U + d.y*V + W);

  PerRayData_Radiance prd;
  prd.depth = 0;
  prd.seed = seed;
  prd.done = false;

  // These represent the current shading state and will be set by the closest-hit or miss program

  // attenuation (<= 1) from surface interaction.
  prd.reflectance = make_float3( 1.0f );

  // light from a light source or miss program
  prd.radiance = make_float3( 0.0f );

  // next ray to be traced
  prd.origin = make_float3( 0.0f );
  prd.direction = make_float3( 0.0f );

  float3 result = make_float3( 0.0f );

  // Main render loop. This is not recursive, and for high ray depths
  // will generally perform better than tracing radiance rays recursively
  // in closest hit programs.
  for(;;) {
      optix::Ray ray(ray_origin, ray_direction, /*ray type*/ 0, scene_epsilon );
      rtTrace(top_object, ray, prd);

      result += prd.reflectance * prd.radiance;

      if ( prd.done ) {
          break;
      } else if ( prd.depth >= max_depth ) {
        result += prd.reflectance * cutoff_color;
        break;
      }

      prd.depth++;

      // Update ray data for the next path segment
      ray_origin = prd.origin;
      ray_direction = prd.direction;
  }

  float4 acc_val = accum_buffer[launch_index];
  if( frame > 0 ) {
    acc_val = optix::lerp( acc_val, make_float4( result, 0.f ), 1.0f / static_cast<float>( frame+1 ) );
  } else {
    acc_val = make_float4( result, 0.f );
  }
  output_buffer[launch_index] = make_color( make_float3( acc_val ) );
  accum_buffer[launch_index] = acc_val;
}

RT_PROGRAM void exception()
{
  const unsigned int code = rtGetExceptionCode();
  rtPrintf( "Caught exception 0x%X at launch index (%d,%d)\n", code, launch_index.x, launch_index.y );
  output_buffer[launch_index] = make_color( bad_color );
}

