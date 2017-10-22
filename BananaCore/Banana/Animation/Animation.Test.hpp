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
    tAni.periodic() = true;
    rAni.periodic() = true;
    sAni.periodic() = true;

    tAni.addKeyFrame(100u, Vec3f(0, 1, 0));
    tAni.addKeyFrame(200u, Vec3f(0, 0, 0));
    tAni.makeReady(true, true, true);

    rAni.addKeyFrame(0u, Vec4f(0, 1, 0, 0));
    rAni.addKeyFrame(100u, Vec4f(0, 1, 0, 90));
    rAni.addKeyFrame(200u, Vec4f(0, 1, 0, 0));
    rAni.makeReady(true, false, false);

    sAni.addKeyFrame(100u, 5.0f);
    sAni.addKeyFrame(200u, 1.0f);
    sAni.makeReady(true, true, false);

    //printf("Translation:\n\n");
    //for(UInt frame = 0; frame <= MAX_FRAME; ++frame) {
    //    Vec3f translation;
    //    Vec4f rotation;
    //    float scale;
    //    tAni.getTransformation(translation, rotation, scale, frame, 0);
    //    printf("%u,    %f, %f, %f\n", frame, translation.x, translation.y, translation.z);
    //}

    printf("\n\nRotation:\n\n");
    for(UInt frame = 0; frame <= MAX_FRAME; ++frame) {
        Vec3f translation;
        Vec4f rotation;
        float scale;
        rAni.getTransformation(translation, rotation, scale, frame, 0);
        printf("%u,    %f, %f, %f, %f\n", frame, rotation.x, rotation.y, rotation.z, rotation.w);
    }

    //printf("\n\Scale:\n\n");
    //for(UInt frame = 0; frame <= MAX_FRAME; ++frame) {
    //    Vec3f translation;
    //    Vec4f rotation;
    //    float scale;
    //    sAni.getTransformation(translation, rotation, scale, frame, 0);
    //    printf("%u,    %f\n", frame, scale);
    //}
}