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
template<Int N, class RealType>
Banana::AnisotropicKernelGenerator<N, RealType>::AnisotropicKernelGenerator(UInt        nParticles,
                                                                            const VecN* particles,
                                                                            RealType    particleRadius,
                                                                            RealType    defaultSpraySize,
                                                                            RealType    kernelRatio) :
    m_nParticles(nParticles), m_Particles(particles), m_DefaultSpraySize(defaultSpraySize)
{
    m_KernelRadius    = kernelRatio * particleRadius;
    m_KernelRadiusSqr = m_KernelRadius * m_KernelRadius;
    m_KernelRadiusInv = RealType(1.0) / m_KernelRadius;

    m_NSearch = std::make_unique<NeighborSearch::NeighborSearch<N, RealType>>(m_KernelRadius, true);
    m_NSearch->add_point_set(glm::value_ptr(m_Particles[0]), m_nParticles, true, true);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void AnisotropicKernelGenerator<N, RealType >::setParameters(RealType positionBlending /*= RealType(0.5)*/,
                                                             RealType axisRatio /*= RealType(4.0)*/,
                                                             UInt     neighborThredhold /*= 25u*/)
{
    m_PositionBlending  = positionBlending;
    m_AxisRatio         = axisRatio;
    m_NeighborThredhold = neighborThredhold;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void AnisotropicKernelGenerator<N, RealType >::computeAniKernels(Vec_VecN&   kernelCenters,
                                                                 Vec_MatNxN& kernelMatrices)
{
    kernelCenters.resize(m_nParticles);
    kernelMatrices.resize(m_nParticles);

    ////////////////////////////////////////////////////////////////////////////////
    m_NSearch->find_neighbors();
    const auto& d0 = m_NSearch->point_set(0);
    Scheduler::parallel_for(m_nParticles,
                            [&](UInt p)
                            {
                                const auto& ppos = m_Particles[p];
                                auto sumW        = RealType(1.0);
                                auto pposWM      = ppos;
                                auto C           = MatNxN(0);

                                for(auto q : d0.neighbors(0, p)) {
                                    const auto& qpos = m_Particles[q];
                                    const auto xpq   = qpos - ppos;
                                    const auto d2    = glm::length2(xpq);
                                    const auto wpq   = W2(d2);
                                    sumW   += wpq;
                                    pposWM += wpq * qpos;
                                    C      += wpq * glm::outerProduct(xpq, xpq);
                                }

                                pposWM /= sumW; // => weighted mean of positions
                                C      /= sumW; // => covariance matrix

                                kernelCenters[p] = MathHelpers::lerp(ppos, pposWM, m_PositionBlending);
                                ////////////////////////////////////////////////////////////////////////////////
                                if(d0.n_neighbors(0, p) < m_NeighborThredhold) {
                                    kernelMatrices[p] = MatNxN(m_DefaultSpraySize);
                                    return;
                                }

                                ////////////////////////////////////////////////////////////////////////////////
                                // compute kernel matrix
                                VecN S;
                                MatNxN U, V;
                                QRSVD::svd(C, U, S, V);
                                VecN sigmas = S;

                                for(Int i = 1; i < N; ++i) {
                                    sigmas[i] = MathHelpers::max(S[i], S[0] / m_AxisRatio);
                                }

                                sigmas           *= std::cbrt(RealType(1.0) / glm::compMul(sigmas)); // scale so that det(covariance) == 1
                                kernelMatrices[p] = U * LinaHelpers::diagMatrix(sigmas) * glm::transpose(V);
                            });
}
