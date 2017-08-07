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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
const Vec_Vec3<ScalarType>& AnisotropicKernelGenerator<ScalarType>::getKernelCenters() const
{
    return m_KernelCenters;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
const Vec_Mat3x3<ScalarType>& AnisotropicKernelGenerator<ScalarType>::getKernelMatrices() const
{
    return m_KernelMatrices;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
void AnisotropicKernelGenerator<ScalarType>::setParticleRadius(ScalarType radius)
{
    m_KernelRadius    = 8 * radius;
    m_KernelRadiusSqr = m_KernelRadius * m_KernelRadius;
    m_KernelRadiusInv = 1.0 / m_KernelRadius;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
void AnisotropicKernelGenerator<ScalarType>::generateAnisotropy()
{
    ////////////////////////////////////////////////////////////////////////////////
    // allocate memory
    m_KernelCenters.resize(m_Particles.size());
    m_KernelMatrices.resize(m_Particles.size());

    ////////////////////////////////////////////////////////////////////////////////
    // compute
    tbb::parallel_for(tbb::blocked_range<size_t>(0, m_Particles.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p)
                          {
                              ////////////////////////////////////////////////////////////////////////////////
                              // compute kernel center and weighted mean position
                              const Vec3& ppos = m_Particles[p];
                              const Vec3i& pcellId = m_Grid3D.getCellIdx<int>(ppos);
                              Vec3 pposWM(0);
                              ScalarType sumW = 0;

                              for(int lk = -m_KernelCellSpan; lk <= m_KernelCellSpan; ++lk)
                              {
                                  for(int lj = -m_KernelCellSpan; lj <= m_KernelCellSpan; ++lj)
                                  {
                                      for(int li = -m_KernelCellSpan; li <= m_KernelCellSpan; ++li)
                                      {
                                          const Vec3i cellId = pcellId + Vec3i(li, lj, lk);

                                          if(!m_Grid3D.isValidCell<int>(cellId))
                                          {
                                              continue;
                                          }

                                          for(UInt32 q : m_CellParticles(cellId))
                                          {
                                              const Vec3& qpos = m_Particles[q];
                                              const Vec3 xpq = qpos - ppos;
                                              const ScalarType d2 = glm::length2(xpq);
                                              if(d2 < m_KernelRadiusSqr)
                                              {
                                                  const ScalarType wpq = W(d2);
                                                  sumW += wpq;
                                                  pposWM += wpq * qpos;
                                              }
                                          }
                                      }
                                  }
                              } // end loop over neighbor cells

                              __BNN_ASSERT(sumW > 0);
                              pposWM /= sumW;
                              m_KernelCenters[p] = (1.0 - AniGen_Lambda) * ppos + AniGen_Lambda * pposWM;

                              ////////////////////////////////////////////////////////////////////////////////
                              // compute covariance matrix and anisotropy matrix
                              Mat3x3<ScalarType> C(0);
                              UInt32 neighborCount = 0;
                              sumW = 0;

                              for(int lk = -m_KernelCellSpan; lk <= m_KernelCellSpan; ++lk)
                              {
                                  for(int lj = -m_KernelCellSpan; lj <= m_KernelCellSpan; ++lj)
                                  {
                                      for(int li = -m_KernelCellSpan; li <= m_KernelCellSpan; ++li)
                                      {
                                          const Vec3i cellId = pcellId + Vec3i(li, lj, lk);

                                          if(!m_Grid3D.isValidCell<int>(cellId))
                                          {
                                              continue;
                                          }

                                          for(UInt32 q : m_CellParticles(cellId))
                                          {
                                              const Vec3& qpos = m_Particles[q];
                                              const Vec3 xpq = qpos - pposWM;
                                              const ScalarType d2 = glm::length2(xpq);

                                              if(d2 < m_KernelRadiusSqr)
                                              {
                                                  const ScalarType wpq = W(d2);
                                                  sumW += wpq;
                                                  C += wpq * glm::outerProduct(xpq, xpq);

                                                  ++neighborCount;
                                              }
                                          }
                                      }
                                  }
                              } // end loop over neighbor cells

                              __BNN_ASSERT(sumW > 0);
                              C /= sumW; // = covariance matrix

                              ////////////////////////////////////////////////////////////////////////////////
                              // compute kernel matrix
                              Mat3x3 U, S, V;

                              SVDDecomposition::svd(C[0][0], C[0][1], C[0][2], C[1][0], C[1][1], C[1][2], C[2][0], C[2][1], C[2][2],
                                                    U[0][0], U[0][1], U[0][2], U[1][0], U[1][1], U[1][2], U[2][0], U[2][1], U[2][2],
                                                    S[0][0], S[0][1], S[0][2], S[1][0], S[1][1], S[1][2], S[2][0], S[2][1], S[2][2],
                                                    V[0][0], V[0][1], V[0][2], V[1][0], V[1][1], V[1][2], V[2][0], V[2][1], V[2][2]);

                              Vec3 sigmas = static_cast<ScalarType>(0.75) * Vec3(1, 1, 1);;

                              if(neighborCount > AniGen_NeighborCountThreshold)
                              {
                                  sigmas = Vec3(S[0][0], std::max(S[1][1], S[0][0] / AniGen_Kr), std::max(S[2][2], S[0][0] / AniGen_Kr));
                                  ScalarType ks = std::cbrt(1.0 / (sigmas[0] * sigmas[1] * sigmas[2])); // scale so that det(covariance) == 1
                                  sigmas *= ks;
                              }

                              m_KernelMatrices[p] = glm::transpose(U) * Mat3x3(Vec3<ScalarType>(sigmas[0], 0, 0),
                                                                               Vec3<ScalarType>(0, sigmas[1], 0),
                                                                               Vec3<ScalarType>(0, 0,         sigmas[2])) * U;
                          }
                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
ScalarType AnisotropicKernelGenerator<ScalarType>::W(ScalarType d2)
{
    return (d2 < m_KernelRadiusSqr) ? 1.0 - MathHelpers::cube(sqrt(d2) * m_KernelRadiusInv) : 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
ScalarType AnisotropicKernelGenerator<ScalarType>::W(const Vec3<ScalarType>& r)
{
    return W(glm::length2(r));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
ScalarType AnisotropicKernelGenerator<ScalarType>::W(const Vec3<ScalarType>& xi, const Vec3<ScalarType>& xj)
{
    return W(glm::length2(xi - xj));
}
