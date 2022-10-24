#ifndef MISC_H
#define MISC_H

#include "darray.h"

double vDistance( double *v1, double *v2, int nitems=3 );
double vDotProduct( double v1[], double v2[], int nitems=3 );
void dswap( double &val1, double &val2 );
void shuffle(int n, IVector &idx);
double mod(double a, double b);
DMatrix mtransf(DMatrix &csys1, DMatrix &csys2);
DVector mvmult(DMatrix &m, DVector &v);
  
#endif
