//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
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

#include <exception>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct MPMMaterial
{
    const Real density, bulkModK, shearModG, yieldStress, isoHardMod, kinHardMod;

    MPMMaterial(const Real dens, const Real Y, const Real P, const Real c, const Real d, const Real e) :
        density(dens),
        bulkModK(Y / (3.0_f * (1.0_f - 2.0_f * P))),
        shearModG(Y / (2.0_f * (1.0_f + P))),
        yieldStress(c),
        isoHardMod(d),
        kinHardMod(e)
    {
        if(density < 0_f) { throw std::exception("invalid density"); }
        if(Y < 0_f) { throw std::exception("invalid Young's modulus"); }
        if(P > 0.5_f - std::numeric_limits<Real>::epsilon() || P < 0_f) { throw std::exception("invalid Poisson's ratio"); }
        if(yieldStress < 0_f) { throw std::exception("invalid yield stress - no plasticity found in plastic model"); }
        if(isoHardMod < 0_f) { throw std::exception("invalid isotropic hardening modulus"); }
        if(kinHardMod < 0_f) { throw std::exception("invalid kinematic hardening modulus"); }
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct ConstitutiveModel
{
    virtual void update(Real) = 0;
    virtual void revert()
    {
        throw std::exception("ConstitutiveSC: revert() and save() must be defined if using implicit methods");
    }

    virtual void save()
    {
        throw std::exception("ConstitutiveSC: revert() and save() must be defined if using implicit methods");
    }

    virtual Real waveSpeed() const = 0;
    virtual ~ConstitutiveModel() {}
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct NeoHookean : public ConstitutiveModel<Real>
{
    NeoHookean(const Real Y, const Real P, const Real D,
               const Vec_Mat3x3r& vg,
               Vec_Mat3x3r& dg,
               Vec_Real& vl,
               Vec_Mat3x3r& st) :
        Ymod(Y),
        Pois(P),
        Dens(D),
        lam(Ymod * Pois / ((1. + Pois) * (1. - 2. * Pois))),
        mu(.5 * Ymod / (1. + Pois)),
        velGrad(vg),
        defGrad(dg),
        volume(vl),
        stress(st)
    {
        if(Npart() < 1) { throw std::exception("NeoHookean: no particles to initialize!"); }
    }

    Real waveSpeed() const { return sqrt((lam + 3. * mu) / Dens); }
    void update(Real);
    void revert();
    void save();

    ////////////////////////////////////////////////////////////////////////////////
    Vec_Mat3x3r        defGrad0, stress0;
    Vec_Real           volume0;
    const Real         Ymod, Pois, Dens, lam, mu;
    const Vec_Mat3x3r& velGrad;
    Vec_Mat3x3r&       defGrad;
    Vec_Real&          volume;
    Vec_Mat3x3r&       stress;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct UpdatedElastic : public ConstitutiveModel<Real>
{
    UpdatedElastic(const Vec_Mat3x3r& vg,
                   Vec_Mat3x3r&       dg,
                   Vec_Real&          vl,
                   Vec_Mat3x3r&       st,
                   const material&    a) :
        twoThirds(2. / 3.),
        bulkModK(a.bulkModK),
        shearModG(a.shearModG),
        density(a.density),
        velGrad(vg),
        defGrad(dg),
        volume(vl),
        stress(st)
    {}
    Real waveSpeed() const { return sqrt((bulkModK + shearModG * 7. / 3.) / density); }
    void update(Real);
    void revert();
    void save();

    ////////////////////////////////////////////////////////////////////////////////
    Vec_Mat3x3r        stress0, defGrad0;
    Vec_Real           volume0;
    const Real         twoThirds, bulkModK, shearModG, density;
    const Vec_Mat3x3r& velGrad;
    Vec_Mat3x3r&       defGrad;
    Vec_Real&          volume;
    Vec_Mat3x3r&       stress;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct J2plasticLinearIsoKin : public ConstitutiveModel<Real>
{
    J2plasticLinearIsoKin(const material&    mt,
                          const Vec_Mat3x3r& vg,
                          Vec_Mat3x3r&       dg,
                          Vec_Real&          vl,
                          Vec_Mat3x3r&       st) :
        oneThird(1. / 3.),
        sqrtTwoThirds(sqrt(2. / 3.)),
        density(mt.density),
        bulkModK(mt.bulkModK),
        shearModG(mt.shearModG),
        yieldStress(mt.yieldStress),
        isoHardMod(mt.isoHardMod),
        kinHardMod(mt.kinHardMod),
        velGrad(vg),
        defGrad(dg),
        volume(vl),
        stress(st)
    {
        for(int i = 0; i < pnt.size(); ++i) {
            pnt[i].strain.fill(0);
            pnt[i].plasticStrain.fill(0);
            pnt[i].backStress.fill(0);
            pnt[i].internalAlpha = 0.;
        }
    }

    Real waveSpeed() const { return sqrt((bulkModK + shearModG * 7. / 3.) / density); }
    void update(Real);
    void revert();
    void save();

    ////////////////////////////////////////////////////////////////////////////////
    struct point
    {
        Mat3x3<Real> strain, plasticStrain, backStress;
        Real         internalAlpha;
    };
    Vec_Mat3x3r        stress0, defGrad0;
    Vec_Real           volume0;
    std::vector<point> pnt, pnt0;
    const Real         oneThird, sqrtTwoThirds, density, bulkModK, shearModG, yieldStress, isoHardMod, kinHardMod;
    const Vec_Mat3x3r& velGrad;
    Vec_Mat3x3r&       defGrad;
    Vec_Real&          volume;
    Vec_Mat3x3r&       stress;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleSolvers/MPM/MPMMaterial.Impl.hpp>
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
