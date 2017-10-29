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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Several algorithmic variations are used, but they all provide these functions
template<class Real>
class TimeIntegration
{
protected:
    const Real initialTime;
    Real       nominalStep, Nstep;

public:
    TimeIntegration(const patch& pch, const constitutiveSC& cst) : initialTime(pch.elapsedTime)
    {
        const Real CFL  = comLineArg("CFL", .5);
        Real       minh = pch.dx;

        if(pch.dy < minh) {
            minh = pch.dy;
        }

        if(pch.dz < minh) {
            minh = pch.dz;
        }

        nominalStep = CFL * minh / cst.waveSpeed();
        const Real totTime = pch.finalTime - initialTime;
        Nstep       = comLineArg("Nstep", totTime / nominalStep); // allow Nstep to modify nominalStep
        nominalStep = comLineArg("dtOveride",
                                 totTime / Nstep);                // allow dtOveride to modify nominalStep
                                                                  // If nominalStep is not modified, then nominalStep = totTime/(totTime/nominalStep) = nominalStep
        report.param("timeStep", nominalStep);
    }

    virtual ~TimeIntegration() {}
    virtual void advance(const Real&) = 0;
    Real         nextTimeStep(const patch& pch) const
    {
        const Real allowedRoundoff = nominalStep * machEps * Real(pch.incCount);
        const Real timeRemaining   = pch.finalTime - pch.elapsedTime;

        if(timeRemaining < nominalStep + allowedRoundoff) {
            return timeRemaining;
        } else {
            return nominalStep;
        }
    }

    int intEstSteps()
    {
        return int(round(Nstep));
    }

protected:
    virtual void makeRes(nodeArray<Vector3>&,
                         nodeArray<Vector3>&,
                         const Real&)
    {
        throw"timeIntSC: implicit solvers must overide this";
    }
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
class momentum : public TimeIntegration
{
    Real            prevStep;
    patch&          pch;
    constitutiveSC& cst;
public:
    momentum(patch& p, constitutiveSC& c) : TimeIntegration(p, c), pch(p), cst(c)
    {
        prevStep = 0.;
    }

    void advance(const Real&);
};

class cenDif : public TimeIntegration
{
    Real            prevStep;
    patch&          pch;
    constitutiveSC& cst;
public:
    cenDif(patch& p, constitutiveSC& c) : TimeIntegration(p, c), pch(p), cst(c)
    {
        prevStep = 0.;
    }

    void advance(const Real&);
};
















void momentum::advance(const Real& dt)
{
    integrate(pch, pch.massP, pch.massN);

    for(int i = 0; i < Nnode(); ++i) {
        pch.massN[i] += tiny;
    }

    for(int i = 0; i < Npart(); ++i) {
        pch.momP[i] = pch.velP[i] * pch.massP[i];
    }

    integrate(pch, pch.momP, pch.momN);

    for(int i = 0; i < Nnode(); ++i) {
        pch.velN[i] = pch.momN[i] / pch.massN[i];
    }

    pch.gridVelocityBC(pch.velN);

    for(int i = 0; i < Nnode(); ++i) {
        pch.velIncN[i] = (-pch.velN[i]);
    }

    pch.makeExternalForces(dt);
    //integrate (pch,pch.setFextP,pch.intFextN);
    divergence(pch, pch.fintN);

    for(int i = 0; i < Nnode(); ++i) {
        pch.velN[i] += (pch.gravityN[i] + pch.fintN[i] / pch.massN[i]) * .5 * (prevStep + dt);
    }

    pch.gridVelocityBC(pch.velN);
    interpolate(pch, pch.posIncP, pch.velN);

    for(int i = 0; i < Npart(); ++i) {
        pch.curPosP[i] += pch.posIncP[i] * dt;
    }

    for(int i = 0; i < Nnode(); ++i) {
        pch.velIncN[i] += pch.velN[i];
    }

    interpolate(pch, pch.velIncP, pch.velIncN);

    for(int i = 0; i < Npart(); ++i) {
        pch.velP[i] += pch.velIncP[i];
    }

    for(int i = 0; i < Npart(); ++i) {
        pch.momP[i] = pch.velP[i] * pch.massP[i];
    }

    integrate(pch, pch.momP, pch.momN);

    for(int i = 0; i < Nnode(); ++i) {
        pch.velN[i] = pch.momN[i] / pch.massN[i];
    }

    pch.gridVelocityBC(pch.velN);
    gradient(pch, pch.velGradP, pch.velN);

    for(int i = 0; i < Npart(); ++i) {
        pch.halfLenP[i] += pch.velGradP[i].diagonal().cwiseProduct(pch.halfLenP[i]) * dt;
    }

    prevStep = dt;
    cst.update(dt);
}

void cenDif::advance(const Real& dt)
{
    for(int i = 0; i < Npart(); ++i) {
        pch.momP[i] = pch.velP[i] * pch.massP[i];
    }

    integrate(pch, pch.massP, pch.massN);

    for(int i = 0; i < Nnode(); ++i) {
        pch.massN[i] += tiny;
    }

    integrate(pch, pch.momP, pch.momN);

    for(int i = 0; i < Nnode(); ++i) {
        pch.velN[i] = pch.momN[i] / pch.massN[i];
    }

    pch.gridVelocityBC(pch.velN);

    for(int i = 0; i < Nnode(); ++i) {
        pch.velIncN[i] = (-pch.velN[i]);
    }

    pch.makeExternalForces(dt);
    divergence(pch, pch.fintN);

    for(int i = 0; i < Nnode(); ++i) {
        pch.velN[i] += (pch.gravityN[i] + pch.fintN[i] / pch.massN[i]) * .5 * (prevStep + dt);
    }

    pch.gridVelocityBC(pch.velN);

    for(int i = 0; i < Nnode(); ++i) {
        pch.velIncN[i] += pch.velN[i];
    }

    interpolate(pch, pch.velIncP, pch.velIncN);
    interpolate(pch, pch.posIncP, pch.velN);
    gradient(pch, pch.velGradP, pch.velN);

    for(int i = 0; i < Npart(); ++i) {
        pch.velP[i] += pch.velIncP[i];
    }

    for(int i = 0; i < Npart(); ++i) {
        pch.curPosP[i] += pch.posIncP[i] * dt;
    }

    for(int i = 0; i < Npart(); ++i) {
        pch.halfLenP[i] += pch.velGradP[i].diagonal().cwiseProduct(pch.halfLenP[i]) * dt;
    }

    prevStep = dt;
    cst.update(dt);
}

void makeTimeInt(timeIntPtr& ti, patch& pch, constitutiveSC* cst, string s)
{
    if(cst == NULL) {
        throw earlyExit("constitutiveSC pointer cst must be set in makePatch");
    } else {
        cst->update(0.);
    }

    if(s == "momentum") {
        ti = new momentum(pch, *cst);
    }

    if(s == "CD") {
        ti = new cenDif(pch, *cst);
    }

    if(s == "quasi") {
        ti = new quasiGM(pch, *cst);
    }

    if(s == "quasiCG") {
        ti = new quasiCG(pch, *cst);
    }

    if(s == "dynGM") {
        ti = new dynGM(pch, *cst);
    }

    if(s == "NewtCG") {
        ti = new dynCG(pch, *cst);
    }

    if(ti == NULL) {
        throw earlyExit("makeTimeInt: no time integrator assigned");
    }
}

template<bool sym>
class DynamicSystem
{
    unsigned                  countRes;
    patch&                    pch;
    constitutiveSC&           cst;
    const nodeArray<Vector3>& vStart;
public:
    DynamicSystem(patch& p, constitutiveSC& c, const nodeArray<Vector3>& v0) : pch(p), cst(c),
        vStart(v0)
    {
        countRes = 0;
    }

    void makeRes(nodeArray<Vector3>& v, nodeArray<Vector3>& res, const Real& delt)
    {
        pch.gridVelocityBC(v);
        cst.revert();
        gradient(pch, pch.velGradP, v);
        cst.update(delt);
        divergence(pch, pch.fintN);

        for(int i = 0; i < Nnode(); ++i) {
            res[i] = vStart[i] + delt * (pch.gravityN[i] + pch.fintN[i] / pch.massN[i]);
        }

        pch.gridVelocityBC(res);

        for(int i = 0; i < Nnode(); ++i) {
            res[i] -= v[i];
        }

        if(sym) {
            for(int i = 0; i < Nnode(); ++i) {
                res[i] *= pch.massN[i];
            }
        }

        ++countRes;
    }
};

class StaticNewton
{
    unsigned                  countRes;
    patch&                    pch;
    constitutiveSC&           cst;
    const nodeArray<Vector3>& vStart;
public:
    StaticNewton(patch& p, constitutiveSC& c, const nodeArray<Vector3>& v0) : pch(p), cst(c),
        vStart(v0)
    {
        countRes = 0;
    }

    void makeRes(nodeArray<Vector3>& v, nodeArray<Vector3>& Z, const Real& delt)
    {
        pch.gridVelocityBC(v);
        cst.revert();
        gradient(pch, pch.velGradP, v);
        cst.update(delt);
        divergence(pch, pch.fintN);

        for(int i = 0; i < Nnode(); ++i) {
            Z[i] = vStart[i] + delt * (pch.gravityN[i] + pch.fintN[i] / pch.massN[i]);
        }

        pch.gridVelocityBC(Z);

        for(int i = 0; i < Nnode(); ++i) {
            Z[i] = -Z[i] * pch.massN[i];
        }

        ++countRes;
    }
};

template<typename eqT>
class NewtonSystem : public eqT
{
    nodeArray<Vector3>        vPert, Zpert;
    const Real                fd;
    const nodeArray<Vector3>& velN;
    const nodeArray<Vector3>& Zcurr;
public:
    NewtonSystem(patch&                    p,
                 constitutiveSC&           c,
                 const nodeArray<Vector3>& v0,
                 const nodeArray<Vector3>& zc) :
        eqT(p, c, v0),
        fd(sqrt(machEps)),
        velN(p.velN),
        Zcurr(zc) {}
    void makeDiff(const nodeArray<Vector3>& s, nodeArray<Vector3>& DZ, const Real& delt)
    {
        const Real dotss  = dot(s, s);
        const Real dotvv  = dot(velN, velN);
        Real       scale2 = dotvv / dotss;
        const Real maxTol = 1e100;
        const Real minTol = 1e0;

        if(scale2 > maxTol) {
            scale2 = maxTol;
        }

        if(scale2 < minTol) {
            scale2 = minTol;
        }

        const Real fd = sqrt(machEps * scale2);

        for(int i = 0; i < Nnode(); ++i) {
            vPert[i] = velN[i] + fd * s[i];
        }

        eqT::makeRes(vPert, Zpert, delt);

        for(int i = 0; i < Nnode(); ++i) {
            DZ[i] = (Zpert[i] - Zcurr[i]) / fd;
        }
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//////// Krylov solvers ///////////////////
template<typename sysT>
class ConjGrad : public sysT
{
    nodeArray<Vector3>        res, P, Q;
    const Real                ratioGMRES, slack;
    Real                      minRatio;
    const int                 iterLim;
    int                       Kiter;
    const nodeArray<Vector3>& Zcurr;
    nodeArray<Vector3>&       vInc;
    ofstream                  cgfile;
    unsigned                  ct;
    unsigned                  CGdivergeCount;
public:
    int solve(const Real& rhoNewton, const Real& delt)
    {
        for(int i = 0; i < Nnode(); ++i) {
            res[i] = -Zcurr[i];
        }

        P = res;
        Real rhoNew2     = rhoNewton * rhoNewton;
        int  lastGoodIdx = 0;
        Real lastGoodRes = 0.;
        minRatio = huge;

        for(Kiter = 0; true; ++Kiter) {
            if(Kiter % 1000 == 0) {
                report.progress("current Kiter", Kiter);
                report.writeProgress();
            }

            sysT::makeDiff(P, Q, delt);
            const Real dotpq = dot(P, Q);
            const Real alpha = rhoNew2 / dotpq;

            for(int i = 0; i < Nnode(); ++i) {
                vInc[i] += alpha * P[i];
            }

            for(int i = 0; i < Nnode(); ++i) {
                res[i] -= alpha * Q[i];
            }

            const Real rhoOld2 = rhoNew2;
            rhoNew2 = dot(res, res);
            const Real ratio = sqrt(rhoNew2) / rhoNewton;

            if(ratio < minRatio) {
                minRatio = ratio;
            }

            if(ratio > slack * minRatio) {
                report.progress("Conjugate Gradient diverged", ++CGdivergeCount);
                break;
            }

            if(ratio < ratioGMRES) {
                break;    // achieved the goal
            }

            if(Kiter > 0) {
                if(rhoNew2 < lastGoodRes) {
                    lastGoodIdx = Kiter;
                    lastGoodRes = rhoNew2;
                } else {
                    if(Kiter - lastGoodIdx > iterLim) {
                        break;    // had iterLim chances, not making progress
                    }
                }
            }

            const Real beta = rhoNew2 / rhoOld2;

            for(int i = 0; i < Nnode(); ++i) {
                P[i] = res[i] + beta * P[i];
            }

            cgfile << ct++ << tab << sqrt(rhoNew2) / rhoNewton << tab << dot(P, P) << tab << dot(Q,
                                                                                                 Q) << tab << dotpq << nwl;
        }

        cgfile << endl;
        return Kiter;
    }

    ConjGrad(patch&                    p,
             constitutiveSC&           c,
             const nodeArray<Vector3>& v0,
             const nodeArray<Vector3>& zc,
             nodeArray<Vector3>&       vi) :
        sysT(p, c, v0, zc),
        ratioGMRES(comLineArg("gmTol", sqrt(machEps))),
        slack(comLineArg("CGslack", 100.)), // further work needed to get this tolerance right
        iterLim(comLineArg("iterLim", 256)),
        Zcurr(zc),
        vInc(vi)
    {
        cgfile.open("cgpq.xls");
        ct             = 0;
        CGdivergeCount = 0;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename sysT>
class GMRES : public sysT
{
    vector<Real>                gNorm, cGiv, sGiv, yMin;
    vector<vector<Real> >       Hess;
    vector<nodeArray<Vector3> > qRes;
    vector<unsigned>            histogram;
    const Real                  ratioGMRES;
    const unsigned              Nstor;
    unsigned                    countBasis;
    const nodeArray<Vector3>&   Zcurr;
    nodeArray<Vector3>&         vInc;
    void multGivens(const int k, Real& x, Real& y)
    {
        const Real tmp = -sGiv[k] * x + cGiv[k] * y;
        x = cGiv[k] * x + sGiv[k] * y;
        y = tmp;
    }

public:
    int solve(const Real& rhoNewton, const Real& delt)
    {
        Real rhoCurr = rhoNewton;
        Real rhoPrev = huge;

        for(int i = 0; i < Nnode(); ++i) {
            qRes[0][i] = -Zcurr[i] / rhoCurr;
        }

        for(unsigned i = 0; i < Hess.size(); ++i) {
            Hess[i].assign(Hess[i].size(), 0.);
        }

        gNorm.assign(gNorm.size(), 0.);
        gNorm[0] = rhoCurr;
        int Kiter = -1;

        while(Kiter < int(Nstor) - 1) {
            if(Kiter % 100 == 0) {
                report.progress("current Kiter", Kiter);
                report.writeProgress();
            }

            ++Kiter;
            sysT::makeDiff(qRes[Kiter], qRes[Kiter + 1], delt);

            for(int j = 0; j <= Kiter; ++j) {
                Hess[j][Kiter] = dot(qRes[Kiter + 1], qRes[j]);

                for(int i = 0; i < Nnode(); ++i) {
                    qRes[Kiter + 1][i] -= Hess[j][Kiter] * qRes[j][i];
                }

                ++countBasis;
            }

            Hess[Kiter + 1][Kiter] = sqrt(dot(qRes[Kiter + 1], qRes[Kiter + 1]));

            for(int i = 0; i < Nnode(); ++i) {
                qRes[Kiter + 1][i] /= Hess[Kiter + 1][Kiter];
            }

            for(int i = 0; i < Kiter; ++i) {
                multGivens(i, Hess[i][Kiter], Hess[i + 1][Kiter]);
            }

            const Real h0 = Hess[Kiter][Kiter];
            const Real h1 = Hess[Kiter + 1][Kiter];
            const Real nu = sqrt(h0 * h0 + h1 * h1);
            cGiv[Kiter]        = Hess[Kiter][Kiter] / nu;
            sGiv[Kiter]        = Hess[Kiter + 1][Kiter] / nu;
            Hess[Kiter][Kiter] = cGiv[Kiter] * Hess[Kiter][Kiter] + sGiv[Kiter] * Hess[Kiter +
                                                                                       1][Kiter];
            Hess[Kiter + 1][Kiter] = 0.;
            multGivens(Kiter, gNorm[Kiter], gNorm[Kiter + 1]);
            rhoPrev = rhoCurr;
            rhoCurr = abs(gNorm[Kiter + 1]);

            if(rhoCurr < rhoNewton * ratioGMRES) {
                break;    // achieved the goal
            }

            if(Kiter > 0 && rhoCurr > rhoPrev) {   // GMRES bottomed out - we need to make a choice
                if(rhoCurr > rhoNewton) {
                    cerr << "!! Discarding GMRES result; vInc=0" << endl;
                    return -1;                       // discard entire GMRES result - vInc=0
                }

                --Kiter;                            // discard this but keep the previous
                break;
            }
        }

        ++histogram[Kiter];
        yMin[Kiter] = gNorm[Kiter] / Hess[Kiter][Kiter];

        for(int i = Kiter - 1; i > -1; --i) {
            yMin[i] = gNorm[i];

            for(int j = i + 1; j <= Kiter; ++j) {
                yMin[i] -= Hess[i][j] * yMin[j];
            }

            yMin[i] /= Hess[i][i];
        }

        for(int i = 0; i < Nnode(); ++i) {
            for(int j = 0; j <= Kiter; ++j) {
                vInc[i] += qRes[j][i] * yMin[j];
            }
        }

        return Kiter;
    }

    GMRES(patch&                    p,
          constitutiveSC&           c,
          const nodeArray<Vector3>& v0,
          const nodeArray<Vector3>& zc,
          nodeArray<Vector3>&       vi) :
        sysT(p, c, v0, zc),
        ratioGMRES(comLineArg("gmTol", sqrt(machEps))),
        Nstor(comLineArg("Nstor", 128)),
        Zcurr(zc),
        vInc(vi)
    {
        report.param("Nstor", Nstor);
        cGiv.resize(Nstor);
        sGiv.resize(Nstor);
        gNorm.resize(Nstor + 1);
        yMin.resize(Nstor + 1);
        qRes.resize(Nstor + 1, nodeArray<Vector3>());
        Hess.resize(Nstor + 1, vector<Real>(Nstor));
        histogram.resize(Nstor);

        for(unsigned i = 0; i < histogram.size(); ++i) {
            histogram[i] = 0;
        }

        countBasis = 0;
    }

    ~GMRES()
    {
        cerr << "Histogram of required GMRES iterations per call" << endl;

        for(unsigned i = 0; i < histogram.size(); ++i) {
            if(histogram[i] > 0) {
                cerr << "  iters " << setw(5) << i + 1 << "  times reached " << histogram[i] << endl;
            }
        }

        report.param("countBasis", countBasis);
    }
};

//////// Newton-Krylov ////////////////////////

template<typename solverT>
class NewtonKrylov : public solverT
{
    ofstream                  newtFile;
    nodeArray<Vector3>        vInc, Zcurr, vTry;
    const Real                ratioNewton, ignoreResidualBelow;
    Real                      rhoStart;
    const unsigned            NewtLim, maxLoadIter;
    patch&                    pch;
    constitutiveSC&           cst;
    const nodeArray<Vector3>& vStart;
public:
    Real startRes() const
    {
        return rhoStart;
    }

    void solve(nodeArray<Vector3>& velN, const Real& delt)
    {
        Real loadResPrev = huge;

        for(unsigned Nload = 0; Nload < maxLoadIter; ++Nload) {
            cst.revert();
            pch.makeExternalForces(delt);
            solverT::makeRes(velN, Zcurr, delt);
            Real rhoCurr = sqrt(dot(Zcurr, Zcurr));
            Real rhoPrev = huge;

            if(Nload == 0) {
                rhoStart = rhoCurr;
                report.progress("rhoStart", rhoStart);
            } else {
                cerr << Nload << " load res: abs / rel " << rhoCurr / rhoStart << " / " << rhoCurr /
                    loadResPrev << endl;

                if(rhoCurr < rhoStart * ratioNewton) {
                    break;
                }

                if(rhoCurr / loadResPrev > 1.) {
                    throw earlyExit("nonlinear load convergence failed");
                }

                report.progress("nonlinear load loop", Nload);
            }

            report.writeProgress();
            unsigned Nnewton = 0;
            loadResPrev = rhoCurr;

            while(rhoCurr > ignoreResidualBelow && rhoCurr >= rhoStart * ratioNewton) {
                ++Nnewton;
                report.progress("Newton step", Nnewton);

                for(int i = 0; i < Nnode(); ++i) {
                    vInc[i] = Vector3(0, 0, 0);
                }

                const int Kiter = solverT::solve(rhoCurr, delt);
                report.progress("ending Kiter", Kiter);

                for(int ArmijoCount = 0; ArmijoCount < 8; ++ArmijoCount) {
                    for(int i = 0; i < Nnode(); ++i) {
                        vTry[i] = velN[i] + vInc[i] / pow(2, ArmijoCount);
                    }

                    solverT::makeRes(vTry, Zcurr, delt);
                    rhoPrev = rhoCurr;
                    rhoCurr = sqrt(dot(Zcurr, Zcurr));
                    newtFile << rhoCurr / rhoStart << tab << rhoCurr / rhoPrev << tab << Kiter + 1 << endl;
                    report.progress("rhoCurr/rhoStart", rhoCurr / rhoStart);
                    report.progress("rhoCurr/rhoPrev",  rhoCurr / rhoPrev);
                    report.writeProgress();

                    if(rhoCurr < rhoStart * ratioNewton) {
                        velN = vTry;    // achieved the goal - go to next time step
                        break;
                    }

                    if(rhoCurr < rhoPrev) {
                        velN = vTry;    // made progress - try another Newton
                        break;
                    }

                    if(Nnewton > 1 && ArmijoCount >= 7) {              // Armijo failed - discard result
                        report.progress("(Armijo) residual not satisfied", rhoCurr / rhoStart);
                        return;
                    }

                    report.progress("Invoking Armijo rhoCurr/rhoStart", rhoCurr / rhoStart);
                }

                if(Nnewton >= NewtLim) {
                    report.progress("(Newton) residual not satisfied", rhoCurr / rhoStart);
                    return;
                }
            }
        }
    }

    NewtonKrylov(patch&                    p,
                 constitutiveSC&           c,
                 const nodeArray<Vector3>& v0) :
        solverT(p, c, v0, Zcurr, vInc),
        ratioNewton(comLineArg("NewtTol", sqrt(machEps))),
        ignoreResidualBelow(comLineArg("ignore", tiny)),
        rhoStart(-huge),
        NewtLim(comLineArg("NewtLim", 8)),
        maxLoadIter(comLineArg("maxLoadIter", 32)),
        pch(p),
        cst(c),
        vStart(v0)
    {
        newtFile.open("newton.xls");
        newtFile.precision(14);
    }

    ~NewtonKrylov()
    {
        newtFile.close();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Solvers
template<typename NewtT>
class QuasiStatic : public NewtT, public TimeIntegration
{
    nodeArray<Vector3> vStart;
    Real               theta;
    Real               startRes;
    patch&             pch;
    constitutiveSC&    cst;
public:
    QuasiStatic(patch&          p,
                constitutiveSC& cc) :
        NewtT(p, cc, vStart),
        TimeIntegration(p, cc),
        //NewtT(p,cc,p.velN),
        theta(comLineArg("theta", .5)),
        startRes(-huge),
        pch(p),
        cst(cc) {}
    void advance(const Real& dt)
    {
        integrate(pch, pch.massP, pch.massN);
        Real maxMass = 0.;

        for(int i = 0; i < Nnode(); ++i) {
            if(pch.massN[i] > maxMass) {
                maxMass = pch.massN[i];
            }
        }

        const Real minMass = machEps * maxMass;

        for(int i = 0; i < Nnode(); ++i) {
            if(pch.massN[i] < minMass) {
                pch.massN[i] = minMass;
            }
        }

        for(int i = 0; i < Nnode(); ++i) {
            pch.velN[i] = Vector3(0, 0, 0);
        }

        pch.gridVelocityBC(pch.velN);
        vStart = pch.velN;
        cst.save();
        NewtT::solve(pch.velN, theta * dt);

        if(pch.incCount == 0) {
            startRes = NewtT::startRes();
        }

        cst.revert();
        interpolate(pch, pch.posIncP, pch.velN);
        gradient(pch, pch.velGradP, pch.velN);

        for(int i = 0; i < Npart(); ++i) {
            pch.curPosP[i] += pch.posIncP[i] * dt;
        }

        for(int i = 0; i < Npart(); ++i) {
            pch.halfLenP[i] += pch.velGradP[i].diagonal().cwiseProduct(pch.halfLenP[i]) * dt;
        }

        cst.update(dt);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename NewtT>
class ImplicitDynamic : public NewtT, public TimeIntegration
{
    nodeArray<Vector3> vStart;
    const Real         theta;
    patch&             pch;
    constitutiveSC&    cst;
public:
    ImplicitDynamic(patch&          p,
                    constitutiveSC& cc) :
        NewtT(p, cc, vStart),
        TimeIntegration(p, cc),
        //NewtT(p,cc,p.velN),
        theta(comLineArg("theta", .5)),
        pch(p),
        cst(cc) {}
    void advance(const Real& dt)
    {
        integrate(pch, pch.massP, pch.massN);
        Real maxMass = 0.;

        for(int i = 0; i < Nnode(); ++i) {
            if(pch.massN[i] > maxMass) {
                maxMass = pch.massN[i];
            }
        }

        const Real minMass = machEps * maxMass;

        for(int i = 0; i < Nnode(); ++i) {
            if(pch.massN[i] < minMass) {
                pch.massN[i] = minMass;
            }
        }

        for(int i = 0; i < Npart(); ++i) {
            pch.momP[i] = pch.velP[i] * pch.massP[i];
        }

        integrate(pch, pch.momP, pch.momN);

        for(int i = 0; i < Nnode(); ++i) {
            pch.velN[i] = pch.momN[i] / pch.massN[i];
        }

        pch.gridVelocityBC(pch.velN);
        vStart = pch.velN;
        cst.save();
        NewtT::solve(pch.velN, theta * dt);
        cst.revert();

        for(int i = 0; i < Nnode(); ++i) {
            pch.velIncN[i] = (pch.velN[i] - vStart[i]) / theta;
        }

        interpolate(pch, pch.velIncP, pch.velIncN);

        for(int i = 0; i < Npart(); ++i) {
            pch.velP[i] += pch.velIncP[i];
        }

        interpolate(pch, pch.posIncP, pch.velN);
        gradient(pch, pch.velGradP, pch.velN);

        for(int i = 0; i < Npart(); ++i) {
            pch.curPosP[i] += pch.posIncP[i] * dt;
        }

        for(int i = 0; i < Npart(); ++i) {
            pch.halfLenP[i] += pch.velGradP[i].diagonal().cwiseProduct(pch.halfLenP[i]) * dt;
        }

        cst.update(dt);
    }
};

typedef QuasiStatic<NewtonKrylov<GMRES<NewtonSystem<StaticNewton> > > >                  quasiGM;
typedef QuasiStatic<NewtonKrylov<ConjGrad<NewtonSystem<StaticNewton> > > >               quasiCG;
typedef ImplicitDynamic<NewtonKrylov<GMRES<NewtonSystem<DynamicSystem<false> > > > >     dynGM;
typedef ImplicitDynamic<NewtonKrylov<ConjGrad<NewtonSystem<DynamicSystem<true> > > > >   dynCG;



#endif
