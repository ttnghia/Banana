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

#pragma once

#include <ParticleSolvers/PICFluid/PIC_Data.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers {
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct APIC_Data : PIC_Data<N, RealType> {
    struct APIC_ParticleData : PIC_Data<N, RealType>::PIC_ParticleData {
        __BNN_TYPE_ALIASING
        ////////////////////////////////////////////////////////////////////////////////
        // affine matrices
        Vec_MatXxX<N, RealType> C;
        ////////////////////////////////////////////////////////////////////////////////
        virtual void reserve(UInt nParticles) override {
            PIC_Data<N, RealType>::PIC_ParticleData::reserve(nParticles);
            C.reserve(nParticles);
        }

        virtual void addParticles(const Vec_VecN& newPositions, const Vec_VecN& newVelocities, const JParams& jParams = JParams()) override {
            PIC_Data<N, RealType>::PIC_ParticleData::addParticles(newPositions, newVelocities);
            C.resize(this->getNParticles(), MatXxX<N, RealType>(0));
        }

        virtual UInt removeParticles(const Vec_Int8& removeMarker) override {
            STLHelpers::eraseByMarker(C, removeMarker);
            return PIC_Data<N, RealType>::PIC_ParticleData::removeParticles(removeMarker);
        }
    };

    virtual void initialize() override {
        APIC_particleData  = std::make_shared<APIC_ParticleData>();
        this->particleData = std::static_pointer_cast<typename PIC_Data<N, RealType>::PIC_ParticleData>(APIC_particleData);

        this->gridData = std::make_shared<typename PIC_Data<N, RealType>::PIC_GridData>();
    }

    ////////////////////////////////////////////////////////////////////////////////
    SharedPtr<APIC_ParticleData> APIC_particleData = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleSolvers
