#ifndef MATINV_H
#define MATINV_H

#include "darray.h"

void matinv(int n, DMatrix a, DMatrix &inva);
void ludcmp(DMatrix &a, int n, IVector &indx, int &d);
void lubksb(DMatrix a, int n, IVector indx, DVector &b);

#endif
