//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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

#include <catch.hpp>
#include <Banana/Animation/Animation.h>

using namespace Banana;

#define MAX_FRAME 1000u
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test Animation", "[Test Animation]")
{
    Animation<3, float> tAni;
    Animation<3, float> rAni;
    Animation<3, float> sAni;
    tAni.setPeriodic(true);
    rAni.setPeriodic(true);
    sAni.setPeriodic(true, 100u);

    tAni.addKeyFrame(100u, Vec3f(0, 1, 0));
    tAni.addKeyFrame(200u, Vec3f(0, 0, 0));
    tAni.makeReady(true, true, true);

    rAni.addKeyFrame(0u,   Vec4f(0, 1, 0, 0));
    rAni.addKeyFrame(100u, Vec4f(0, 1, 0, 90));
    rAni.addKeyFrame(200u, Vec4f(0, 1, 0, 0));
    rAni.makeReady(true, false, false);

    sAni.addKeyFrame(100u, 1.0f);
    sAni.addKeyFrame(200u, 5.0f);
    sAni.addKeyFrame(300u, 1.0f);
    sAni.makeReady(true, true, false);

    //printf("Translation:\n\n");
    //for(UInt frame = 0; frame <= MAX_FRAME; ++frame) {
    //    Vec3f translation;
    //    Vec4f rotation;
    //    float scale;
    //    tAni.getTransformation(translation, rotation, scale, frame, 0);
    //    printf("%u,    %f, %f, %f\n", frame, translation.x, translation.y, translation.z);
    //}

    //printf("\n\nRotation:\n\n");
    //for(UInt frame = 0; frame <= MAX_FRAME; ++frame) {
    //    Vec3f translation;
    //    Vec4f rotation;
    //    float scale;
    //    rAni.getTransformation(translation, rotation, scale, frame, 0);
    //    printf("%u,    %f, %f, %f, %f\n", frame, rotation.x, rotation.y, rotation.z, rotation.w);
    //}

    printf("\n\nScale:\n\n");
    for(UInt frame = 0; frame <= MAX_FRAME; ++frame) {
        Vec3f translation;
        Vec4f rotation;
        float scale;
        sAni.getTransformation(translation, rotation, scale, frame, 0);
        printf("%u,    %f\n", frame, scale);
    }
}