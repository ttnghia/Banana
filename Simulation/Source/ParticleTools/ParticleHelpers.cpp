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

#include <tbb/tbb.h>

#include <iostream>
#include <string>
#include <sstream>
#include <math.h>

#include <Noodle/Core/Particle/Tools/ParticleUtils.h>
#include <Noodle/Core/Data/DataIO.h>

namespace ParticleHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    Vec2i create_grid(const Vec2& bmin, const Vec2& bmax,
            Real spacing)
    {
        Vec2i grid2d;

        grid2d[0] = (int)floor((bmax[0] - bmin[0]) / spacing);
        grid2d[1] = (int)floor((bmax[1] - bmin[1]) / spacing);

        return grid2d;
    }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    Vec3i create_grid(const Vec3& bmin, const Vec3& bmax,
            Real spacing)
    {
        Vec3i grid3d;

        grid3d[0] = (int)floor((bmax[0] - bmin[0]) / spacing);
        grid3d[1] = (int)floor((bmax[1] - bmin[1]) / spacing);
        grid3d[2] = (int)floor((bmax[2] - bmin[2]) / spacing);

        return grid3d;
    }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void transform(Vec_Vec3& particles, const Vec3& translation,
            const Vec3& rotation)
    {
        Real azimuth = rotation[0];
        Real elevation = rotation[1];
        Real roll = rotation[2];
        Real sinA, cosA, sinE, cosE, sinR, cosR;

        Vec3 R[3];

        sinA = (Real)sin(azimuth);
        cosA = (Real)cos(azimuth);
        sinE = (Real)sin(elevation);
        cosE = (Real)cos(elevation);
        sinR = (Real)sin(roll);
        cosR = (Real)cos(roll);

        R[0][0] = cosR * cosA - sinR * sinA * sinE;
        R[0][1] = sinR * cosA + cosR * sinA * sinE;
        R[0][2] = -sinA * cosE;

        R[1][0] = -sinR * cosE;
        R[1][1] = cosR * cosE;
        R[1][2] = sinE;

        R[2][0] = cosR * sinA + sinR * cosA * sinE;
        R[2][1] = sinR * sinA - cosR * cosA * sinE;
        R[2][2] = cosA * cosE;

        tbb::parallel_for(tbb::blocked_range < size_t > (0, particles.size()),
                [&](tbb::blocked_range < size_t > r)
        {
            for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
            {
                Real tmp[3];

                for(int j = 0; j < 3; ++j)
                {
#ifdef __Using_Eigen_Lib__
                    tmp[j] = R[j].dot(particles[i]);
#else
                    tmp[j] = glm::dot(R[j], particles[i]);
#endif
                }

                particles[i] = Vec3(tmp[0] + translation[0], tmp[1] + translation[1],
                        tmp[2] + translation[2]);
                //            if(particles[i][0] < 0.1)
                //            printf("%d, %s\n", i, NumberHelpers::to_string(particles[i]).c_str());
            }
        }); // end parallel_for
    }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    UInt32 load_binary(const std::string& fileName, Vec_Vec2& particles,
            Real& particle_radius)
    {
        DataBuffer buffer;
        __NOODLE_ASSERT_MSG(FileHelpers::readFile(buffer.buffer(), fileName),
                "Could not open file for reading.");

        UInt32 num_particles;
        UInt32 vector_size;
        float  tmp_radius;
        UInt64 segmentStart = 0;
        UInt64 segmentSize;


        segmentSize = sizeof(UInt32);
        memcpy(&num_particles, &buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        segmentSize = sizeof(float);
        memcpy(&tmp_radius, &buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        segmentSize = sizeof(UInt32);
        memcpy(&vector_size, &buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;
        __NOODLE_ASSERT_EQUAL(num_particles, vector_size);

        float* tmp_particles = new float[num_particles * 2];
        segmentSize = num_particles * sizeof(float) * 2;
        memcpy(tmp_particles, &buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        __NOODLE_ASSERT(segmentStart == buffer.size());
        particles.resize(num_particles);

        for(UInt32 i = 0; i < num_particles; ++i)
        {
            particles[i] = Vec2(tmp_particles[i * 2],
                    tmp_particles[i * 2 + 1]);
        }

        delete[] tmp_particles;
        particle_radius = tmp_radius;

        return num_particles;
    }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    UInt32 load_binary(const std::string& fileName, Vec_Vec3& particles,
            Real& particle_radius)
    {
        DataBuffer buffer;
        __NOODLE_ASSERT_MSG(FileHelpers::readFile(buffer.buffer(), fileName),
                "Could not open file for reading.");

        UInt32 num_particles;
        UInt32 vector_size;
        float  tmp_radius;
        UInt64 segmentStart = 0;
        UInt64 segmentSize;


        segmentSize = sizeof(UInt32);
        memcpy(&num_particles, &buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        segmentSize = sizeof(float);
        memcpy(&tmp_radius, &buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        segmentSize = sizeof(UInt32);
        memcpy(&vector_size, &buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;
        __NOODLE_ASSERT_EQUAL(num_particles, vector_size);

        float* tmp_particles = new float[num_particles * 3];
        segmentSize = num_particles * sizeof(float) * 3;
        memcpy(tmp_particles, &buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        __NOODLE_ASSERT(segmentStart == buffer.size());
        particles.resize(num_particles);

        for(UInt32 i = 0; i < num_particles; ++i)
        {
            particles[i] = Vec3(tmp_particles[i * 3], tmp_particles[i * 3 + 1], tmp_particles[i * 3 + 2]);
        }

        delete[] tmp_particles;
        particle_radius = tmp_radius;

        return num_particles;
    }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace ParticleHelpers
