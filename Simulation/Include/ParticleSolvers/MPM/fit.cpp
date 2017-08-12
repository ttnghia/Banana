// Copyright 2009,2011 Philip Wallstedt
// 
// This software is covered by the following FreeBSD-style license:
// 
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following 
// conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following 
//    disclaimer in the documentation and/or other materials provided with the distribution.
// 
// This software is provided "as is" and any express or implied warranties, including, but not limited to, the implied warranties 
// of merchantability and fitness for a particular purpose are disclaimed.  In no event shall any authors or contributors be 
// liable for any direct, indirect, incidental, special, exemplary, or consequential damages (including, but not limited to, 
// procurement of substitute goods or services; loss of use, data, or profits; or business interruption) however caused and on any 
// theory of liability, whether in contract, strict liability, or tort (including negligence or otherwise) arising in any way out 
// of the use of this software, even if advised of the possibility of such damage.

#include<iostream>
#include<iomanip>
#include<sstream>
#include<fstream>
#include<vector>
#include<limits>
#include<cassert>
#include<cstdlib>
#include<cmath>
using namespace std;
const double pi=3.1415926535897932384626433832795028841971694;

template<const int d>class vec{
   double v[d];
public:
   vec(){}
   vec(const double r){for(int i=0;i<d;++i)v[i]=r;}
   vec(const vec<d>&r){for(int i=0;i<d;++i)v[i]=r.v[i];}
   double&operator[](const int i)     {assert(i>=0&&i<d);return v[i];}
   double operator[](const int i)const{assert(i>=0&&i<d);return v[i];}
   vec<d>&operator =(const double&r){for(int i=0;i<d;i+=1)v[i] =r;return*this;}
};

template<const int d>class mat{
   double m[d*d];
public:
   const int n;
   mat():n(d*d){}
   mat(const double r):n(d*d){for(int i=0;i<n;++i)m[i]=r;}
   mat(const mat<d>&r):n(d*d){for(int i=0;i<n;++i)m[i]=r.m[i];}
   double&operator[](int i)     {assert(i>=0&&i<n);return m[i];}
   double operator[](int i)const{assert(i>=0&&i<n);return m[i];}
   mat<d>&operator =(const double&r){for(int i=0;i<n;i+=1)m[i] =r;return*this;}
};

template<const int d>inline void Crout(const mat<d>&a,mat<d>&A){
   for(int k=0;k<d;++k){
      for(int i=k;i<d;++i){
         double sum=0.;
         for(int p=0;p<k;++p)sum+=A[i*d+p]*A[p*d+k];
         A[i*d+k]=a[i*d+k]-sum;
      }
      for(int j=k+1;j<d;++j){
         double sum=0.;
         for(int p=0;p<k;++p)sum+=A[k*d+p]*A[p*d+j];
         A[k*d+j]=(a[k*d+j]-sum)/A[k*d+k];
      }
   }
}
template<const int d>inline void solveCrout(const mat<d>&A,const vec<d>&b,vec<d>&x){
   vec<d>y;
   for(int i=0;i<d;++i){
      double sum=0.;
      for(int k=0;k<i;++k)sum+=A[i*d+k]*y[k];
      y[i]=(b[i]-sum)/A[i*d+i];
   }
   for(int i=d-1;i>=0;--i){
      double sum=0.;
      for(int k=i+1;k<d;++k)sum+=A[i*d+k]*x[k];
      x[i]=(y[i]-sum);
   }
}

bool checkInput(const vector<double>&T,const vector<double>&V){
   bool goodInput=true;
   if(T.size()!=V.size()){
      cerr<<"fit.cpp: V vs T input size mismatch"<<endl;
      goodInput=false;
   }
   if(T.size()<4){ // finding four unknowns, need four points
      cerr<<"fit.cpp: too few points to fit"<<endl;
      goodInput=false;
   }
   const double inf=numeric_limits<double>::infinity();
   for(unsigned i=0;i<T.size();++i){
      if(T[i]!=T[i] || V[i]!=V[i]){
         cerr<<"fit.cpp: nan found in input"<<endl;
         goodInput=false;
         break;
      }
   }
   for(unsigned i=0;i<T.size();++i){
      if(T[i]==inf || T[i]==-inf ||
         V[i]==inf || V[i]==-inf){
         cerr<<"fit.cpp: inf found in input"<<endl;
         goodInput=false;
         break;
      }
   }
   return goodInput;
}

double findNperiods(const vector<double>&V){
   double currV=V[0];
   unsigned idx=0;
   unsigned Nzero=0;
   bool keepGoing=true;
   while(V[idx]*currV>0.){
      ++idx;
      if(idx>=V.size()){
         cerr<<"Warning: less than one period"<<endl;
         return .1;
      }
   }
   const unsigned NtoFirst0=idx;
   unsigned prior0=idx;
   ++Nzero;
   while(keepGoing){
      while(idx-prior0<NtoFirst0){
         ++idx;
         if(idx>=V.size()){keepGoing=false;break;}

      }
      if(!keepGoing)break;
      currV=V[idx];
      while(V[idx]*currV>0.){
         ++idx;
         if(idx>=V.size()){keepGoing=false;break;}
      }
      prior0=idx;
      ++Nzero;
   }
   return .5*(double(Nzero)-.5)*double(V.size()-1)/double(prior0);
}

void makeFit(const vector<double>&T,const vector<double>&V,const string fitName){
   if(checkInput(T,V)==false)return;
   const double Nper=findNperiods(V);
   double sumV=0.;
   for(unsigned i=0;i<V.size();++i)sumV+=V[i];
   const double avgV=sumV/double(V.size());
   sumV=0.;
   for(unsigned i=0;i<V.size();++i){
      const double diff=V[i]-avgV;
      sumV+=diff*diff;
   }
   const double SStot=sumV; // used later to find the goodness-of-fit or "R-squared"
   double amp=V[0]; // initial values for coefficients beta = amp,sigma,omega,phi
   double sigma=0.;
   double omega=Nper*2.*pi/(T[T.size()-1]-T[0]);
   double phi=0.;
   vec<4>rhs,betaInc;
   mat<4>moment;
   double resCurr=numeric_limits<double>::max();
   double resPrev=resCurr,Rsqr=0.;
   cerr<<"======== Begin Gauss-Newton nonlinear fit of vibration data ========"<<endl;
   cerr<<"Number of samples:"<<T.size()<<endl;
   const int fw=24; // field width for output
   cerr<<setw(fw)<<"amp"<<setw(fw)<<"sigma"<<setw(fw)<<"omega"<<setw(fw)<<"phi"<<setw(fw)<<"resCurr"<<setw(fw)<<"goodness-of-fit R^2"<<endl;
   cerr.precision(17);
   cerr<<setw(fw)<<amp<<setw(fw)<<sigma<<setw(fw)<<omega<<setw(fw)<<phi<<endl;
   const double tol=1.-numeric_limits<double>::epsilon();
   for(int i=0;i<2||resCurr<tol*resPrev;++i){
      resPrev=resCurr;
      resCurr=0.;
      moment=0.;
      rhs=0.;
      double SSerr=0.;
      Rsqr=0.;
      // Re-form moment and rhs based on current values of beta
      for(unsigned i=0;i<T.size();++i){ // sum contributions from input points
         const double t=T[i];
         const double e=exp(-sigma*t);
         const double c=cos(omega*t+phi);
         const double s=sin(omega*t+phi);
         const double r=V[i]-amp*e*c; // error between this data point and the current equation
         const double J0=e*c;
         const double J1=-amp*t*e*c;
         const double J2=-amp*t*e*s;
         const double J3=-amp*e*s;
         moment[ 0]+=J0*J0;
         moment[ 1]+=J0*J1;
         moment[ 2]+=J0*J2;
         moment[ 3]+=J0*J3;
         moment[ 5]+=J1*J1;
         moment[ 6]+=J1*J2;
         moment[ 7]+=J1*J3;
         moment[10]+=J2*J2;
         moment[11]+=J2*J3;
         moment[15]+=J3*J3;
         rhs[0]+=J0*r;
         rhs[1]+=J1*r;
         rhs[2]+=J2*r;
         rhs[3]+=J3*r;
         SSerr+=r*r;
      }
      Rsqr=1.-SSerr/SStot;  // statistical measure of the goodness-of-fit
      resCurr=sqrt(SSerr);
      moment[ 4]=moment[1]; // moment matrix must be symmetric
      moment[ 8]=moment[2];
      moment[ 9]=moment[6];
      moment[12]=moment[3];
      moment[13]=moment[7];
      moment[14]=moment[11];
      Crout(moment,moment);           // LU decomposition
      solveCrout(moment,rhs,betaInc); // back substitution
      amp+=betaInc[0];
      sigma+=betaInc[1];
      omega+=betaInc[2];
      phi+=betaInc[3];
      cerr<<setw(fw)<<amp<<setw(fw)<<sigma<<setw(fw)<<omega<<setw(fw)<<phi<<setw(fw)<<resCurr<<setw(fw)<<Rsqr<<endl;
   }
   cerr<<"Decaying sinusoid equation: amp*exp(-sigma*t)*cos(omega*t+phi)"<<endl;
   cerr<<"Amplitude:  "<<amp<<endl;
   cerr<<"Frequency (rad): "<<omega<<endl;
   cerr<<"Frequency (Hz):  "<<omega/(2.*pi)<<endl;
   const double w0=sqrt(omega*omega+sigma*sigma);
   cerr<<"Natural Frequency (rad): "<<w0<<endl;
   cerr<<"Natural Frequency (Hz):  "<<w0/(2.*pi)<<endl;
   cerr<<"Phase (rad):"<<phi<<endl;
   cerr<<"Phase (Hz): "<<phi/(2.*pi)<<endl;
   cerr<<"Damping ratio: "<<sigma/w0<<endl;
   cerr<<"Quality factor: "<<1./(2.*sigma/w0)<<endl;
   cerr<<"goodness-of-fit R^2: "<<Rsqr<<endl;
   cerr<<"Nper="<<Nper<<endl;
   ofstream fitFile(fitName.c_str());
   fitFile.precision(17);
   fitFile<<"Nper="<<Nper<<endl;
   fitFile<<"amp="<<amp<<endl;
   fitFile<<"actFreq="<<omega<<endl;
   fitFile<<"natFreq="<<w0<<endl;
   fitFile<<"phase="<<phi<<endl;
   fitFile<<"decay="<<sigma<<endl;
   fitFile<<"dampRatio="<<sigma/w0<<endl;
   fitFile<<"Qfactor="<<1./(2.*sigma/w0)<<endl;
   fitFile<<"goodness-of-fit="<<Rsqr<<endl;
   cerr<<"======== End nonlinear fit ========================================="<<endl;
}

// Uncomment int main()... if you want to run fit.cpp as a stand-alone
// application with a command-line argument to your data, which must
// be arranged as two columns, separated by white space.  Run as:
// prompt> ./a.out my.data

//int main(int argc,char**argv){
   //assert(argc==2);
   //ifstream tf(argv[1]);
   //vector<double>T,V;
   //double a,b;
   //while(tf>>a>>b){
      //T.push_back(a);
      //V.push_back(b);
   //}
   //makeFit(T,V);
//}
