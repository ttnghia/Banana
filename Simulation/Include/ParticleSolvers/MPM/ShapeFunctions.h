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



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct shapeSC
{
    virtual void updateContribList() = 0;
    virtual ~shapeSC() {}
};
typedef shapeSC * shapePtr;

void makeShape(shapePtr & shp, patch & pch, string);

// integration and interpolation operators
void integrate(const patch& pch, const partArray<Real>& pu, nodeArray<Real>& gu);
void integrate(const patch& pch, const partArray<Vector3>& pu, nodeArray<Vector3>& gu);
void divergence(const patch& pch, nodeArray<Vector3>& gu);
void interpolate(const patch& pch, partArray<Vector3>& pu, const nodeArray<Vector3>& gu);
void gradient(const patch& pch, partArray<Matrix33>& pu, const nodeArray<Vector3>& gu);

// io.cpp - utility functions for setting up arrangements of particles
void fillBox(patch& pch, const Vector3& b, const Vector3& e, Vector3 ppe = Vector3(2., 2.,
                                                                                   2.));
int partProbe(const patch& pch, const Vector3& pos);
int nodeProbe(const patch& pch, const Vector3& pos);

// constitutive super class
struct constitutiveSC
{
    virtual void update(const Real&) = 0;
    virtual void revert()
    {
        throw earlyExit("constitutiveSC: revert() and save() must be defined if using implicit methods");
    }

    virtual void save()
    {
        throw earlyExit("constitutiveSC: revert() and save() must be defined if using implicit methods");
    }

    virtual Real waveSpeed() const = 0;
    virtual ~constitutiveSC() {}
};
typedef constitutiveSC * constitPtr;

// Several algorithmic variations are used, but they all provide these functions
class timeIntSC
{
protected:
    const Real initialTime;
    Real       nominalStep, Nstep;
public:
    timeIntSC(const patch& pch, const constitutiveSC& cst) : initialTime(pch.elapsedTime)
    {
        const Real CFL  = comLineArg("CFL", .5);
        Real       minh = pch.dx;

        if(pch.dy < minh)
        {
            minh = pch.dy;
        }

        if(pch.dz < minh)
        {
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

    virtual ~timeIntSC() {}
    virtual void advance(const Real&) = 0;
    Real nextTimeStep(const patch& pch) const
    {
        const Real allowedRoundoff = nominalStep * machEps * Real(pch.incCount);
        const Real timeRemaining   = pch.finalTime - pch.elapsedTime;

        if(timeRemaining < nominalStep + allowedRoundoff)
        {
            return timeRemaining;
        }
        else
        {
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
typedef timeIntSC * timeIntPtr;
void makeTimeInt(timeIntPtr& ti, patch& pch, constitutiveSC* cst, string s);

// Convenience class for a geometric series of load factors
// Useful for non-linear problems such as pull-in
class geomStepSeries
{
    Real dt;
    Real beta;
    int  N;
    bool alreadyInit;
public:
    geomStepSeries()
    {
        alreadyInit = false;
    }

    void init(Real lastFactor, int totSteps)
    {
        if(alreadyInit)
        {
            throw earlyExit("geomStepSeries may only be initialized once!");
        }

        dt = lastFactor;
        N  = totSteps;

        if(totSteps > 1 && abs(1. / Real(totSteps) - lastFactor) > machEps)
        {
            beta = 1.1;
            Real fcurr = 1. - dt * (pow(beta, N) - 1.) / (beta - 1.);
            Real fprev = 0.;

            for(int c = 0; c < 2 || abs(fcurr) < fprev; ++c)
            {
                fprev = abs(fcurr);
                const Real df = dt * ((pow(beta, N) - 1.) / ((beta - 1.) * (beta - 1.)) - (pow(beta,
                                                                                               N) * Real(N)) / (beta * (beta - 1.)));
                beta -= fcurr / df;
                fcurr = 1. - dt * (pow(beta, N) - 1.) / (beta - 1.);
                cerr << "geomStepSeries: Newton-Raphson: f / beta: " << fcurr << " / " << beta << endl;
            }

            cerr << "geomStepSeries: beta: " << beta << endl;
            cerr << "geomStepSeries: first factor: " << factorI(0) << endl;
        }
        else
        {
            beta = 1.;
        }

        alreadyInit = true;
    }

    Real factorI(const int I) const
    {
        if(beta == 1.)
        {
            return Real(I + 1) * dt;
        }

        return 1. - dt * (pow(beta, N - (I + 1)) - 1.) / (beta - 1.); // big to small
                                                                      //return dt*(pow(beta,(I+1))-1.)/(beta-1.);    // small to big (should check before use)
    }
};

// Every input file must define initRun
void initRun(patchPtr&, constitPtr&, timeIntPtr&, shapePtr&);
