


using RealType = float;
const RealType source_velocity          = 40.0;
const int      particle_correction_step = 1;

RealType fraction_inside(RealType phi_left, RealType phi_right);
void     extrapolate(Array2<RealType>& grid, Array2<RealType>& old_grid, const Array2<RealType>& grid_weight,
                     const Array2<RealType>& grid_liquid_weight, Array2c& valid, Array2c old_valid, const Vec2i& offset);

template<class RealType>
RealType Banana::FLIP2DSolver<RealType>::cfl()
{
    RealType maxvel = 0;
    for(int i = 0; i < u.a.size(); ++i)
        maxvel = fmax(maxvel, fabs(u.a[i]));
    for(int i = 0; i < v.a.size(); ++i)
        maxvel = fmax(maxvel, fabs(v.a[i]));
    return dx / maxvel;
}

template<class RealType>
void Banana::FLIP2DSolver<RealType>::initialize(const Vec2<RealType>& origin_, RealType width, int ni_, int nj_, RealType rho_,
                                                bool draw_grid_, bool draw_particles_, bool draw_velocities_, bool draw_boundaries_)
{
    rho             = rho_;
    draw_grid       = draw_grid_;
    draw_particles  = draw_particles_;
    draw_velocities = draw_velocities_;
    draw_boundaries = draw_boundaries_;
    origin          = origin_;
    ni              = ni_;
    nj              = nj_;
    dx              = width / (RealType)ni;
    u.resize(ni + 1, nj); temp_u.resize(ni + 1, nj); u_weights.resize(ni + 1, nj); u_valid.resize(ni + 1, nj); saved_u.resize(ni + 1, nj);
    v.resize(ni, nj + 1); temp_v.resize(ni, nj + 1); v_weights.resize(ni, nj + 1); v_valid.resize(ni, nj + 1); saved_v.resize(ni, nj + 1);
    u.set_zero();
    v.set_zero();
    nodal_solid_phi.resize(ni + 1, nj + 1);
    valid.resize(ni + 1, nj + 1);
    old_valid.resize(ni + 1, nj + 1);
    liquid_phi.resize(ni, nj);
    m_sorter = new sorter(ni, nj);
}

//Initialize the grid-based signed distance field that dictates the position of the solid boundary
template<class RealType>
void Banana::FLIP2DSolver<RealType>::update_boundary()
{
    for(int j = 0; j < nj + 1; ++j)
        for(int i = 0; i < ni + 1; ++i)
        {
            Vec2<RealType> pos(i* dx, j* dx);
            nodal_solid_phi(i, j) = compute_phi(pos + origin);
        }
}

template<class RealType>
void Banana::FLIP2DSolver<RealType>::resample(Vec2<RealType>& p, Vec2<RealType>& u, Mat2x2<RealType>& c)
{
    std::vector<Particle*> neighbors;
    RealType               wsum  = 0.0;
    Vec2<RealType>         save  = u;
    Mat2x2<RealType>       csave = c;
    u = Vec2<RealType>::Zero();
    c = Mat2x2<RealType>::Zero();

    int ix = std::max(0, std::min(ni - 1, (int)((p(0) - origin(0)) / dx)));
    int iy = std::max(0, std::min(nj - 1, (int)((p(1) - origin(1)) / dx)));

    m_sorter->getNeigboringParticles_cell(ix, iy, -1, 1, -1, 1, neighbors);

    const RealType re = dx;

    for(Particle* np : neighbors)
    {
        Vec2<RealType> diff = np->x - p;
        RealType       w    = 4.0 / 3.0 * M_PI * np->radii * np->radii * np->radii * rho * kernel::linear_kernel(diff, re);
        u    += w * np->v;
        c    += w * np->c;
        wsum += w;
    }

    if(wsum)
    {
        u /= wsum;
        c /= wsum;
    }
    else
    {
        u = save;
        c = csave;
    }
}

template<class RealType>
void Banana::FLIP2DSolver<RealType>::compute_density()
{
    int np = (int)particles.size();

    const RealType re = dx;
    // Compute Pseudo Moved Point
    for(int n = 0; n < np; ++n)
    {
        Particle& p    = particles[n];
        RealType  dens = 0.0;

        int ix = std::max(0, std::min((int)((p.x(0) - origin(0)) / dx), ni));
        int iy = std::max(0, std::min((int)((p.x(1) - origin(1)) / dx), nj));

        std::vector<Particle*> neighbors;
        m_sorter->getNeigboringParticles_cell(ix, iy, -1, 1, -1, 1, neighbors);

        for(Particle* np : neighbors)
        {
            RealType dist = (p.x - np->x).norm();
            RealType w    = kernel::poly6_kernel(dist * dist, re);
            dens += w;
        }

        p.dens = dens;
    }
}

template<class RealType>
void Banana::FLIP2DSolver<RealType>::correct(RealType dt)
{
    int np = (int)particles.size();

    const RealType re = dx / sqrt(2.0);

    int offset = rand() % particle_correction_step;
    // Compute Pseudo Moved Point
    for(int n = 0; n < np; ++n)
    {
        if(n % particle_correction_step != offset)
            continue;

        Particle& p = particles[n];

        if(p.type != PT_LIQUID)
            continue;
        Vec2<RealType> spring = Vec2<RealType>::Zero();

        int ix = std::max(0, std::min((int)((p.x(0) - origin(0)) / dx), ni));
        int iy = std::max(0, std::min((int)((p.x(1) - origin(1)) / dx), nj));

        std::vector<Particle*> neighbors;
        m_sorter->getNeigboringParticles_cell(ix, iy, -1, 1, -1, 1, neighbors);

        for(Particle* np : neighbors)
        {
            if(&p != np)
            {
                RealType dist = (p.x - np->x).norm();
                RealType w    = 50.0 * kernel::smooth_kernel(dist * dist, re);
                if(dist > 0.01 * re)
                {
                    spring += w * (p.x - np->x) / dist * re;
                }
                else
                {
                    spring(0) += 0.01 * re / dt * (rand() & 0xFF) / 255.0;
                    spring(1) += 0.01 * re / dt * (rand() & 0xFF) / 255.0;
                }
            }
        }

        p.buf0 = p.x + dt * spring;

        Vec2<RealType> pp        = (p.buf0 - origin) / dx;
        RealType       phi_value = interpolate_value(pp, nodal_solid_phi);

        if(phi_value < 0)
        {
            Vec2<RealType> normal;
            interpolate_gradient(normal, pp, nodal_solid_phi);
            normal.normalize();
            p.buf0 -= phi_value * normal;
        }
    }

    // Update
    for(int n = 0; n < np; ++n)
    {
        if(n % particle_correction_step != offset)
            continue;
        Particle& p = particles[n];
        if(p.type != PT_LIQUID)
            continue;

        p.x = p.buf0;
    }
}

//The main fluid simulation step
template<class RealType>
void Banana::FLIP2DSolver<RealType>::advance(RealType dt)
{
    RealType t = 0;

    while(t < dt)
    {
        float substep = cfl() * 3.0;
        if(t + substep > dt)
            substep = dt - t;

        //Passively advect particles
        advect_particles(dt);

//        m_sorter->sort(this);
        // TODO: find neighbor

        map_p2g();

        add_force(dt);
        project(dt);

        m_SimData->temp_u = m_SimData->u;
        m_SimData->temp_v = m_SimData->v;
        //Pressure projection only produces valid velocities in faces with non-zero associated face area.
        //Because the advection step may interpolate from these invalid faces,
        //we must extrapolate velocities from the fluid domain into these zero-area faces.
        extrapolate(m_SimData->u, m_SimData->temp_u, m_SimData->u_weights, m_SimData->liquid_phi, m_SimData->valid, m_SimData->old_valid, Vec2i(-1, 0));
        extrapolate(m_SimData->v, m_SimData->temp_v, m_SimData->v_weights, m_SimData->liquid_phi, m_SimData->valid, m_SimData->old_valid, Vec2i(0, -1));

        //For extrapolated velocities, replace the normal component with
        //that of the object.
        constrain_velocity();

        correct(dt);

        map_g2p_apic();

        t += substep;
    }
}

template<class RealType>
void Banana::FLIP2DSolver<RealType>::save_velocity()
{
    saved_u = u;
    saved_v = v;
}

template<class RealType>
void Banana::FLIP2DSolver<RealType>::add_force(RealType dt)
{
    // splat particles

    for(int j = 0; j < nj + 1; ++j)
        for(int i = 0; i < ni; ++i)
        {
            v(i, j) += -981.0 * dt;
            Vec2<RealType> pos = Vec2<RealType>((i + 0.5) * dx, j * dx) + origin;

            if(root_sources && compute_phi(pos, *root_sources) < dx)
            {
                v(i, j) = -source_velocity;
            }
        }
}

//For extrapolated points, replace the normal component
//of velocity with the object velocity (in this case zero).
template<class RealType>
void Banana::FLIP2DSolver<RealType>::constrain_velocity()
{
    temp_u = u;
    temp_v = v;

    //(At lower grid resolutions, the normal estimate from the signed
    //distance function is poor, so it doesn't work quite as well.
    //An exact normal would do better.)

    //constrain u
    for(int j = 0; j < u.nj; ++j)
        for(int i = 0; i < u.ni; ++i)
        {
            if(u_weights(i, j) == 0)
            {
                //apply constraint
                Vec2<RealType> pos = Vec2<RealType>(i * dx, (j + 0.5) * dx) + origin;
                Vec2<RealType> vel = get_velocity(pos);
                Vec2<RealType> normal(0, 0);
                interpolate_gradient(normal, Vec2<RealType>(i, j + 0.5), nodal_solid_phi);
                normal.normalize();
                RealType perp_component = vel.dot(normal);
                vel         -= perp_component * normal;
                temp_u(i, j) = vel[0];
            }
        }

    //constrain v
    for(int j = 0; j < v.nj; ++j)
        for(int i = 0; i < v.ni; ++i)
        {
            if(v_weights(i, j) == 0)
            {
                //apply constraint
                Vec2<RealType> pos = Vec2<RealType>((i + 0.5) * dx, j * dx) + origin;
                Vec2<RealType> vel = get_velocity(pos);
                Vec2<RealType> normal(0, 0);
                interpolate_gradient(normal, Vec2<RealType>(i + 0.5, j), nodal_solid_phi);
                normal.normalize();
                RealType perp_component = vel.dot(normal);
                vel         -= perp_component * normal;
                temp_v(i, j) = vel[1];
            }
        }

    //update
    u = temp_u;
    v = temp_v;
}

template<class RealType>
void Banana::FLIP2DSolver<RealType>::compute_phi()
{
    //Estimate from particles
    liquid_phi.assign(3 * dx);
    for(int p = 0; p < particles.size(); ++p)
    {
        if(particles[p].type != PT_LIQUID)
            continue;

        const Vec2<RealType>& point = particles[p].x;
        int                   i, j;
        RealType              fx, fy;
        //determine containing cell;
        get_barycentric((point[0]) / dx - 0.5, i, fx, 0, ni);
        get_barycentric((point[1]) / dx - 0.5, j, fy, 0, nj);

        //compute distance to surrounding few points, keep if it's the minimum
        for(int j_off = j - 2; j_off <= j + 2; ++j_off)
            for(int i_off = i - 2; i_off <= i + 2; ++i_off)
            {
                if(i_off < 0 || i_off >= ni || j_off < 0 || j_off >= nj)
                    continue;

                Vec2<RealType> pos      = Vec2<RealType>((i_off + 0.5f) * dx, (j_off + 0.5f) * dx) + origin;
                RealType       phi_temp = (pos - point).norm() - 1.00002 * particles[p].radii;
                liquid_phi(i_off, j_off) = min(liquid_phi(i_off, j_off), phi_temp);
            }
    }

    //"extrapolate" phi into solids if nearby
    for(int j = 0; j < nj; ++j)
    {
        for(int i = 0; i < ni; ++i)
        {
            if(liquid_phi(i, j) < 0.5 * dx)
            {
                float solid_phi_val = 0.25f * (nodal_solid_phi(i, j) + nodal_solid_phi(i + 1, j) + nodal_solid_phi(i, j + 1) + nodal_solid_phi(i + 1, j + 1));
                if(solid_phi_val < 0)
                    liquid_phi(i, j) = -0.5 * dx;
            }
        }
    }
}

//Add a tracer particle for visualization
template<class RealType>
void Banana::FLIP2DSolver<RealType>::add_particle(const SimulationData_FLIP2D<RealType>& p)
{
    particles.push_back(p);
}

//move the particles in the fluid
template<class RealType>
void Banana::FLIP2DSolver<RealType>::advect_particles(RealType dt)
{
    for(int p = 0; p < particles.size(); ++p)
    {
        if(particles[p].type == PT_SOLID)
            continue;

        particles[p].x += particles[p].v * dt;
        Vec2<RealType> pp = (particles[p].x - origin) / dx;

        //Particles can still occasionally leave the domain due to truncation errors,
        //interpolation error, or large timesteps, so we project them back in for good measure.

        //Try commenting this section out to see the degree of accumulated error.
        RealType phi_value = interpolate_value(pp, nodal_solid_phi);
        if(phi_value < 0)
        {
            Vec2<RealType> normal;
            interpolate_gradient(normal, pp, nodal_solid_phi);
            normal.normalize();
            particles[p].x -= phi_value * normal;
        }
    }

    m_sorter->sort(this);

    if(root_sources)
    {
        for(int j = 0; j < nj; ++j)
            for(int i = 0; i < ni; ++i)
            {
                int num_p_need = 2 - std::min(2, m_sorter->getNumParticleAt(i, j));
                for(int k = 0; k < num_p_need; ++k)
                {
                    RealType       x  = ((RealType)i + 0.5 + (((RealType)rand() / (RealType)RAND_MAX) * 0.5 - 0.5)) * dx;
                    RealType       y  = ((RealType)j + 0.5 + (((RealType)rand() / (RealType)RAND_MAX) * 0.5 - 0.5)) * dx;
                    Vec2<RealType> pt = Vec2<RealType>(x, y) + origin;

                    if(compute_phi(pt, *root_sources) < 0.0)
                    {
                        Vec2<RealType> pp        = (pt - origin) / dx;
                        RealType       phi_value = interpolate_value(pp, nodal_solid_phi);
                        if(phi_value > 0.0)
                        {
                            add_particle(Particle(pt, Vec2<RealType>(0.0, -source_velocity), dx / sqrt(2.0), PT_LIQUID));
                        }
                    }
                }
            }
    }

    particles.erase(std::remove_if(particles.begin(), particles.end(), [&](const Particle& p)
                                   {
                                       return p.x(0) < origin(0) - 0.5 * dx || p.x(0) > origin(0) + ((RealType)ni + 1.5) * dx || p.x(1) < origin(1) - 0.5 * dx || p.x(1) > origin(1) + ((RealType)nj + 1.5) * dx;
                                   }), particles.end());
}

template<class RealType>
void Banana::FLIP2DSolver<RealType>::project(RealType dt)
{
    compute_phi();

    //Compute finite-volume type face area weight for each velocity sample.
    compute_weights();

    //Set up and solve the variational pressure solve.
    solve_pressure(dt);
}

//Interpolate velocity from the MAC grid.
template<class RealType>
Vec2<RealType> Banana::FLIP2DSolver<RealType>::get_velocity(const Vec2<RealType>& position)
{
    //Interpolate the velocity from the u and v grids
    Vec2<RealType> p       = (position - origin) / dx;
    Vec2<RealType> p0      = p - Vec2<RealType>(0, 0.5);
    Vec2<RealType> p1      = p - Vec2<RealType>(0.5, 0);
    RealType       u_value = interpolate_value(p0, u);
    RealType       v_value = interpolate_value(p1, v);

    return Vec2<RealType>(u_value, v_value);
}

template<class RealType>
Mat2x2<RealType> get_affine_matrix(const Vec2<RealType>& position)
{
    Vec2<RealType> p  = (position - origin) / dx;
    Vec2<RealType> p0 = p - Vec2<RealType>(0, 0.5);
    Vec2<RealType> p1 = p - Vec2<RealType>(0.5, 0);

    Mat2x2<RealType> c;
    c.col(0) = affine_interpolate_value(p0, u) / dx;
    c.col(1) = affine_interpolate_value(p1, v) / dx;

    return c;
}

template<class RealType>
Mat2x2<RealType> Banana::FLIP2DSolver<RealType>::get_saved_affine_matrix(const Vec2<RealType>& position)
{
    Vec2<RealType> p  = (position - origin) / dx;
    Vec2<RealType> p0 = p - Vec2<RealType>(0, 0.5);
    Vec2<RealType> p1 = p - Vec2<RealType>(0.5, 0);

    Mat2x2<RealType> c;
    c.col(0) = affine_interpolate_value(p0, saved_u);
    c.col(1) = affine_interpolate_value(p1, saved_v);

    return c;
}

template<class RealType>
Vec2<RealType> Banana::FLIP2DSolver<RealType>::get_saved_velocity(const Vec2<RealType>& position)
{
    //Interpolate the velocity from the u and v grids
    Vec2<RealType> p       = (position - origin) / dx;
    Vec2<RealType> p0      = p - Vec2<RealType>(0, 0.5);
    Vec2<RealType> p1      = p - Vec2<RealType>(0.5, 0);
    RealType       u_value = interpolate_value(p0, saved_u);
    RealType       v_value = interpolate_value(p1, saved_v);

    return Vec2<RealType>(u_value, v_value);
}

//Given two signed distance values, determine what fraction of a connecting segment is "inside"
RealType fraction_inside(RealType phi_left, RealType phi_right)
{
    if(phi_left < 0 && phi_right < 0)
        return 1;
    if(phi_left < 0 && phi_right >= 0)
        return phi_left / (phi_left - phi_right);
    if(phi_left >= 0 && phi_right < 0)
        return phi_right / (phi_right - phi_left);
    else
        return 0;
}

//Compute finite-volume style face-weights for fluid from nodal signed distances
template<class RealType>
void Banana::FLIP2DSolver<RealType>::compute_weights()
{
    for(int j = 0; j < u_weights.nj; ++j)
        for(int i = 0; i < u_weights.ni; ++i)
        {
            u_weights(i, j) = 1 - fraction_inside(nodal_solid_phi(i, j + 1), nodal_solid_phi(i, j));
            u_weights(i, j) = clamp(u_weights(i, j), 0.0, 1.0);
        }
    for(int j = 0; j < v_weights.nj; ++j)
        for(int i = 0; i < v_weights.ni; ++i)
        {
            v_weights(i, j) = 1 - fraction_inside(nodal_solid_phi(i + 1, j), nodal_solid_phi(i, j));
            v_weights(i, j) = clamp(v_weights(i, j), 0.0, 1.0);
        }
}

//An implementation of the variational pressure projection solve for static geometry
template<class RealType>
void Banana::FLIP2DSolver<RealType>::solve_pressure(RealType dt)
{
    //This linear system could be simplified, but I've left it as is for clarity
    //and consistency with the standard naive discretization

    int ni          = v.ni;
    int nj          = u.nj;
    int system_size = ni * nj;
    if(rhs.size() != system_size)
    {
        rhs.resize(system_size);
        pressure.resize(system_size);
        matrix.resize(system_size);
    }
    matrix.zero();

    //Build the linear system for pressure
    for(int j = 1; j < nj - 1; ++j)
    {
        for(int i = 1; i < ni - 1; ++i)
        {
            int index = i + ni * j;
            rhs[index]      = 0;
            pressure[index] = 0;
            float centre_phi = liquid_phi(i, j);
            if(centre_phi < 0)
            {
                //right neighbour
                float term      = u_weights(i + 1, j) * dt / sqr(dx);
                float right_phi = liquid_phi(i + 1, j);
                if(right_phi < 0)
                {
                    matrix.add_to_element(index, index,     term);
                    matrix.add_to_element(index, index + 1, -term);
                }
                else
                {
                    float theta = fraction_inside(centre_phi, right_phi);
                    if(theta < 0.01)
                        theta = 0.01;
                    matrix.add_to_element(index, index, term / theta);
                }
                rhs[index] -= u_weights(i + 1, j) * u(i + 1, j) / dx;

                //left neighbour
                term = u_weights(i, j) * dt / sqr(dx);
                float left_phi = liquid_phi(i - 1, j);
                if(left_phi < 0)
                {
                    matrix.add_to_element(index, index,     term);
                    matrix.add_to_element(index, index - 1, -term);
                }
                else
                {
                    float theta = fraction_inside(centre_phi, left_phi);
                    if(theta < 0.01)
                        theta = 0.01;
                    matrix.add_to_element(index, index, term / theta);
                }
                rhs[index] += u_weights(i, j) * u(i, j) / dx;

                //top neighbour
                term = v_weights(i, j + 1) * dt / sqr(dx);
                float top_phi = liquid_phi(i, j + 1);
                if(top_phi < 0)
                {
                    matrix.add_to_element(index, index,      term);
                    matrix.add_to_element(index, index + ni, -term);
                }
                else
                {
                    float theta = fraction_inside(centre_phi, top_phi);
                    if(theta < 0.01)
                        theta = 0.01;
                    matrix.add_to_element(index, index, term / theta);
                }
                rhs[index] -= v_weights(i, j + 1) * v(i, j + 1) / dx;

                //bottom neighbour
                term = v_weights(i, j) * dt / sqr(dx);
                float bot_phi = liquid_phi(i, j - 1);
                if(bot_phi < 0)
                {
                    matrix.add_to_element(index, index,      term);
                    matrix.add_to_element(index, index - ni, -term);
                }
                else
                {
                    float theta = fraction_inside(centre_phi, bot_phi);
                    if(theta < 0.01)
                        theta = 0.01;
                    matrix.add_to_element(index, index, term / theta);
                }
                rhs[index] += v_weights(i, j) * v(i, j) / dx;
            }
        }
    }

    //Solve the system using Robert Bridson's incomplete Cholesky PCG solver

    RealType tolerance;
    int      iterations;
    bool     success = solver.solve(matrix, rhs, pressure, tolerance, iterations);
    if(!success)
    {
        printf("WARNING: Pressure solve failed!************************************************\n");
    }

    //Apply the velocity update
    u_valid.assign(0);
    for(int j = 0; j < u.nj; ++j)
        for(int i = 1; i < u.ni - 1; ++i)
        {
            int index = i + j * ni;
            if(u_weights(i, j) > 0)
            {
                if(liquid_phi(i, j) < 0 || liquid_phi(i - 1, j) < 0)
                {
                    float theta = 1;
                    if(liquid_phi(i, j) >= 0 || liquid_phi(i - 1, j) >= 0)
                        theta = fraction_inside(liquid_phi(i - 1, j), liquid_phi(i, j));
                    if(theta < 0.01)
                        theta = 0.01;
                    u(i, j)      -= dt * (pressure[index] - pressure[index - 1]) / dx / theta;
                    u_valid(i, j) = 1;
                }
            }
            else
                u(i, j) = 0;
        }
    v_valid.assign(0);
    for(int j = 1; j < v.nj - 1; ++j)
        for(int i = 0; i < v.ni; ++i)
        {
            int index = i + j * ni;
            if(v_weights(i, j) > 0)
            {
                if(liquid_phi(i, j) < 0 || liquid_phi(i, j - 1) < 0)
                {
                    float theta = 1;
                    if(liquid_phi(i, j) >= 0 || liquid_phi(i, j - 1) >= 0)
                        theta = fraction_inside(liquid_phi(i, j - 1), liquid_phi(i, j));
                    if(theta < 0.01)
                        theta = 0.01;
                    v(i, j)      -= dt * (pressure[index] - pressure[index - ni]) / dx / theta;
                    v_valid(i, j) = 1;
                }
            }
            else
                v(i, j) = 0;
        }
}

template<class RealType>
RealType Banana::FLIP2DSolver<RealType>::compute_phi(const Vec2<RealType>& pos, const Boundary<RealType>& b) const
{
    switch(b.type)
    {
        case BT_BOX:
            return b.sign * box_phi(pos, b.center, b.parameter);
        case BT_CIRCLE:
            return b.sign * circle_phi(pos, b.center, b.parameter(0));
        case BT_TORUS:
            return b.sign * torus_phi(pos, b.center, b.parameter(0), b.parameter(1));
        case BT_TRIANGLE:
            return b.sign * triangle_phi(pos, b.center, b.parameter(0));
        case BT_HEXAGON:
            return b.sign * hexagon_phi(pos, b.center, b.parameter(0));
        case BT_CYLINDER:
            return b.sign * cylinder_phi(pos, b.center, b.parameter(0), b.parameter(1));
        case BT_UNION:
            return union_phi(compute_phi(pos, *b.op0), compute_phi(pos, *b.op1));
        case BT_INTERSECTION:
            return intersection_phi(compute_phi(pos, *b.op0), compute_phi(pos, *b.op1));
        default:
            return 1e+20;
    }
}

template<class RealType>
RealType Banana::FLIP2DSolver<RealType>::compute_phi(const Vec2<RealType>& pos) const
{
    return compute_phi(pos, *root_boundary);
}

template<class RealType>
void Banana::FLIP2DSolver<RealType>::init_random_particles()
{
    int num_particle = ni * nj;
    for(int i = 0; i < ni; ++i)
    {
        for(int j = 0; j < nj; ++j)
        {
            for(int k = 0; k < 2; ++k)
            {
                RealType       x  = ((RealType)i + 0.5 + (((RealType)rand() / (RealType)RAND_MAX) * 0.5 - 0.5)) * dx;
                RealType       y  = ((RealType)j + 0.5 + (((RealType)rand() / (RealType)RAND_MAX) * 0.5 - 0.5)) * dx;
                Vec2<RealType> pt = Vec2<RealType>(x, y) + origin;

                RealType phi = compute_phi(pt);
                if(phi > dx * 20.0)
                    add_particle(Particle(pt, Vec2<RealType>::Zero(), dx / sqrt(2.0), PT_LIQUID));
            }
        }
    }
}

template<class RealType>
void Banana::FLIP2DSolver<RealType>::map_p2g()
{
    //u-component of velocity
    for(int j = 0; j < m_SimData->nj; ++j)
        for(int i = 0; i < m_SimData->ni + 1; ++i)
        {
            Vec2<RealType>         pos = Vec2<RealType>(i * dx, (j + 0.5) * dx) + origin;
            std::vector<Particle*> neighbors;
            m_sorter->getNeigboringParticles_cell(i, j, -1, 0, -1, 1, neighbors);

            RealType sumw = 0.0;
            RealType sumu = 0.0;
            for(Particle* p : neighbors)
            {
                RealType w = 4.0 / 3.0 * M_PI * rho * p->radii * p->radii * p->radii * kernel::linear_kernel(p->x - pos, dx);
                sumu += w * (p->v(0) + p->c.col(0).dot(pos - p->x));
                sumw += w;
            }

            u(i, j) = sumw ? sumu / sumw : 0.0;
        }

    //v-component of velocity
    for(int j = 0; j < nj + 1; ++j)
        for(int i = 0; i < ni; ++i)
        {
            Vec2<RealType>         pos = Vec2<RealType>((i + 0.5) * dx, j * dx) + origin;
            std::vector<Particle*> neighbors;
            m_sorter->getNeigboringParticles_cell(i, j, -1, 1, -1, 0, neighbors);

            RealType sumw = 0.0;
            RealType sumu = 0.0;
            for(Particle* p : neighbors)
            {
                RealType w = 4.0 / 3.0 * M_PI * rho * p->radii * p->radii * p->radii * kernel::linear_kernel(p->x - pos, dx);
                sumu += w * (p->v(1) + p->c.col(1).dot(pos - p->x));
                sumw += w;
            }

            v(i, j) = sumw ? sumu / sumw : 0.0;
        }
}

template<class RealType>
void Banana::FLIP2DSolver<RealType>::map_g2p_pic()
{
    for(Particle& p : particles)
    {
        if(p.type == PT_SOLID)
            continue;

        p.v = get_velocity(p.x);
    }
}

template<class RealType>
void Banana::FLIP2DSolver<RealType>::map_g2p_apic()
{
    for(Particle& p : particles)
    {
        if(p.type == PT_SOLID)
            continue;

        p.v = get_velocity(p.x);
        p.c = get_affine_matrix(p.x);
    }
}

template<class RealType>
void Banana::FLIP2DSolver<RealType>::map_g2p_flip(const RealType coeff)
{
    for(Particle& p : particles)
    {
        if(p.type == PT_SOLID)
            continue;

        p.v = get_velocity(p.x) + coeff * (p.v - get_saved_velocity(p.x));
    }
}

template<class RealType>
void Banana::FLIP2DSolver<RealType>::map_g2p_aflip(const RealType coeff)
{
    for(Particle& p : particles)
    {
        if(p.type == PT_SOLID)
            continue;

        p.v = get_velocity(p.x) + coeff * (p.v - get_saved_velocity(p.x));
        p.c = get_affine_matrix(p.x);
    }
}

template<class RealType>
void Banana::FLIP2DSolver<RealType>::render_boundaries(const Boundary<RealType>& b)
{
    switch(b.type)
    {
        case BT_CIRCLE:
            draw_circle2d(b.center, b.parameter(0), 64);
            break;
        case BT_BOX:
            draw_box2d(b.center, b.parameter(0), b.parameter(1));
            break;
        case BT_TORUS:
            draw_circle2d(b.center, b.parameter(0), 64);
            draw_circle2d(b.center, b.parameter(1), 64);
            break;
        case BT_HEXAGON:
            draw_circle2d(b.center, b.parameter(0), 6);
            break;
        case BT_TRIANGLE:
            draw_circle2d(b.center, b.parameter(0), 3);
            break;
        case BT_UNION: case BT_INTERSECTION:
            render_boundaries(*b.op0);
            render_boundaries(*b.op1);
            break;
        default:
            break;
    }
}

template<class RealType>
void Banana::FLIP2DSolver<RealType>::render()
{
    glPushMatrix();
    glScaled(1.0 / (dx * ni), 1.0 / (dx * ni), 1.0 / (dx * ni));

    if(draw_grid)
    {
        glColor3f(0.75, 0.75, 0.75);
        glLineWidth(1);
        draw_grid2d(origin, dx, ni, nj);
    }

    if(draw_boundaries)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        render_boundaries(*root_boundary);
    }

    if(draw_particles)
    {
        glColor3d(0, 0, 1);
        glPointSize(5);
        glBegin(GL_POINTS);
        for(unsigned int i = 0; i < particles.size(); ++i)
        {
            if(particles[i].type == PT_LIQUID)
                glVertex2dv(particles[i].x.data());
        }
        glEnd();

        glColor3d(1, 0, 0);
        glBegin(GL_POINTS);
        for(unsigned int i = 0; i < particles.size(); ++i)
        {
            if(particles[i].type == PT_SOLID)
                glVertex2dv(particles[i].x.data());
        }
        glEnd();
    }
    if(draw_velocities)
    {
        glColor3d(1, 0, 0);
        for(int j = 0; j < nj; ++j)
            for(int i = 0; i < ni; ++i)
            {
                Vec2<RealType> pos = Vec2<RealType>((i + 0.5) * dx, (j + 0.5) * dx) + origin;
                draw_arrow2d(pos, pos + 0.01 * get_velocity(pos), 0.1 * dx);
            }
    }

    glPopMatrix();
}

//Apply several iterations of a very simple "Jacobi"-style propagation of valid velocity data in all directions
void extrapolate(Array2<RealType>& grid, Array2<RealType>& old_grid, const Array2<RealType>& grid_weight, const Array2<RealType>& grid_liquid_weight, Array2c& valid, Array2c old_valid, const Vec2ui& offset)
{
    //Initialize the list of valid cells
    for(size_t j = 0; j < valid.sizeY(); ++j)
        valid(size_t(0), j) = valid(valid.sizeX() - 1, j) = 0;
    for(size_t i = 0; i < valid.sizeX(); ++i)
        valid(i, size_t(0)) = valid(i, valid.sizeY() - 1) = 0;

    Array2<RealType>* pgrids[]  = { &grid, &old_grid };
    Array2c*          pvalids[] = { &valid, &old_valid };

    for(size_t j = 1; j < grid.sizeY() - 1; ++j)
        for(size_t i = 1; i < grid.sizeX() - 1; ++i)
            valid(i, j) = grid_weight(i, j) > 0 && (grid_liquid_weight(i, j) < 0 || grid_liquid_weight(i + offset[0], j + offset[1]) < 0);

    old_valid = valid;

    for(int layers = 0; layers < 4; ++layers)
    {
        Array2<RealType>* pgrid_source = pgrids[layers & 1];
        Array2<RealType>* pgrid_target = pgrids[!(layers & 1)];

        Array2c* pvalid_source = pvalids[layers & 1];
        Array2c* pvalid_target = pvalids[!(layers & 1)];

        for(size_t j = 1; j < grid.sizeY() - 1; ++j)
            for(size_t i = 1; i < grid.sizeX() - 1; ++i)
            {
                RealType sum   = 0;
                int      count = 0;

                if(!(*pvalid_source)(i, j))
                {
                    if((*pvalid_source)(i + 1, j))
                    {
                        sum += (*pgrid_source)(i + 1, j);
                        ++count;
                    }
                    if((*pvalid_source)(i - 1, j))
                    {
                        sum += (*pgrid_source)(i - 1, j);
                        ++count;
                    }
                    if((*pvalid_source)(i, j + 1))
                    {
                        sum += (*pgrid_source)(i, j + 1);
                        ++count;
                    }
                    if((*pvalid_source)(i, j - 1))
                    {
                        sum += (*pgrid_source)(i, j - 1);
                        ++count;
                    }

                    //If any of neighbour cells were valid,
                    //assign the cell their average value and tag it as valid
                    if(count > 0)
                    {
                        (*pgrid_target)(i, j)  = sum / (RealType)count;
                        (*pvalid_target)(i, j) = 1;
                    }
                }
            }

        *pgrid_source  = *pgrid_target;
        *pvalid_source = *pvalid_target;
    }
}
