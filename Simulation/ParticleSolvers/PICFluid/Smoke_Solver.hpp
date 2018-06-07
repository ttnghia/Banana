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
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Smoke_Solver<N, RealType>::project()
{
    Array divergence(res, 0.0_f);
    for(auto& ind : u.get_region()) {
        if(0 < ind.i) {
            divergence[ind + Vector3i(-1, 0, 0)] += u[ind];
        }
        if(ind.i < res[0]) {
            divergence[ind] -= u[ind];
        }
    }
    for(auto& ind : v.get_region()) {
        if(0 < ind.j) {
            divergence[ind + Vector3i(0, -1, 0)] += v[ind];
        }
        if(ind.j < res[1]) {
            divergence[ind] -= v[ind];
        }
    }
    for(auto& ind : w.get_region()) {
        if(0 < ind.k) {
            divergence[ind + Vector3i(0, 0, -1)] += w[ind];
        }
        if(ind.k < res[2]) {
            divergence[ind] -= w[ind];
        }
    }
    pressure = 0;
    pressure_solver->set_boundary_condition(boundary_condition);
    for(auto& ind : boundary_condition.get_region()) {
        if(boundary_condition[ind] != PoissonSolver3D::INTERIOR) {
            divergence[ind] = 0.0_f;
        }
    }
    pressure_solver->run(divergence, pressure, pressure_tolerance);
    auto is_neumann = [&](Index3D const& ind) -> bool
                      {
                          if(boundary_condition.inside(ind)) {
                              return boundary_condition[ind] == PoissonSolver3D::NEUMANN;
                          } else {
                              if(open_boundary) {
                                  return false;
                              } else {
                                  return true;
                              }
                          }
                      };
    for(auto& ind : pressure.get_region()) {
        if(!is_neumann(ind.neighbour(Vector3i(-1, 0, 0)))) {
            u[ind] += pressure[ind];
        }
        if(!is_neumann(ind.neighbour(Vector3i(1, 0, 0)))) {
            u[ind + Vector3i(1, 0, 0)] -= pressure[ind];
        }

        if(!is_neumann(ind.neighbour(Vector3i(0, -1, 0)))) {
            v[ind] += pressure[ind];
        }
        if(!is_neumann(ind.neighbour(Vector3i(0, 1, 0)))) {
            v[ind + Vector3i(0, 1, 0)] -= pressure[ind];
        }

        if(!is_neumann(ind.neighbour(Vector3i(0, 0, -1)))) {
            w[ind] += pressure[ind];
        }
        if(!is_neumann(ind.neighbour(Vector3i(0, 0, 1)))) {
            w[ind + Vector3i(0, 0, 1)] -= pressure[ind];
        }
    }
    last_pressure = pressure;
}

template<Int N, class RealType>
void Smoke_Solver<N, RealType>::initialize(const Config& config)
{
    Simulation3D::initialize(config);
    res                = config.get<Vector3i>("resolution");
    smoke_alpha        = config.get("smoke_alpha", 0.0_f);
    smoke_beta         = config.get("smoke_beta", 0.0_f);
    temperature_decay  = config.get("temperature_decay", 0.0_f);
    pressure_tolerance = config.get("pressure_tolerance", 0.0_f);
    density_scaling    = config.get("density_scaling", 1.0_f);
    tracker_generation = config.get("tracker_generation", 100.0_f);
    num_threads        = config.get<int>("num_threads");
    super_sampling     = config.get<int>("super_sampling");
    std::string padding;
    open_boundary = config.get<bool>("open_boundary");
    if(open_boundary) {
        padding = "dirichlet";
    } else {
        padding = "neumann";
    }

    perturbation = config.get("perturbation", 0.0_f);
    Config solver_config;
    solver_config.set("res", res)
        .set("num_threads", num_threads)
        .set("padding",     padding)
        .set("maximum_iterations",
             config.get<int>("maximum_pressure_iterations"));
    pressure_solver = create_instance<PoissonSolver3D>(
        config.get<std::string>("pressure_solver"), solver_config);
    u                  = Array(res + Vector3i(1, 0, 0), 0.0_f, VecN(0.0_f, 0.5f, 0.5f));
    v                  = Array(res + Vector3i(0, 1, 0), 0.0_f, VecN(0.5f, 0.0_f, 0.5f));
    w                  = Array(res + Vector3i(0, 0, 1), 0.0_f, VecN(0.5f, 0.5f, 0.0_f));
    rho                = Array(res, 0.0_f);
    pressure           = Array(res, 0.0_f);
    last_pressure      = Array(res, 0.0_f);
    t                  = Array(res, config.get("initial_t", 0.0_f));
    current_t          = 0.0_f;
    boundary_condition = PoissonSolver3D::BCArray(res);
    for(auto& ind : boundary_condition.get_region()) {
        VecN d = ind.get_pos() - res.cast<RealType>() * 0.5_f;
        if(length(d) * 4 < res[0] || ind.i == 0 || ind.i == res[0] - 1 ||
           ind.j == 0 || ind.k == 0 || ind.k == res[2] - 1) {
            // boundary_condition[ind] = PoissonSolver3D::NEUMANN;
        }
    }
}

VecN hsv2rgb(VecN hsv)
{
    RealType h = hsv.x;
    RealType s = hsv.y;
    RealType v = hsv.z;
    int      j = (int)floor(h * 6);
    RealType f = h * 6 - j;
    RealType p = v * (1 - s);
    RealType q = v * (1 - f * s);
    RealType t = v * (1 - (1 - f) * s);
    RealType r, g, b;
    switch(j % 6) {
        case 0:
            r = v, g = t, b = p;
            break;
        case 1:
            r = q, g = v, b = p;
            break;
        case 2:
            r = p, g = v, b = t;
            break;
        case 3:
            r = p, g = q, b = v;
            break;
        case 4:
            r = t, g = p, b = v;
            break;
        default: // 5, actually
            r = v, g = p, b = q;
            break;
    }
    return VecN(r, g, b);
}

std::vector<RenderParticle> Smoke3D::get_render_particles() const
{
    using Particle = RenderParticle;
    std::vector<Particle> render_particles;
    render_particles.reserve(trackers.size());
    VecN center(res[0] / 2.0f, res[1] / 2.0f, res[2] / 2.0f);
    for(auto p : trackers) {
        render_particles.push_back(Particle(
                                       p.position - center, Vector4(p.color.x, p.color.y, p.color.z, 1.0_f)));
    }
    return render_particles;
}

template<Int N, class RealType>
void Smoke_Solver<N, RealType>::show(Array2D<VecN>& buffer)
{
    buffer.reset(VecN(0));
    int half_width  = buffer.get_width() / 2,
        half_height = buffer.get_height() / 2;
    for(int i = 0; i < half_width; i++) {
        for(int j = 0; j < buffer.get_height(); j++) {
            RealType rho_sum = 0.0_f;
            RealType t_sum   = 0.0_f;
            for(int k = 0; k < res[2]; k++) {
                RealType x = (i + 0.5f) / (RealType)half_width * res[0];
                RealType y = (j + 0.5f) / (RealType)buffer.get_height() * res[1];
                RealType z = k + 0.5f;
                rho_sum += rho.sample(x, y, z);
                t_sum   += t.sample(x, y, z);
            }
            rho_sum     *= density_scaling;
            t_sum        = std::min(1.0_f, t_sum / res[2]);
            rho_sum      = std::min(1.0_f, rho_sum / res[2]);
            buffer[i][j] = VecN(t_sum, rho_sum * 0.3f, rho_sum * 0.8f);
        }
    }
    for(int i = 0; i < half_width; i++) {
        for(int j = 0; j < half_height; j++) {
            RealType rho_sum = 0.0_f;
            RealType t_sum   = 0.0_f;
            for(int k = 0; k < res[2]; k++) {
                RealType x = (i + 0.5f) / (RealType)half_width * res[0];
                RealType y = k + 0.5f;
                RealType z = (j + 0.5f) / (RealType)half_height * res[2];
                rho_sum += rho.sample(x, y, z);
                t_sum   += t.sample(x, y, z);
            }
            rho_sum                  *= density_scaling;
            t_sum                     = std::min(1.0_f, t_sum / res[2]);
            rho_sum                   = std::min(1.0_f, rho_sum / res[2]);
            buffer[half_width + i][j] =
                VecN(t_sum, rho_sum * 0.3f, rho_sum * 0.8f);
        }
    }
}

template<Int N, class RealType>
void Smoke_Solver<N, RealType>::move_trackers(RealType delta_t)
{
    for(auto& tracker : trackers) {
        auto velocity = sample_velocity(tracker.position);
        tracker.position +=
            sample_velocity(tracker.position + 0.5f * delta_t * velocity) * delta_t;
    }
}

template<Int N, class RealType>
void Smoke_Solver<N, RealType>::step(RealType delta_t)
{
    {
        Time::Timer _("Seeding");
        for(auto& ind : rho.get_region()) {
            for(int k = 0; k < super_sampling; k++) {
                VecN pos = ind.get_pos() + VecN(rand(), rand(), rand()) -
                           ind.storage_offset;
                VecN relative_pos =
                    pos / Vector3i(rho.get_width(), rho.get_height(), rho.get_depth()).template cast<RealType>();
                RealType seed = generation_tex->sample(relative_pos).x / super_sampling;
                if(seed == 0) {
                    continue;
                }
                VecN initial_speed = initial_velocity_tex->sample3(relative_pos);
                VecN color         = color_tex->sample3(relative_pos);
                t[ind]    = temperature_tex->sample3(relative_pos).x;
                rho[ind] += seed;

                u[ind] = initial_speed.x;
                v[ind] = initial_speed.y;
                w[ind] = initial_speed.z;

                RealType gen     = delta_t * seed;
                int      gen_int =
                    (int)std::floor(gen) + int(rand() < gen - std::floor(gen));

                for(int i = 0; i < gen_int; i++) {
                    trackers.push_back(Tracker3D(pos, color));
                }
            }
        }
        for(auto& ind : v.get_region()) {
            if(ind.j < res[1]) {
                v[ind] += (-smoke_alpha * rho[ind] + smoke_beta * t[ind]) * delta_t;
            }
        }
        RealType t_decay = std::exp(-delta_t * temperature_decay);
        for(auto& ind : t.get_region()) {
            t[ind] *= t_decay;
        }
    }
    apply_boundary_condition();
    TIME(project());
    apply_boundary_condition();
    TIME(move_trackers(delta_t));
    TIME(remove_outside_trackers());
    TIME(advect(delta_t));
    apply_boundary_condition();
    current_t += delta_t;
}

template<Int N, class RealType>
void Smoke_Solver<N, RealType>::remove_outside_trackers()
{
    std::vector<Tracker3D> all_trackers = trackers;
    trackers.clear();
    for(auto& tracker : all_trackers) {
        VecN p = tracker.position;
        if(0 <= p.x && p.x <= res[0] && 0 <= p.y && p.y <= res[1] && 0 <= p.z &&
           p.z <= res[2]) {
            trackers.push_back(tracker);
        }
    }
}

template<Int N, class RealType>
VecN Smoke_Solver<N, RealType>::::sample_velocity(const Array& u,
                                                  const Array& v,
                                                  const Array& w,
                                                  const VecN&  pos)
{
    return VecN(u.sample(pos), v.sample(pos), w.sample(pos));
}

template<Int N, class RealType>
VecN Smoke_Solver<N, RealType>::::sample_velocity(const VecN& pos) const
{
    return sample_velocity(u, v, w, pos);
}

template<Int N, class RealType>
void Smoke_Solver<N, RealType>::advect(Array& attr, RealType delta_t)
{
    auto new_attr = attr.same_shape(0);
    for(auto& ind : new_attr.get_region()) {
        auto old_position =
            ind.get_pos() - delta_t * sample_velocity(ind.get_pos());
        new_attr[ind] = attr.sample(old_position);
    }
    attr = new_attr;
}

template<Int N, class RealType>
void Smoke_Solver<N, RealType>::apply_boundary_condition()
{
    for(auto& ind : boundary_condition.get_region()) {
        if(boundary_condition[ind] == PoissonSolver3D::NEUMANN) {
            u[ind]                     = 0;
            u[ind + Vector3i(1, 0, 0)] = 0;
            v[ind]                     = 0;
            v[ind + Vector3i(0, 1, 0)] = 0;
            w[ind]                     = 0;
            w[ind + Vector3i(0, 0, 1)] = 0;
        }
    }
    if(!open_boundary) {
        for(int i = 0; i < res[0]; i++) {
            for(int j = 0; j < res[1]; j++) {
                w[i][j][0] = w[i][j][res[2] - 1] = 0;
            }
        }
        for(int i = 0; i < res[0]; i++) {
            for(int k = 0; k < res[2]; k++) {
                v[i][0][k] = v[i][res[1] - 1][k] = 0;
            }
        }
        for(int j = 0; j < res[1]; j++) {
            for(int k = 0; k < res[2]; k++) {
                u[0][j][k] = u[res[0] - 1][j][k] = 0;
            }
        }
    }
}

template<Int N, class RealType>
void Smoke_Solver<N, RealType>::advect(RealType delta_t)
{
    advect(rho, delta_t);
    advect(t,   delta_t);
    advect(u,   delta_t);
    advect(v,   delta_t);
    advect(w,   delta_t);
}

template<Int N, class RealType>
void Smoke_Solver<N, RealType>::confine_vorticity(RealType delta_t) {}

template<Int N, class RealType>
void Smoke_Solver<N, RealType>::update(const Config& config)
{
    generation_tex =
        AssetManager::get_asset<Texture>(config.get<int>("generation_tex"));
    initial_velocity_tex =
        AssetManager::get_asset<Texture>(config.get<int>("initial_velocity_tex"));
    color_tex       = AssetManager::get_asset<Texture>(config.get<int>("color_tex"));
    temperature_tex =
        AssetManager::get_asset<Texture>(config.get<int>("temperature_tex"));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
