#ifndef GEOM_H
#define GEOM_H

#include "darray.h"

bool LineCutPoly2D(int n, DMatrix V, DMatrix P, DVector &tV, DVector &tP, DMatrix &Q, 
		   int &iV1, DMatrix &V1, int &iV2, DMatrix &V2, IVector &idx1, IVector &idx2);

bool isconvex(int nv, DMatrix v);

#endif
