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
void SphereBuilder::set_sphere_params(const Vec3& center, Real radius)
{
    sphere_center = center;
    sphere_radius = radius;

    ////////////////////////////////////////////////////////////////////////////////
    monitor.printLog("Set sphere center: " + NumberHelpers::toString(
        sphere_center) + ", radius: " +
        NumberHelpers::formatWithCommas(sphere_radius, 5));

    sphere_initialized = true;
    invalidate_data();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SphereBuilder::pos_process(Vec_Vec3& particles, Real particle_radius,
                                Real threshold)
{
    // move the particle near sphere surface to surface
    for(Vec3& pos : particles)
    {
        for(int i = 0; i < 3; ++i)
        {
            const Vec3 r = pos - sphere_center;

#ifdef __Using_Eigen_Lib__

            if(fabs(r.norm() - sphere_radius + particle_radius) <
               threshold * particle_radius)
            {
                pos = sphere_center + (sphere_radius - particle_radius) * r.normalized();
#else

            if(fabs(glm::length(r) - sphere_radius + particle_radius) <
               threshold * particle_radius)
            {
                pos = sphere_center + (sphere_radius - particle_radius) * glm::normalize(r);
#endif
            }

            }
        }
    }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SphereBuilder::compute_aabb()
{
    obj_bbmin = sphere_center - Vec3(1, 1, 1) * (Real)(sphere_radius * 1.01);
    obj_bbmax = sphere_center + Vec3(1, 1, 1) * (Real)(sphere_radius * 1.01);

    aabb_ready = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real SphereBuilder::object_sdf_value(const Vec3& pos)
{
    return SignDistanceField::sign_distance_sphere(pos, sphere_center, sphere_radius);
}




//    SphereBuilder* objBuilder = new SphereBuilder(simParams, globalParams);
//    objBuilder->setSphere(Vec3(1.0, 1.0, 1.0), 0.8);
//    objBuilder->initPDObject(pdSimulator.pdPosition,
//                             pdSimulator.pdVelocity);
