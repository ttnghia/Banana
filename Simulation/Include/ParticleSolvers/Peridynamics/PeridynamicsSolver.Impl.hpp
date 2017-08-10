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
void PeridynamicsSolver::make_ready()
{
    Timer timer;
    timer.tick();

    ////////////////////////////////////////////////////////////////////////////////

    if(particles_t0.size() < particles.size())
    {
        size_t old_size = particles_t0.size();
        particles_t0.resize(particles.size());
        std::copy(particles.begin() + old_size, particles.end(), particles_t0.begin() + old_size);
    }

    if(bond_list_t0.size() < bond_list.size())
    {
        size_t old_size = bond_list_t0.size();
        bond_list_t0.resize(bond_list.size());

        for(size_t i = old_size; i < bond_list.size(); ++i)
        {
            bond_list_t0[i].resize(bond_list[i].size());
            std::copy(bond_list[i].begin(), bond_list[i].end(), bond_list_t0[i].begin());
        }
    }

    if(stretch_limit_t0.size() < stretch_limit.size())
    {
        size_t old_size = stretch_limit_t0.size();
        stretch_limit_t0.resize(stretch_limit.size());
        std::copy(stretch_limit.begin() + old_size, stretch_limit.end(),
                  stretch_limit_t0.begin() + old_size);
    }

    if(particleParams->predict_velocity_for_collision)
    {
        predicted_velocity.resize(particles.size());
    }

    particle_forces.resize(particles.size());
    new_stretch_limit.resize(particles.size());

    matrix.resize(particleParams->num_active_particles);
    rhs.resize(particleParams->num_active_particles);
    solution.resize(particleParams->num_active_particles);

    if(merlit_tool != nullptr)
    {
        merlit_tool->indexer->update_solver_particles();
        merlit_tool->indexer->backup_particle_mass();
    }

    ////////////////////////////////////////////////////////////////////////////////
    timer.tock();
    monitor.print_log("Allocate solver memory: " + timer.get_run_time());

    ////////////////////////////////////////////////////////////////////////////////
    if(bond_d0.size() < bond_list.size())
    {
        timer.tick();
        size_t old_size = bond_d0.size();
        bond_d0.resize(bond_list.size());

        PeridynamicsUtils::compute_bond_length(particles_t0, bond_list, bond_d0, old_size);
        timer.tock();
        monitor.print_log("Calculate d0: " + timer.get_run_time());
    }

    ////////////////////////////////////////////////////////////////////////////////
    solver_initalized = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::advance_velocity(Real timestep, const MergingSplittingData* merlit_data)
{
    if(particleParams->num_active_particles <= 0)
    {
        return;
    }

    __NOODLE_ASSERT(solver_initalized);

    static Timer timer;

    ////////////////////////////////////////////////////////////////////////////////

    if(has_gravity)
    {
        timer.tick();
        add_gravity(timestep);
        timer.tock();
        monitor.print_log("Add grativy: " + timer.get_run_time());
    }

    if(!re_integration)
    {
        remove_broken_bonds();
    }

    clear_broken_bond_list();

    ////////////////////////////////////////////////////////////////////////////////
    switch(integration_scheme)
    {
        case IntegrationScheme::ImplicitEuler:
            monitor.print_log("Start Implicit Euler velocity integration:");
            break;

        case IntegrationScheme::ImplicitNewmarkBeta:
            monitor.print_log("Start Newmark Beta velocity integration:");
            break;

        default:
            __NOODLE_INVALID_SWITCH_DEFAULT_VALUE
    }

    ////////////////////////////////////////////////////////////////////////////////
    Real integration_time = 0;
    timer.tick();

    static Vec_Vec3 velocity_old;

    if(integration_scheme == IntegrationScheme::ImplicitNewmarkBeta)
    {
        velocity_old.resize(particleParams->num_active_particles);

        std::copy(velocity.begin(), velocity.begin() + particleParams->num_active_particles, velocity_old.begin());
    }


    integration_time += timer.tock();

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    build_linear_system(timestep, merlit_data);
    integration_time += timer.tock();
    monitor.print_log_indent("Build linear system: " + timer.get_run_time());

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    if(re_integration)
        cgSolver.setZeroInitial(false);
    else
        cgSolver.setZeroInitial(true);
    solve_system();
    integration_time += timer.tock();

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    update_velocity();

    if(integration_scheme == IntegrationScheme::ImplicitNewmarkBeta)
    {
        tbb::parallel_for(tbb::blocked_range<UInt32>(0, particleParams->num_active_particles), [&](tbb::blocked_range<UInt32> r)
                          {
                              for(UInt32 p = r.begin(); p != r.end(); ++p)
                              {
                                  velocity[p] = (velocity[p] + velocity_old[p]) * (Real)0.5;
                              }
                          }); // end parallel_for
    }

    integration_time += timer.tock();
    monitor.print_log_indent("Update velocity: " + timer.get_run_time());



    //    if(collision_method == InteractionMethod::MergingSplitting
    //       && merlit_tool->has_particle_in_merge_list)
    //    {

    //        PRINT_LINE;
    //        matrix.print_debug();

    //        DataDebug::print(rhs, "rhs");
    //        DataDebug::print(solution, "solution");

    //        DataDebug::print(velocity_old, "velocity_old");
    //        DataDebug::print(velocity, "velocity");
    //    }

    ////////////////////////////////////////////////////////////////////////////////
    monitor.print_log("Time integration total: " + NumberUtils::format_with_commas(integration_time) + "ms");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::predict_velocity_explicit(Real timestep)
{
    compute_explicit_forces(nullptr);

    static Timer timer;
    const Real   beta = timestep;

    timer.tick();
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, particleParams->num_active_particles), [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 p = r.begin(); p != r.end(); ++p)
                          {
                              predicted_velocity[p] = velocity[p] + particle_forces[p] / particle_mass[p] * beta;
                          }
                      }); // end parallel_for

    static bool filled = false;

    if(!filled)
    {
        std::fill(predicted_velocity.begin() + particleParams->num_active_particles, predicted_velocity.end(), Vec3(0));
        filled = true;
    }

    //timer.tock();
    monitor.print_log_indent("Predict velocity: " + timer.get_run_time());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::predict_velocity_implicit(Real timestep)
{
    const float  beta = timestep;
    static Timer timer;

    timer.tick();
    clear_broken_bond_list();
    build_linear_system(beta, nullptr);
    timer.tock();
    monitor.print_log_indent("Build linear system: " + timer.get_run_time());

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    cgSolver.enableZeroInitial();
    solve_system();
    timer.tock();

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();

    tbb::parallel_for(tbb::blocked_range<UInt32>(0, particleParams->num_active_particles), [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 p = r.begin(); p != r.end(); ++p)
                          {
                              Vec3 tmp = (integration_scheme == IntegrationScheme::ImplicitNewmarkBeta) ? (solution[p] * (Real)0.5) : solution[p];
                              predicted_velocity[p] = velocity[p] + tmp;
                          }
                      }); // end parallel_for


    timer.tock();
    monitor.print_log_indent("Update predict velocity: " + timer.get_run_time());



    static bool filled = false;

    if(!filled)
    {
        std::fill(predicted_velocity.begin() + particleParams->num_active_particles, predicted_velocity.end(), Vec3(0.0));
        filled = true;
    }

    monitor.print_log_indent("Predict velocity finished.");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::pos_process_per_step(Real timestep)
{
    ParticleSolver::pos_process_per_step(timestep);
    ////////////////////////////////////////////////////////////////////////////////
    // check for potential collision

    // if all potential collision particles are not ok (they penetrate deeper), then go back
    static Timer timer;
    timer.tick();
    re_integration = check_potential_collision();
    timer.tock();

    if(re_integration)
    {
        monitor.print_log("Potential collision confirmed. Go back and re-integrate. Check time: " + timer.get_run_time());

        monitor.print_log("Restore particle position and velocity");
        restore_particles();
        restore_velocity();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::pos_process_per_frame(int frame)
{
    ParticleSolver::pos_process_per_frame(frame);

    ////////////////////////////////////////////////////////////////////////////////
    if(connected_component_analysis)
    {
        find_connected_components();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::move_particles(Real timestep)
{
#if 0
    ParticleSolver::move_particles(timestep);
#else
    static const Real margin      = particle_radius;
    static const Vec3 lower_bound = moving_bmin + Vec3(1, 1, 1) * margin;
    static const Vec3 upper_bound = moving_bmax - Vec3(1, 1, 1) * margin;

    static Timer timer;
    timer.tick();

    tbb::parallel_for(tbb::blocked_range<UInt32>(0,
                                                 particleParams->num_active_particles),
                      [&, timestep](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 p = r.begin(); p != r.end(); ++p)
                          {
                              const Vec3& pvel = velocity[p];

                              if(!(isnan(pvel[0]) || isnan(pvel[1]) || isnan(pvel[2])))
                              {
                                  const bool p_merged = (merlit_tool != nullptr) ?
                                                        merlit_tool->merge_data->merged[p] > 0 : false;

                                  if(!p_merged)
                                  {
                                      const Vec3& ppos = particles[p];
                                      Vec3 new_vel;
                                      Vec3 new_pos;

                                      bool velocity_changed = ParticleUtils::move_particle_constrained(ppos, pvel, new_pos, new_vel, lower_bound, upper_bound, timestep, reflect_boundary);
                                      // position correction
                                      particles[p] = Vec3(std::min(upper_bound[0], std::max(new_pos[0], lower_bound[0])),
                                                          std::min(upper_bound[1], std::max(new_pos[1], lower_bound[1])),
                                                          std::min(upper_bound[2], std::max(new_pos[2], lower_bound[2])));

                                      if(velocity_changed)
                                      {
                                          velocity[p] = new_vel;
                                      }
                                  } // not merge
                                  else
                                  {
                                      // move merged particle by the root of mlist, not by itself
                                      const Vec_UInt& mlist = merlit_tool->merge_data->merge_list[p];

                                      if(mlist.size() == 0)
                                      {
                                          continue;
                                      }

                                      // all the particle in mlist have the same velocity
                                      Vec3 new_vel;
                                      Vec_Vec3 old_pos;
                                      Vec_Vec3 new_pos;
                                      old_pos.resize(mlist.size());

                                      for(size_t i = 0; i < mlist.size(); ++i)
                                      {
                                          const UInt32 q = mlist[i];
                                          old_pos[i] = particles[q];
                                      }

                                      bool velocity_changed = ParticleUtils::move_group_particles_constrained(old_pos, new_pos, pvel, new_vel, lower_bound, upper_bound, timestep, reflect_boundary);

                                      for(size_t i = 0; i < mlist.size(); ++i)
                                      {
                                          const UInt32 q = mlist[i];
                                          //                        particles[q] = new_pos[i];
                                          particles[q] = Vec3(std::min(upper_bound[0], std::max(new_pos[i][0], lower_bound[0])),
                                                              std::min(upper_bound[1], std::max(new_pos[i][1], lower_bound[1])),
                                                              std::min(upper_bound[2], std::max(new_pos[i][2], lower_bound[2])));
                                      }

                                      if(velocity_changed)
                                      {
                                          velocity[p] = new_vel;
                                      }
                                  } // end if merged
                              }     // end velocity all finite
                              else
                              {
                                  monitor.print_warning("Velocity has nan values");
                              }
                          }
                      }); // end parallel_for

    timer.tock();

    monitor.print_log("Move particles: " + timer.get_run_time());
#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// particles colliding only they don't have bond
bool PeridynamicsSolver::are_colliding(UInt32 p, UInt32 q)
{
#if 0
    const Vector_UInt& pbonds = bond_list[p];

    // colliding if no bond found
    return !(std::binary_search(pbonds.begin(), pbonds.end(), q));
#else
    __NOODLE_UNUSED(p);
    __NOODLE_UNUSED(q);
    return true;
#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::save_frame(int frame)
{
    for(auto item : dataIOs)
    {
        DataFile data_file = item.first;
        DataIO*  dataIO    = dynamic_cast<DataIO*>(item.second);
        __NOODLE_ASSERT(dataIO != nullptr);

        switch(data_file)
        {
            case DataFile::FramePosition:
            {
                dataIO->reset_buffer();
                dataIO->getBuffer().push_back(static_cast<UInt32>(particles.size()));
                dataIO->getBuffer().push_back(static_cast<UInt32>(particleParams->num_active_particles));
                dataIO->getBuffer().push_back_to_float(particle_radius);
                dataIO->getBuffer().push_back_to_float_array(particles, false);
                dataIO->flush_buffer_async(frame);
            }
            break;

            case DataFile::FrameVelocity:
            {
                dataIO->reset_buffer();
                dataIO->getBuffer().push_back_to_float_array(velocity);
                dataIO->flush_buffer_async(frame);
            }
            break;

            case DataFile::FrameParticleMarker:
            {
                dataIO->reset_buffer();
                dataIO->getBuffer().push_back(static_cast<UInt32>(particles.size()));
                dataIO->getBuffer().push_back(static_cast<UInt32>(particleParams->num_components));
                dataIO->getBuffer().push_back(particle_marker, false);
                dataIO->flush_buffer_async(frame);
            }
            break;

            case DataFile::FrameParticleConnectionList:
            {
                dataIO->reset_buffer();
                dataIO->getBuffer().push_back(bond_list);
                dataIO->flush_buffer_async(frame);
            }
            break;

            case DataFile::FrameRemaingConnectionRatio:
            {
                compute_remaining_bond_ratio();
                dataIO->reset_buffer();
                dataIO->getBuffer().push_back_to_float_array(remaining_bond_ratio);
                dataIO->flush_buffer_async(frame);
            }
            break;

            default:
                ; // nothing
        } // end switch
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::save_state(int frame)
{
    for(auto item : dataIOs)
    {
        DataFile data_file = item.first;
        DataIO*  dataIO    = dynamic_cast<DataIO*>(item.second);
        __NOODLE_ASSERT(dataIO != nullptr);

        switch(data_file)
        {
            case DataFile::StatePosition:
            {
                dataIO->reset_buffer();
                dataIO->getBuffer().push_back(static_cast<UInt32>(particles.size()));
                dataIO->getBuffer().push_back(static_cast<UInt32>(particleParams->num_active_particles));
                dataIO->getBuffer().push_back(particle_radius);
                dataIO->getBuffer().push_back(particles, false);
                dataIO->flush_buffer_async(frame);
            }
            break;

            case DataFile::StateVelocity:
            {
                dataIO->reset_buffer();
                dataIO->getBuffer().push_back(velocity);
                dataIO->flush_buffer_async(frame);
            }
            break;

            case DataFile::StateMass:
            {
                dataIO->reset_buffer();
                dataIO->getBuffer().push_back(particle_mass);
                dataIO->flush_buffer_async(frame);
            }
            break;

            case DataFile::StateStretchThreshold:
            {
                dataIO->reset_buffer();
                dataIO->getBuffer().push_back(stretch_limit);
                dataIO->flush_buffer_async(frame);
            }
            break;

            case DataFile::FrameParticleConnectionList:
            {
                if(frame == 0)
                {
                    dataIO->reset_buffer();
                    dataIO->getBuffer().push_back(bond_list);
                    dataIO->flush_buffer_async(frame);
                }
            }

            break;


            default:
                ; // nothing
        } // end switch
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int PeridynamicsSolver::load_latest_state()
{
    __NOODLE_CHECK_MAP_ITEM(dataIOs, DataFile::StatePosition);
    __NOODLE_CHECK_MAP_ITEM(dataIOs, DataFile::StateVelocity);
    __NOODLE_CHECK_MAP_ITEM(dataIOs, DataFile::StateMass);
    __NOODLE_CHECK_MAP_ITEM(dataIOs, DataFile::StateStretchThreshold);
    __NOODLE_CHECK_MAP_ITEM(dataIOs, DataFile::FrameParticleMarker);
    __NOODLE_CHECK_MAP_ITEM(dataIOs, DataFile::FrameParticleConnectionList);

    ////////////////////////////////////////////////////////////////////////////////
    int latest_frame = get_latest_state_index();

    if(latest_frame < 0)
    {
        return latest_frame;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // position
    Real particle_radius;
    dataIOs[DataFile::StatePosition]->load_file_index(latest_frame);
    dataIOs[DataFile::StatePosition]->getBuffer().get_data<Real>(particle_radius, 2 * sizeof(UInt32));
    __NOODLE_ASSERT_APPROX_NUMBERS(particleParams->particle_radius, particle_radius, 1e-8);

    dataIOs[DataFile::StatePosition]->getBuffer().get_data<UInt32>(particleParams->num_particles, 0);
    dataIOs[DataFile::StatePosition]->getBuffer().get_data<UInt32>(particleParams->num_active_particles, sizeof(UInt32));
    dataIOs[DataFile::StatePosition]->getBuffer().get_data<Real>(particles, 2 * sizeof(UInt32) + sizeof(Real), particleParams->num_particles);

    // position at t=0
    dataIOs[DataFile::StatePosition]->load_file_index(0);
    dataIOs[DataFile::StatePosition]->getBuffer().get_data<Real>(particles_t0, 2 * sizeof(UInt32) + sizeof(Real), particleParams->num_particles);

    ////////////////////////////////////////////////////////////////////////////////
    // velocity
    dataIOs[DataFile::StateVelocity]->load_file_index(latest_frame);
    dataIOs[DataFile::StateVelocity]->getBuffer().get_data<Real>(velocity);
    __NOODLE_ASSERT(velocity.size() == particles.size());

    ////////////////////////////////////////////////////////////////////////////////
    // particle mass
    dataIOs[DataFile::StateMass]->load_file_index(latest_frame);
    dataIOs[DataFile::StateMass]->getBuffer().get_data<Real>(particle_mass);
    __NOODLE_ASSERT(particle_mass.size() == particles.size());

    ////////////////////////////////////////////////////////////////////////////////
    // stretch threshold
    dataIOs[DataFile::StateStretchThreshold]->load_file_index(latest_frame);
    dataIOs[DataFile::StateStretchThreshold]->getBuffer().get_data<Real>(stretch_limit);
    __NOODLE_ASSERT(stretch_limit.size() == particles.size());

    ////////////////////////////////////////////////////////////////////////////////
    // bond list at latest state and state t=0
    dataIOs[DataFile::FrameParticleConnectionList]->load_file_index(latest_frame);
    dataIOs[DataFile::FrameParticleConnectionList]->getBuffer().get_data(bond_list);
    __NOODLE_ASSERT(bond_list.size() == particles.size());

    dataIOs[DataFile::FrameParticleConnectionList]->load_file_index(0);
    dataIOs[DataFile::FrameParticleConnectionList]->getBuffer().get_data(bond_list_t0);
    __NOODLE_ASSERT(bond_list_t0.size() == particles.size());

    ////////////////////////////////////////////////////////////////////////////////
    // marker
    dataIOs[DataFile::FrameParticleMarker]->load_file_index(latest_frame);
    dataIOs[DataFile::FrameParticleMarker]->getBuffer().get_data<Int8>(particle_marker);
    __NOODLE_ASSERT(particle_marker.size() == particles.size());


    // calculate d0, using current bond list, not bond_list_t0
//    Timer timer;
//    timer.tick();
//    bond_d0.resize(bond_list.size());
//    PeridynamicsUtils::compute_bond_length(particles_t0, bond_list, bond_d0);
//    timer.tock();
//    monitor.print_log("Calculate d0: " + timer.get_run_time());

    ////////////////////////////////////////////////////////////////////////////////
    // init memory
//    init_solver();

    ////////////////////////////////////////////////////////////////////////////////

    return latest_frame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int PeridynamicsSolver::get_latest_state_index()
{
    int latest_frame = dataIOs[DataFile::StatePosition]->find_latest_file_index(
        params->sim_params()->final_frame);

    if(latest_frame < 0)
    {
        return -1;
    }

    if(!FileHelpers::file_existed(dataIOs[DataFile::StateVelocity]->get_file_name(latest_frame)))
    {
        return -1;
    }

    if(!FileHelpers::file_existed(dataIOs[DataFile::StateMass]->get_file_name(latest_frame)))
    {
        return -1;
    }

    if(!FileHelpers::file_existed(dataIOs[DataFile::StateStretchThreshold]->get_file_name(latest_frame)))
    {
        return -1;
    }

    if(!FileHelpers::file_existed(dataIOs[DataFile::FrameParticleMarker]->get_file_name(latest_frame)))
    {
        return -1;
    }

    if(!FileHelpers::file_existed(dataIOs[DataFile::FrameParticleConnectionList]->get_file_name(0)))
    {
        return -1;
    }

    if(!FileHelpers::file_existed(dataIOs[DataFile::FrameParticleConnectionList]->get_file_name(latest_frame)))
    {
        return -1;
    }

    return latest_frame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::save_state_t0()
{
    save_state(0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::setup_data_io()
{
    ////////////////////////////////////////////////////////////////////////////////
    // state data
    dataIOs[DataFile::StatePosition]         = new DataIO(params->dataIO_params()->data_path, "PDState", "state", "pos");
    dataIOs[DataFile::StateVelocity]         = new DataIO(params->dataIO_params()->data_path, "PDState", "state", "vel");
    dataIOs[DataFile::StateMass]             = new DataIO(params->dataIO_params()->data_path, "PDState", "state", "mass");
    dataIOs[DataFile::StateStretchThreshold] = new DataIO(params->dataIO_params()->data_path, "PDState", "state", "stretch");

    ////////////////////////////////////////////////////////////////////////////////
    // frame data
    dataIOs[DataFile::FramePosition]               = new DataIO(params->dataIO_params()->data_path, "PDFrame", "frame", "pos");
    dataIOs[DataFile::FrameParticleMarker]         = new DataIO(params->dataIO_params()->data_path, "PDFrame", "frame", "mkr");
    dataIOs[DataFile::FrameParticleConnectionList] = new DataIO(params->dataIO_params()->data_path, "PDFrame", "frame", "bond");


    if(StdUtils::vector_has_item(particleParams->optional_output, DataFile::FrameVelocity))
    {
        dataIOs[DataFile::FrameVelocity] = new DataIO(params->dataIO_params()->data_path, "PDFrame", "frame", "vel");
    }

    if(StdUtils::vector_has_item(particleParams->optional_output, DataFile::FrameRemaingConnectionRatio))
    {
        dataIOs[DataFile::FrameRemaingConnectionRatio] = new DataIO(params->dataIO_params()->data_path, "PDFrame", "frame", "brt");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::compute_cfl_timestep()
{
    Real max_vel = compute_max_vel();

    cfl_timestep = std::min(std::max(max_vel > SMALL_NUMBER ? 2.0 * particle_radius / max_vel : BIG_NUMBER, default_timestep * 0.1),
                            default_timestep * 3.0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::build_linear_system(Real dt, const MergingSplittingData* merlit_data)
{
    const Real rhs_coeff = (integration_scheme == IntegrationScheme::ImplicitNewmarkBeta) ? 2.0 : 1.0;
    matrix.zero();
    rhs.assign(rhs.size(), Vec3(0));

    ////////////////////////////////////////////////////////////////////////////////
    tbb::parallel_for(tbb::blocked_range<UInt32>(0,
                                                 particleParams->num_active_particles),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 p = r.begin(); p != r.end(); ++p)
                          {
                              if(!is_active(p))
                              {
                                  continue;
                              }

                              Mat3x3 sumLHS = Mat3x3(0.0);
                              Vec3 sumRHS = Vec3(0.0);
                              Vec3 pforce = Vec3(0.0);

                              if(merlit_data == nullptr)
                              {
                                  compute_particle_forces(sumLHS, sumRHS, pforce, nullptr, p, false, dt);
                              }
                              else
                              {
                                  const bool merged = merlit_data->merged[p] > 0;

                                  if(!merged)
                                  {
                                      compute_particle_forces(sumLHS, sumRHS, pforce, merlit_data, p, false, dt);
                                  }
                                  else
                                  {
                                      if(merlit_data->merge_list[p].size() == 0)
                                      {
                                          continue;
                                      }

                                      __NOODLE_ASSERT(merlit_data->merge_list[p].size() > 1);

                                      // process the merge list, including the root and the leaf
                                      for(UInt32 q : merlit_data->merge_list[p])
                                      {
                                          __NOODLE_ASSERT(merlit_data->merged[q] > 0);
                                          compute_particle_forces(sumLHS, sumRHS, pforce, merlit_data, q, true, dt);
                                      }
                                  }
                              }

                              // add the mass matrix
                              sumLHS += Mat3x3(1.0) * particle_mass[p];
                              matrix.set_element(p, p, sumLHS);

                              // calculate right hand side
                              rhs[p] = sumRHS * rhs_coeff + pforce * dt;
                          }
                      }); // end parallel_for
    //    matrix.print_debug();
    //matrix.check_symmetry();


//    exit(0);
    std::copy(new_stretch_limit.begin(), new_stretch_limit.end(), stretch_limit.begin());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::compute_particle_forces(Mat3x3& sumLHS, Vec3& sumRHS, Vec3& pforce, const MergingSplittingData* merlit_data,
                                                 UInt32 p, bool p_merged, Real dt)
{
    const Real fdx_coeff = (integration_scheme == IntegrationScheme::ImplicitNewmarkBeta) ? MathUtils::sqr(dt) / 4.0 : MathUtils::sqr(dt);
    const Real fdv_coeff = (integration_scheme == IntegrationScheme::ImplicitNewmarkBeta) ? dt * 0.5 : dt;


    const UInt32 p_merged_to = p_merged ? merlit_data->merge_to[p] : p;
    const Vec3&  pvel        = p_merged ? velocity[p_merged_to] : velocity[p];

    const Vec3& ppos = particles[p];

    const Vec_UInt& pbonds = bond_list[p];
    const Vec_Real& pd0    = bond_d0[p];
    __NOODLE_ASSERT(pbonds.size() == pd0.size());

    const Real bthreshold = stretch_limit[p];

    Vec3 spring_force(0, 0, 0);
    Vec3 damping_force(0, 0, 0);
    Real smin = 0.0;
    Real ss0;

    for(UInt32 bondIndex = 0; bondIndex < pbonds.size(); ++bondIndex)
    {
        const UInt32 q = pbonds[bondIndex];
        __NOODLE_ASSERT(q != p);

        const bool   q_merged    = (merlit_data != nullptr) ? merlit_data->merged[q] > 0 : false;
        const UInt32 q_merged_to = q_merged ? merlit_data->merge_to[q] : q;

        // do not accumulate force between two merged particles
        if(q_merged && (p == q_merged_to || q == p_merged_to))
        {
            continue;
        }

        Vec3 eij = particles[q] - ppos;
        Real dij = glm::length(eij);

        if(dij > SMALL_NUMBER)
        {
            eij /= dij;
        }

        Real d0     = pd0[bondIndex];
        Real drdivd = dij / d0 - 1.0;

        if(is_active(p) && is_active(q) && drdivd > std::max(bthreshold, stretch_limit[q]))
        {
            broken_bonds[p].push_back(bondIndex);
            continue; // continue to the next loop without change the walker
        }

        Real vscale = 1.0;

        if(d0 > (horizon - particle_radius))
        {
            vscale = 0.5 + (horizon - d0) / 2.0 / particle_radius;
        }

        if(drdivd < smin)
        {
            smin = drdivd;
        }

        spring_force += (drdivd * vscale) * eij;


        const Vec3 qvel   = q_merged ? velocity[q_merged_to] : velocity[q];
        const Vec3 relVel = qvel - pvel;
        damping_force += glm::dot(eij, relVel) * eij;

        // calculate force derivative
        Mat3x3 fDx;
        Mat3x3 fDv;
        compute_force_derivative(eij, dij, d0, fDx, fDv);

        fDx *= fdx_coeff;
        fDv *= fdv_coeff;

        sumRHS -= (fDx * relVel);

        fDx    += fDv;
        sumLHS -= fDx;

        if(is_active(p_merged_to) && is_active(q_merged_to))
        {
            matrix.add_to_element(p_merged_to, q_merged_to, fDx);
        }
    }

    ss0                  = stretch_limit_t0[p];
    new_stretch_limit[p] = ss0 - 0.25 * smin;
    pforce              += spring_constant * spring_force + damping_constant * damping_force;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::compute_explicit_forces(const MergingSplittingData*)
{
    static Timer timer;
    timer.tick();

    tbb::parallel_for(tbb::blocked_range<UInt32>(0, particleParams->num_active_particles),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 p = r.begin(); p != r.end(); ++p)
                          {
                              if(!is_active(p))
                              {
                                  continue;
                              }

                              Vec3 spring_force(0, 0, 0);
                              Vec3 damping_force(0, 0, 0);

                              const Vec3& ppos = particles[p];
                              const Vec3& pvel = velocity[p];
                              const Real bthreshold = stretch_limit[p];

                              const Vec_UInt& pbonds = bond_list[p];
                              const Vec_Real& pd0 = bond_d0[p];
                              __NOODLE_ASSERT(pbonds.size() == pd0.size());

                              for(UInt32 bondIndex = 0; bondIndex < pbonds.size(); ++bondIndex)
                              {
                                  const UInt32 q = pbonds[bondIndex];
                                  __NOODLE_ASSERT(q != p);

                                  Vec3 eij = particles[q] - ppos;
                                  Real dij = glm::length(eij);

                                  if(dij > SMALL_NUMBER)
                                  {
                                      eij /= dij;
                                  }

                                  Real d0 = pd0[bondIndex];
                                  Real drdivd = dij / d0 - 1.0;

                                  if(is_active(p) && is_active(q) && drdivd > std::max(bthreshold, stretch_limit[q]))
                                  {
                                      continue; // continue to the next loop without change the walker
                                  }

                                  Real vscale = 1.0;

                                  if(d0 > (horizon - particle_radius))
                                  {
                                      vscale = 0.5 + (horizon - d0) / 2.0 / particle_radius;
                                  }

                                  spring_force += (drdivd * vscale) * eij;


                                  const Vec3 qvel = velocity[q];
                                  const Vec3 relVel = qvel - pvel;

                                  damping_force += glm::dot(eij, relVel) * eij;
                              }

                              particle_forces[p] = spring_constant * spring_force + damping_constant * damping_force;
                          }
                      });

    timer.tock();
    monitor.print_log_indent("Compute all particle forces: " + timer.get_run_time());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::compute_force_derivative(const Vec3& eij, Real dij, Real d0, Mat3x3& springDx, Mat3x3& dampingDx)
{
    const Mat3x3 xijxijT = glm::outerProduct(eij, eij);

    dampingDx = xijxijT * (-damping_constant);
    springDx  = Mat3x3(1.0) * (dij / d0 - 1.0) + xijxijT;
    springDx *= (-spring_constant / dij);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::solve_system()
{
    static Timer timer;
    Real         tolerance;
    int          iterations;

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    cgSolver.set_solver_parameters(CG_RELATIVE_TOLERANCE, MAX_CG_ITERATION);
    bool success = cgSolver.solve_precond(matrix, rhs, solution, tolerance, iterations);
    timer.tock();
    monitor.print_log_indent("Solve linear system: " + timer.get_run_time() +
                             ". Iterations: " + NumberUtils::format_with_commas(iterations) +
                             ", final tolerance: " + NumberUtils::format_to_scientific(tolerance));

    ////////////////////////////////////////////////////////////////////////////////
    Monitor::check_critical_condition(success, "Solution solve failed!");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::update_velocity()
{
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, particleParams->num_active_particles),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 p = r.begin(); p != r.end(); ++p)
                          {
                              velocity[p] += solution[p];
                          }
                      }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::clear_broken_bond_list()
{
    broken_bonds.resize(particleParams->num_active_particles);

    __NOODLE_CLEAR_VECVEC(broken_bonds);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PeridynamicsSolver::remove_broken_bonds()
{
    if(broken_bonds.size() == 0)
    {
        return false;
    }

    static Timer timer;
    timer.tick();
    volatile bool hasBrokenBond = false;

    ////////////////////////////////////////////////////////////////////////////////
    for(UInt32 p = 0; p < broken_bonds.size(); ++p)
    {
        if(broken_bonds[p].size() > 0)
        {
            for(auto it = broken_bonds[p].rbegin(); it != broken_bonds[p].rend(); ++it)
            {
                hasBrokenBond = true;
                const UInt32 bondIndex = (UInt32)(*it);

                tbb::parallel_invoke(
                    [&, bondIndex]
                {
                    bond_list[p].erase(bond_list[p].begin() + bondIndex);
                },
                    [&, bondIndex]
                {
                    bond_d0[p].erase(bond_d0[p].begin() + bondIndex);
                });
            }
        }
    }

    // if there is only 1/2 bonds remain, then just delete it
    for(UInt32 p = 0; p < (UInt32)bond_list.size(); ++p)
    {
        if(bond_list[p].size() <= 2)
        {
            for(UInt32 q : bond_list[p])
            {
                for(int i = (int)bond_list[q].size() - 1; i >= 0; --i)
                {
                    if(bond_list[q][i] == p)
                    {
                        bond_list[q].erase(bond_list[q].begin() + i);
                        bond_d0[q].erase(bond_d0[q].begin() + i);
                        break;
                    }
                }
            }

            bond_list[p].clear();
            bond_d0[p].clear();
        }
    }

    timer.tock();

    if(hasBrokenBond)
    {
        monitor.print_log("Remove broken bonds: " + timer.get_run_time());
    }

    return hasBrokenBond;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::compute_remaining_bond_ratio()
{
    remaining_bond_ratio.resize(particles.size());

    for(size_t p = 0; p < remaining_bond_ratio.size(); ++p)
    {
        remaining_bond_ratio[p] = (Real)bond_list[p].size() / (Real)bond_list_t0[p].size();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::find_connected_components()
{
    static Timer timer;
    timer.tick();

    particle_marker.assign(particle_marker.size(), -1);

    // label from first particle
    UInt32 num_processed_particles = 0;
    num_processed_particles += spawn_component(0, 0, 0);

    UInt32 total_component      = 1; // need?
    Int8   current_component_id = 0;

    bool new_label = false;
    bool labeled;

    while(num_processed_particles < particles.size())
    {
        labeled = false;

        for(UInt32 p = 0; p < particles.size(); ++p)
        {
            // find any particle that has not been label
            // and that particle has a labeled neighbor
            if(particle_marker[p] >= 0)
            {
                continue;
            }

            if(new_label)
            {
                num_processed_particles += spawn_component(p, 0, current_component_id);

                labeled   = true;
                new_label = false;
            }
            else
            {
                // find the component index from neighbor
                Int8 neighbor_component_id = -1;

                for(UInt32 q : bond_list[p])
                {
                    if(particle_marker[q] >= 0)
                    {
                        neighbor_component_id = particle_marker[q];
                        break;
                    }
                }

                // has a labeled neighbor?
                // get component id from neighbor
                if(neighbor_component_id >= 0)
                {
                    num_processed_particles += spawn_component(p, 0, neighbor_component_id);
                    labeled                  = true;
                }
            }
        }

        // not any particle has been labeled in the last loop
        // while num_process still < num particles
        // that means, there is more component
        if(!labeled)
        {
            ++current_component_id;
            ++total_component;

            new_label = true;
        }
    }

    __NOODLE_ASSERT(num_processed_particles == particles.size());

    particleParams->num_components = total_component;

    timer.tock();
    monitor.print_log("Number of connnected component: " +
                      std::to_string(total_component) +
                      ". Analysis time: " + timer.get_run_time());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
UInt32 PeridynamicsSolver::spawn_component(UInt32 p, int depth, Int8 component)
{
    particle_marker[p] = component;
    UInt32 num_processed_particles = 1;

    // max depth = 1000 to avoid stack overflow due to many time recursively call this func
    if(depth < 1000)
    {
        for(UInt32 q : bond_list[p])
        {
            if(particle_marker[q] >= 0)
            {
                continue;
            }

            num_processed_particles += spawn_component(q, depth + 1, component);
        }
    }

    return num_processed_particles;
}
