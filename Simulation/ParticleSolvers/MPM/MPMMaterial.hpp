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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// could optimize revert so arrays are swapped rather than copied
template<class Real>
void NeoHookean<Real >::revert()
{
    for(int i = 0; i < Npart(); ++i) {
        defGrad[i] = defGrad0[i];
    }

    for(int i = 0; i < Npart(); ++i) {
        stress[i] = stress0[i];
    }

    for(int i = 0; i < Npart(); ++i) {
        volume[i] = volume0[i];
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// could optimize save so resize is only called once
template<class Real>
void NeoHookean<Real >::save()
{
    defGrad0.resize(Npart());
    stress0.resize(Npart());
    volume0.resize(Npart());

    for(int i = 0; i < Npart(); ++i) {
        defGrad0[i] = defGrad[i];
    }

    for(int i = 0; i < Npart(); ++i) {
        stress0[i] = stress[i];
    }

    for(int i = 0; i < Npart(); ++i) {
        volume0[i] = volume[i];
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
void NeoHookean<Real >::update(Real delt)
{
    for(int i = 0; i < Npart(); ++i) {
        defGrad[i] += inner(velGrad[i], (defGrad[i])) * delt;
        const Real J = defGrad[i].determinant();
        volume[i] += velGrad[i].trace() * volume[i] * delt;
        stress[i]  = I3(lam * log(J) / J) + (mu / J) * (inner(defGrad[i], defGrad[i].transpose()) - I3());
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// could optimize revert so arrays are swapped rather than copied
template<class Real>
void UpdatedElastic<Real >::revert()
{
    for(int i = 0; i < Npart(); ++i) {
        defGrad[i] = defGrad0[i];
    }

    for(int i = 0; i < Npart(); ++i) {
        stress[i] = stress0[i];
    }

    for(int i = 0; i < Npart(); ++i) {
        volume[i] = volume0[i];
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// could optimize save so resize is only called once
template<class Real>
void UpdatedElastic<Real >::save()
{
    defGrad0.resize(Npart());
    stress0.resize(Npart());
    volume0.resize(Npart());

    for(int i = 0; i < Npart(); ++i) {
        defGrad0[i] = defGrad[i];
    }

    for(int i = 0; i < Npart(); ++i) {
        stress0[i] = stress[i];
    }

    for(int i = 0; i < Npart(); ++i) {
        volume0[i] = volume[i];
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
void UpdatedElastic<Real >::update(Real delt)
{
    for(int i = 0; i < Npart(); ++i) {
        defGrad[i] += inner(velGrad[i], defGrad[i]) * delt; // Nanson works for all
        const Mat3x3<Real> strainInc(.5 * (velGrad[i] + velGrad[i].transpose()) * delt);
        //defGrad[i]+=strainInc; // Nanson works with shear problem, but not with fixed-fixed beam
        const Real evol = strainInc.trace();
        volume[i] += evol * volume[i];
        stress[i] += 2. * shearModG * strainInc + I3((bulkModK - twoThirds * shearModG) * evol);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// could optimize revert so arrays are swapped rather than copied
template<class Real>
void J2plasticLinearIsoKin<Real >::revert()
{
    for(int i = 0; i < Npart(); ++i) {
        defGrad[i] = defGrad0[i];
    }

    for(int i = 0; i < Npart(); ++i) {
        stress[i] = stress0[i];
    }

    for(int i = 0; i < Npart(); ++i) {
        volume[i] = volume0[i];
    }

    for(int i = 0; i < Npart(); ++i) {
        pnt[i] = pnt0[i];
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// could optimize save so resize is only called once
template<class Real>
void J2plasticLinearIsoKin<Real >::save()
{
    defGrad0.resize(Npart());
    stress0.resize(Npart());
    volume0.resize(Npart());
    pnt0.resize(Npart());

    for(int i = 0; i < Npart(); ++i) {
        defGrad0[i] = defGrad[i];
    }

    for(int i = 0; i < Npart(); ++i) {
        stress0[i] = stress[i];
    }

    for(int i = 0; i < Npart(); ++i) {
        volume0[i] = volume[i];
    }

    for(int i = 0; i < Npart(); ++i) {
        pnt0[i] = pnt[i];
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
void J2plasticLinearIsoKin<Real >::update(Real delt)
{
    for(int i = 0; i < Npart(); ++i) {
        defGrad[i] += inner(velGrad[i], defGrad[i]) * delt;
        point&             pt = pnt[i];
        const Mat3x3<Real> strainInc(.5 * (velGrad[i] + velGrad[i].transpose()) * delt);
        const Real         evol = strainInc.trace();
        volume[i] += volume[i] * evol;
        pt.strain += strainInc;
        const Mat3x3<Real> devStrainInc(dev(strainInc));
        const Mat3x3<Real> dilStressInc(I3(bulkModK* evol));
        const Mat3x3<Real> trialStress(dev(stress[i]) + 2. * shearModG* devStrainInc);
        const Real         hardLaw     = yieldStress + isoHardMod * pt.internalAlpha;
        const Real         ffYieldCond = trialStress.norm() - sqrtTwoThirds * hardLaw;

        if(ffYieldCond <= 0.) {
            stress[i] += 2. * shearModG * devStrainInc + dilStressInc;
        } else {
            const Mat3x3<Real> unitStress(trialStress / trialStress.norm());
            const Real         gammaInc = ffYieldCond / (2. * (shearModG + oneThird *
                                                               (isoHardMod + kinHardMod)));
            const Mat3x3<Real> plasticStrainInc(gammaInc* unitStress);
            pt.internalAlpha += sqrtTwoThirds * gammaInc;
            pt.backStress    += (sqrtTwoThirds * kinHardMod * gammaInc) * unitStress;
            pt.plasticStrain += plasticStrainInc;
            stress[i]        += 2. * shearModG * (devStrainInc - plasticStrainInc) + dilStressInc;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}  // end namespace Banana::ParticleSolvers

