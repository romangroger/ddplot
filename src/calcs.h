#ifndef CALCS_H
#define CALCS_H

#include "qstring.h"
#include "darray.h"

double AtomicMass( QString elem );
int CalcIndex( int item, int stacklen );
QString CreateFName( QString fname, QString ext );
bool isequal( double num1, double num2, double tol=1e-4 );
void mtransf( DMatrix csys1, DMatrix csys2, DMatrix &mat );

#endif
