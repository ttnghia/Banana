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

#define AniGen_Lambda                 0.5
#define AniGen_NeighborCountThreshold 25
#define AniGen_Kr                     4

#include <Banana/LinearAlgebra/ImplicitQRSVD.h>
#include <Banana/LinearAlgebra/LinaHelpers.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>
#include <SurfaceReconstruction/AniKernelGenerator.h>
#include <Banana/Utils/NumberHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AnisotropicKernelGenerator::generateAniKernels()
{
    m_KernelCenters.resize(m_NParticles);
    m_KernelMatrices.resize(m_NParticles);

    ////////////////////////////////////////////////////////////////////////////////
    m_NSearch->find_neighbors();
    const auto& d0 = m_NSearch->point_set(0);

    ParallelFuncs::parallel_for<UInt>(0, m_NParticles,
                                      [&](UInt p)
                                      {
                                          ////////////////////////////////////////////////////////////////////////////////
                                          // compute kernel center and weighted mean position
                                          const Vec3r& ppos = m_Particles[p];
                                          Vec3r pposWM      = ppos;
                                          Real sumW         = Real(1.0);

                                          for(UInt q : d0.neighbors(0, p)) {
                                              const Vec3r& qpos = m_Particles[q];
                                              const Vec3r xpq   = qpos - ppos;
                                              const Real d2     = glm::length2(xpq);
                                              const Real wpq    = W(d2);
                                              sumW   += wpq;
                                              pposWM += wpq * qpos;
                                          }

                                          pposWM            /= sumW;
                                          m_KernelCenters[p] = Real(1.0 - AniGen_Lambda) * ppos + Real(AniGen_Lambda) * pposWM;

                                          ////////////////////////////////////////////////////////////////////////////////
                                          if(d0.n_neighbors(0, p) < AniGen_NeighborCountThreshold) {
                                              m_KernelMatrices[p] = Mat3x3r(m_DefaultSpraySize);
                                              return;
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          // compute covariance matrix and anisotropy matrix
                                          Mat3x3<Real> C(0);
                                          sumW = Real(0);
                                          for(UInt q : d0.neighbors(0, p)) {
                                              const Vec3r& qpos = m_Particles[q];
                                              const Vec3r xpq   = qpos - pposWM;
                                              const Real d2     = glm::length2(xpq);
                                              const Real wpq    = W(d2);
                                              sumW += wpq;
                                              C    += wpq * glm::outerProduct(xpq, xpq);
                                          }

                                          C /= sumW;   // => covariance matrix

                                          ////////////////////////////////////////////////////////////////////////////////
                                          // compute kernel matrix
                                          Vec3r S;
                                          Mat3x3r U, V;
                                          QRSVD::svd(C, U, S, V);

                                          Vec3r sigmas = Vec3r(S[0], MathHelpers::max(S[1], S[0] / AniGen_Kr), MathHelpers::max(S[2], S[0] / AniGen_Kr));
                                          sigmas             *= std::cbrt(Real(1.0) / glm::compMul(sigmas)); // scale so that det(covariance) == 1
                                          m_KernelMatrices[p] = U * LinaHelpers::diagMatrix(sigmas) * glm::transpose(V);
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana