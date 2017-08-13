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

#include <exception>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct MPMMaterial
{
    const RealType density, bulkModK, shearModG, yieldStress, isoHardMod, kinHardMod;

    MPMMaterial(const RealType dens, const RealType Y, const RealType P, const RealType c, const RealType d, const RealType e) :
        density(dens),
        bulkModK(Y / (RealType(3.0) * (RealType(1.0) - RealType(2.0) * P))),
        shearModG(Y / (RealType(2.0) * (RealType(1.0) + P))),
        yieldStress(c),
        isoHardMod(d),
        kinHardMod(e)
    {
        if(density < RealType(0.0)) throw std::exception("invalid density");
        if(Y < RealType(0.0)) throw std::exception("invalid Young's modulus");
        if(P > RealType(0.5) - std::numeric_limits<RealType>::epsilon() || P < RealType(0.0)) throw std::exception("invalid Poisson's ratio");
        if(yieldStress < RealType(0.0)) throw std::exception("invalid yield stress - no plasticity found in plastic model");
        if(isoHardMod < RealType(0.0)) throw std::exception("invalid isotropic hardening modulus");
        if(kinHardMod < RealType(0.0)) throw std::exception("invalid kinematic hardening modulus");
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct ConstitutiveModel
{
    virtual void update(RealType) = 0;
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
template<class RealType>
struct NeoHookean : public ConstitutiveModel<RealType>
{
    NeoHookean(const RealType Y, const RealType P, const RealType D,
               const std::vector<Mat3x3<RealType> >& vg,
               std::vector<Mat3x3<RealType> >& dg,
               std::vector<RealType>& vl,
               std::vector<Mat3x3<RealType> >& st) :
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
        if(Npart() < 1) throw std::exception("NeoHookean: no particles to initialize!");
    }

    RealType waveSpeed() const { return sqrt((lam + 3. * mu) / Dens); }
    void update(RealType);
    void revert();
    void save();

    ////////////////////////////////////////////////////////////////////////////////
    std::vector<Mat3x3<RealType> >        defGrad0, stress0;
    std::vector<RealType>                 volume0;
    const RealType                        Ymod, Pois, Dens, lam, mu;
    const std::vector<Mat3x3<RealType> >& velGrad;
    std::vector<Mat3x3<RealType> >&       defGrad;
    std::vector<RealType>&                volume;
    std::vector<Mat3x3<RealType> >&       stress;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct UpdatedElastic : public ConstitutiveModel<RealType>
{
    UpdatedElastic(const std::vector<Mat3x3<RealType> >& vg,
                   std::vector<Mat3x3<RealType> >&       dg,
                   std::vector<RealType>&                vl,
                   std::vector<Mat3x3<RealType> >&       st,
                   const material&                       a) :
        twoThirds(2. / 3.),
        bulkModK(a.bulkModK),
        shearModG(a.shearModG),
        density(a.density),
        velGrad(vg),
        defGrad(dg),
        volume(vl),
        stress(st)
    {}
    RealType waveSpeed() const { return sqrt((bulkModK + shearModG * 7. / 3.) / density); }
    void update(RealType);
    void revert();
    void save();

    ////////////////////////////////////////////////////////////////////////////////
    std::vector<Mat3x3<RealType> >        stress0, defGrad0;
    std::vector<RealType>                 volume0;
    const RealType                        twoThirds, bulkModK, shearModG, density;
    const std::vector<Mat3x3<RealType> >& velGrad;
    std::vector<Mat3x3<RealType> >&       defGrad;
    std::vector<RealType>&                volume;
    std::vector<Mat3x3<RealType> >&       stress;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct J2plasticLinearIsoKin : public ConstitutiveModel<RealType>
{
    J2plasticLinearIsoKin(const material&                       mt,
                          const std::vector<Mat3x3<RealType> >& vg,
                          std::vector<Mat3x3<RealType> >&       dg,
                          std::vector<RealType>&                vl,
                          std::vector<Mat3x3<RealType> >&       st) :
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
        for(int i = 0; i < pnt.size(); ++i)
        {
            pnt[i].strain.fill(0);
            pnt[i].plasticStrain.fill(0);
            pnt[i].backStress.fill(0);
            pnt[i].internalAlpha = 0.;
        }
    }

    RealType waveSpeed() const { return sqrt((bulkModK + shearModG * 7. / 3.) / density); }
    void update(RealType);
    void revert();
    void save();

    ////////////////////////////////////////////////////////////////////////////////
    struct point
    {
        Mat3x3<RealType> strain, plasticStrain, backStress;
        RealType         internalAlpha;
    };
    std::vector<Mat3x3<RealType> >        stress0, defGrad0;
    std::vector<RealType>                 volume0;
    std::vector<point>                    pnt, pnt0;
    const RealType                        oneThird, sqrtTwoThirds, density, bulkModK, shearModG, yieldStress, isoHardMod, kinHardMod;
    const std::vector<Mat3x3<RealType> >& velGrad;
    std::vector<Mat3x3<RealType> >&       defGrad;
    std::vector<RealType>&                volume;
    std::vector<Mat3x3<RealType> >&       stress;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleSolvers/MPM/MPMMaterial.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana