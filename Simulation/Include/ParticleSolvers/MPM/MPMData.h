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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct SimulationParametersMPM
{};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct SimulationDataMPM
{
    Vec_Vec3<RealType> positions;
    Vec_Vec3<RealType> velocity;
};





// Rather than re-construct the shape and gradient weights within
// each interpolation function we find them once per time step and store them.
struct contrib
{
    Vector3 G;
    Real    w;
    int     p, i;
    contrib(const int P, const int I, const Real W, const Vector3 GG)
    {
        p = P;
        i = I;
        w = W;
        G = GG;
    }
};

struct partSurface
{
    Vector3 norm;
    Real    area;
    int     p;
    partSurface(const Vector3& a, const Real& b, const int& c) : norm(a), area(b), p(c) {}
};

struct patch
{
    // const patch-wide values
    const Vector3 regionLo;          // minimum x and y of region
    const Vector3 regionHi;          // maximum x and y of region
    const Real    dx;                // x-dir cell size
    const Real    dy;                // y-dir cell size
    const Real    dz;                // z-dir cell size
    const int     I;                 // I columns of nodes
    const int     J;                 // J rows of nodes
    const int     K;                 // K layers of nodes
                                     // patch-wide variables
    Real                elapsedTime; // elapsed time within the simulation
    Real                finalTime;   // Expected final state of the simulation
    Real                timeStep;    // current time step (set in main)
    int                 incCount;    // Number of time steps taken
    vector<contrib>     con;         // particle-node shape and gradient contributions
    vector<partSurface> surfList;    // list of surfaces on particles
                                     // particle variables
    partArray<Matrix33> velGradP;    // velocity gradient
    partArray<Matrix33> defGradP;    // deformation gradient
    partArray<Matrix33> stressP;     // stress
    partArray<Vector3>  refPosP;     // initial position
    partArray<Vector3>  curPosP;     // position
    partArray<Vector3>  velP;        // velocity
    partArray<Vector3>  momP;        // momentum
    partArray<Vector3>  velIncP;     // velocity increment
    partArray<Vector3>  posIncP;     // position increment
    partArray<Vector3>  halfLenP;    // particle size
    partArray<Real>     massP;       // mass
    partArray<Real>     volumeP;     // volume
    partArray<Real>     refVolP;     // Initial volume
                                     // node variables
    nodeArray<Real>    massN;        // node mass
    nodeArray<Vector3> curPosN;      // node position
    nodeArray<Vector3> velN;         // node velocity
    nodeArray<Vector3> momN;         // node momentum
    nodeArray<Vector3> fintN;        // internal force on the node
    nodeArray<Vector3> gravityN;     // external acceleration
    nodeArray<Vector3> velIncN;      // grid velocity increment
                                     // inline methods
                                     // These functions find the cell within which a particle is contained
                                     // The lower left node of the cell is always defined as the "parent" node of that cell.
    Real rsx(const Real x) const
    {
        return (x - regionLo[0]) / dx + 1;
    }

    Real rsy(const Real y) const
    {
        return (y - regionLo[1]) / dy + 1;
    }

    Real rsz(const Real z) const
    {
        return (z - regionLo[2]) / dz + 1;
    }

    int rsi(const Real x) const
    {
        return int(floor(rsx(x)));
    }

    int rsj(const Real y) const
    {
        return int(floor(rsy(y)));
    }

    int rsk(const Real z) const
    {
        return int(floor(rsz(z)));
    }

    int inCell(const Vector3& p) const // find the cell into which x,y,z falls
    {
        const int i = rsi(p[0]);
        const int j = rsj(p[1]);
        const int k = rsk(p[2]);

        if(i > 0 && i < I - 1 && j > 0 && j < J - 1 && k > 0 && k < K - 1)
        {
            return k * I * J + j * I + i;
        }
        else
        {
            throw"inCell:outside findable region";
        }
    }

    bool inRegion(const Vector3& p) const
    {
        return (regionLo[0] <= p[0]
                && regionLo[1] <= p[1]
                && regionLo[2] <= p[2]
                && p[0] < regionHi[0]
                && p[1] < regionHi[1]
                && p[2] < regionHi[2]);
    }

    // This is a variation on finding the cell.  However, this is for
    // GIMP particles where portions of a particle may fall in up to
    // eight cells.  Hence, we return the lowest and left-est node to
    // which the particle may contribute.
    int inCell8(const Vector3& p) const
    {
        const Real& x = p[0];
        const Real& y = p[1];
        const Real& z = p[2];
        const int   i = rsi(x);
        const int   j = rsj(y);
        const int   k = rsk(z);

        if(!(i > 0 && i < I - 1 && j > 0 && j < J - 1 && k > 0 && k < K - 1))
        {
            throw"inCell8:outside findable region";
        }

        const Real enx = rsx(x) - Real(rsi(x));
        const Real eny = rsy(y) - Real(rsj(y));
        const Real enz = rsz(z) - Real(rsk(z));
        const int  io  = (enx < .5 ? i - 1 : i);
        const int  jo  = (eny < .5 ? j - 1 : j);
        const int  ko  = (enz < .5 ? k - 1 : k);
        return ko * I * J + jo * I + io;
    }

    // declared methods
    patch(const int Nx, const int Ny, const int Nz, const Vector3 lo, const Vector3 hi);
    virtual bool afterStep() = 0;
    virtual void gridVelocityBC(vector<Vector3>&) {};
    virtual void makeExternalForces(const Real&) {};
    virtual ~patch() {}
};








patch::patch(const int Nx, const int Ny, const int Nz, const Vector3 lo, const Vector3 hi) :
    regionLo(lo),
    regionHi(hi),
    dx((hi[0] - lo[0]) / Real(Nx)),
    dy((hi[1] - lo[1]) / Real(Ny)),
    dz((hi[2] - lo[2]) / Real(Nz)),
    I(Nx + 3), // must keep full boundary layer for GIMP
    J(Ny + 3),
    K(Nz + 3)
{
    globalNodeArrays.resizeAll(I * J * K);
    elapsedTime = 0.;   // default
    finalTime   = huge; // default (go forever)
    incCount    = 0;
    // laying out nodes in a grid
    for(int k = 0; k < K; ++k)
    {
        const Real z = (k - 1) * dz + regionLo[2];
        for(int j = 0; j < J; ++j)
        {
            const Real y = (j - 1) * dy + regionLo[1];
            for(int i = 0; i < I; ++i)
            {
                const Real x = (i - 1) * dx + regionLo[0];
                curPosN[k * I * J + j * I + i] = Vector3(x, y, z);
            }
        }
    }
    report.param("regionLo",   regionLo);
    report.param("regionHi",   regionHi);
    report.param("cell sizes", Vector3(dx, dy, dz));
    report.param("Icols",      I);
    report.param("Jrows",      J);
    report.param("Klayers",    K);
    report.param("Nnode",      Nnode());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana