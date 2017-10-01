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
#define AniGen_Lambda                 0.5
#define AniGen_NeighborCountThreshold 25
#define AniGen_Kr                     4

#include <Banana/LinearAlgebra/SVD.h>
#include <Banana/LinearAlgebra/ImplicitQRSVD.h>
#include <Banana/LinearAlgebra/LinaHelpers.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>
#include <SurfaceReconstruction/AniKernelGenerator.h>

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
                                          Vec3r pposWM(0);
                                          Real sumW = 0;

                                          for(UInt q : d0.neighbors(0, p)) {
                                              const Vec3r& qpos = m_Particles[q];
                                              const Vec3r xpq = qpos - ppos;
                                              const Real d2 = glm::length2(xpq);
                                              const Real wpq = W(d2);
                                              sumW += wpq;
                                              pposWM += wpq * qpos;
                                          }

                                          __BNN_ASSERT(sumW > 0);
                                          pposWM /= sumW;
                                          m_KernelCenters[p] = Real(1.0 - AniGen_Lambda) * ppos + Real(AniGen_Lambda) * pposWM;

                                          ////////////////////////////////////////////////////////////////////////////////
                                          // compute covariance matrix and anisotropy matrix
                                          Mat3x3<Real> C(0);
                                          sumW = 0;
                                          for(UInt q : d0.neighbors(0, p)) {
                                              const Vec3r& qpos = m_Particles[q];
                                              const Vec3r xpq = qpos - pposWM;
                                              const Real d2 = glm::length2(xpq);
                                              const Real wpq = W(d2);
                                              sumW += wpq;
                                              C += wpq * glm::outerProduct(xpq, xpq);
                                          }

                                          __BNN_ASSERT(sumW > 0);
                                          C /= sumW;   // => covariance matrix

                                          ////////////////////////////////////////////////////////////////////////////////
                                          // compute kernel matrix
                                          Mat3x3r U, V;
                                          Vec3r S;

                                          //SVDDecomposition::svd(C[0][0], C[0][1], C[0][2], C[1][0], C[1][1], C[1][2], C[2][0], C[2][1], C[2][2],
                                          //                      U[0][0], U[0][1], U[0][2], U[1][0], U[1][1], U[1][2], U[2][0], U[2][1], U[2][2],
                                          //                      S[0][0], S[0][1], S[0][2], S[1][0], S[1][1], S[1][2], S[2][0], S[2][1], S[2][2],
                                          //                      V[0][0], V[0][1], V[0][2], V[1][0], V[1][1], V[1][2], V[2][0], V[2][1], V[2][2]);
                                          QRSVD::svd(C, U, S, V);

                                          Vec3r sigmas = Vec3r(0.75);

                                          if(d0.n_neighbors(0, p) > AniGen_NeighborCountThreshold) {
                                              sigmas = Vec3r(S[0], MathHelpers::max(S[1], S[0] / AniGen_Kr), MathHelpers::max(S[2], S[0] / AniGen_Kr));
                                              Real ks = std::cbrt(Real(1.0) / (sigmas[0] * sigmas[1] * sigmas[2]));   // scale so that det(covariance) == 1
                                              sigmas *= ks;
                                          }

                                          //for(UInt j = 0; j < 3; ++j) {
                                          //    U[j] = glm::normalize(U[j]);
                                          //}

                                          //m_KernelMatrices[p] = glm::transpose(U) * LinaHelpers::diagMatrix(sigmas) * U;
                                          m_KernelMatrices[p] = U * LinaHelpers::diagMatrix(sigmas) * glm::transpose(V);
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana