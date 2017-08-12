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

#include "patch.h"
using namespace std;

patch::patch(const int Nx,const int Ny,const int Nz,const Vector3 lo,const Vector3 hi):
               regionLo(lo),
               regionHi(hi),
               dx((hi[0]-lo[0])/Real(Nx)),
               dy((hi[1]-lo[1])/Real(Ny)),
               dz((hi[2]-lo[2])/Real(Nz)),
               I(Nx+3), // must keep full boundary layer for GIMP
               J(Ny+3),
               K(Nz+3){
   globalNodeArrays.resizeAll(I*J*K);
   elapsedTime=0.;    // default
   finalTime=huge;  // default (go forever)
   incCount=0;
   // laying out nodes in a grid
   for(int k=0;k<K;++k){
      const Real z=(k-1)*dz+regionLo[2];
      for(int j=0;j<J;++j){
         const Real y=(j-1)*dy+regionLo[1];
         for(int i=0;i<I;++i){
            const Real x=(i-1)*dx+regionLo[0];
            curPosN[k*I*J+j*I+i]=Vector3(x,y,z);
         }
      }
   }
   report.param("regionLo",regionLo);
   report.param("regionHi",regionHi);
   report.param("cell sizes",Vector3(dx,dy,dz));
   report.param("Icols",I);        
   report.param("Jrows",J);        
   report.param("Klayers",K);        
   report.param("Nnode",Nnode());        
}





