#ifndef BVECT_H
#define BVECT_H

#include "darray.h"
#include "pltwin.h"

void BVectFromCircuit(PltWin *pw, int iV, IVector at, DVector &B, double &Blen);

#endif
