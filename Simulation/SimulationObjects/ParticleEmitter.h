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

#include <Banana/Geometry/GeometryObjects.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class ParticleEmitter
{
public:
    using GeometryPtr = SharedPtr<GeometryObjects::GeometryObject<N, Real> >;
    static constexpr UInt objDimension() noexcept { return static_cast<UInt>(N); }

    ParticleEmitter() = delete;
    ParticleEmitter(const String& geometryType) : m_GeometryObj(GeometryObjectFactory::createGeometry<N, Real>(geometryType)) { __BNN_ASSERT(m_GeometryObj != nullptr); }

    virtual void advanceFrame() {}
    String&      meshFile() { return m_MeshFile; }
    String&      particleFile() { return m_ParticleFile; }
    GeometryPtr& getGeometry() { return m_GeometryObj; }

protected:
    String      m_MeshFile     = "";
    String      m_ParticleFile = "";
    GeometryPtr m_GeometryObj  = nullptr;
};

#if 0
#include <tbb/tbb.h>

#include <Noodle/Core/Global/Setup.h>
#include <Noodle/Core/Global/Enums.h>
#include <Noodle/Core/Global/Constants.h>
#include <Noodle/Core/Global/Setup.h>
#include <Noodle/Core/Math/MathUtils.h>
#include <Noodle/Core/Monitor/Monitor.h>
#include <Noodle/Core/Monitor/Timer.h>
#include <Noodle/Core/Particle/Tools/ParticleUtils.h>
#include <Noodle/Core/LevelSet/SignDistanceField.h>
#include <Noodle/Core/Data/NumberHelpers.h>
#include <Noodle/Core/Array/Array3.h>
#include <Noodle/Core/Array/Array3Utils.h>
#include <Noodle/Core/Particle/ObjectBuilders/ObjectBuilder.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleEmitter
{
public:
    ParticleEmitter(DomainParameters*           domainParams_,
                    ParticleSamplingParameters* samplingParams_,
                    const Array3_Real&          sdf_boundary_,
                    Real                        particle_radius_);

    void setEmitterParams(const Vec3& v0, UInt max_particles_ = 0,
                          UInt max_times_ = 0, Real allow_distance_ = 2.0);
    void makeReady();
    void emit_all_particles(bool status);
    void emitParticles(Vec_Vec3& particles, Vec_Vec3& velocity,
                       Array3_VecUInt& cellParticles);
    bool emission_finished();
    void add_emitter_sdf(Array3_Real& gsdf_grid, Real padding = 0);

protected:
    virtual bool check_all_params() = 0;
    virtual void build_emitter()    = 0;
    // velocity is different for each type of emitter
    virtual void add_velocity(const Vec_Vec3& new_particles, Vec_Vec3& velocity);

    ////////////////////////////////////////////////////////////////////////////////
    void add_new_particles(Vec_Vec3& new_particles, Vec_Vec3& particles,
                           Array3_VecUInt& cellParticles);
    void add_emitter_shape_particles(Vec_Vec3& new_particles, Vec_Vec3& particles,
                                     Array3_VecUInt& cellParticles);
    void add_particles(Vec_Vec3& new_particles, Vec_Vec3& particles,
                       Array3_VecUInt& cellParticles);

    void get_neighbor_particles(Vec_Vec3&             neighbor_particles,
                                const Vec_Vec3&       particles,
                                const Array3_VecUInt& cellParticles,
                                const Vec3i&          cellId);
    Real         get_min_distance(const Vec_Vec3& particles, const Vec3& pos);
    Real         get_min_distance_parallel(const Vec_Vec3& particles, const Vec3& pos);
    Real         get_sdf_cell_size();
    Array3_Real& get_sdf();

    ////////////////////////////////////////////////////////////////////////////////
    Real particle_radius;

    ObjectBuilder* particle_generator;
    Vec_Vec3       emitter_particles;

    Vec3 initial_velocity;
    UInt emitted_times;
    UInt max_times;
    UInt emitted_particles;
    UInt max_particles;

    bool params_initialized;
    bool emitter_ready;
    bool emit_complete_emitter_shape;
    Real allow_distance;
    int  max_iters;

    Monitor                     monitor;
    DomainParameters*           domainParams;
    ParticleSamplingParameters* samplingParams;
    const Array3_Real&          sdf_boundary;
    ;
};
#endif  // PARTICLE_EMITTER_H


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana