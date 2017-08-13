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
        bulkModK(Y / (3. * (1. - 2. * P))),
        shearModG(Y / (2. * (1. + P))),
        yieldStress(c),
        isoHardMod(d),
        kinHardMod(e)
    {
        if(density < 0.) throw std::exception("invalid density");
        if(Y < 0.) throw std::exception("invalid Young's modulus");
        if(P > .5 - numeric_limits<RealType>::epsilon() || P < 0.) throw std::exception("invalid Poisson's ratio");
        if(yieldStress < 0.) throw std::exception("invalid yield stress - no plasticity found in plastic model");
        if(isoHardMod < 0.) throw std::exception("invalid isotropic hardening modulus");
        if(kinHardMod < 0.) throw std::exception("invalid kinematic hardening modulus");
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct ConstitutiveModel
{
    virtual void update(const Real&) = 0;
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
struct NeoHookean : public ConstitutiveModel
{
    vector<Matrix33>           defGrad0, stress0;
    vector<RealType>           volume0;
    const RealType             Ymod, Pois, Dens, lam, mu;
    const partArray<Matrix33>& velGrad;
    partArray<Matrix33>&       defGrad;
    partArray<RealType>&       volume;
    partArray<Matrix33>&       stress;
    void update(const RealType&);
    void revert();
    void save();
    RealType waveSpeed() const { return sqrt((lam + 3. * mu) / Dens); }
    NeoHookean(const RealType             Y,
               const RealType             P,
               const RealType             D,
               const partArray<Matrix33>& vg,
               partArray<Matrix33>&       dg,
               partArray<RealType>&       vl,
               partArray<Matrix33>&       st) :
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
        if(Npart() < 1) throw std::exception("NeoHookean:no particles to initialize!");
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct UpdatedElastic : public ConstitutiveModel
{
    vector<Matrix33>           stress0, defGrad0;
    vector<RealType>           volume0;
    const RealType             twoThirds, bulkModK, shearModG, density;
    const partArray<Matrix33>& velGrad;
    partArray<Matrix33>&       defGrad;
    partArray<RealType>&       volume;
    partArray<Matrix33>&       stress;
    void update(const RealType&);
    void revert();
    void save();
    RealType waveSpeed() const { return sqrt((bulkModK + shearModG * 7. / 3.) / density); }
    UpdatedElastic(const partArray<Matrix33>& vg,
                   partArray<Matrix33>&       dg,
                   partArray<RealType>&       vl,
                   partArray<Matrix33>&       st,
                   const material&            a) :
        twoThirds(2. / 3.),
        bulkModK(a.bulkModK),
        shearModG(a.shearModG),
        density(a.density),
        velGrad(vg),
        defGrad(dg),
        volume(vl),
        stress(st)
    {}
};


struct J2plasticLinearIsoKin : public ConstitutiveModel
{
    struct point
    {
        Matrix33 strain, plasticStrain, backStress;
        RealType internalAlpha;
    };
    vector<Matrix33>           stress0, defGrad0;
    vector<RealType>           volume0;
    partArray<point>           pnt, pnt0;
    const RealType             oneThird, sqrtTwoThirds, density, bulkModK, shearModG, yieldStress, isoHardMod, kinHardMod;
    const partArray<Matrix33>& velGrad;
    partArray<Matrix33>&       defGrad;
    partArray<RealType>&       volume;
    partArray<Matrix33>&       stress;
    void update(const RealType&);
    void revert();
    void save();
    RealType waveSpeed() const { return sqrt((bulkModK + shearModG * 7. / 3.) / density); }
    J2plasticLinearIsoKin(const material&            mt,
                          const partArray<Matrix33>& vg,
                          partArray<Matrix33>&       dg,
                          partArray<RealType>&       vl,
                          partArray<Matrix33>&       st) :
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
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana