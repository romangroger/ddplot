#include "darray.h"

/*
  Linear interpolation from points A,B. Returns the functional value
  of this line at point x.  
*/

double interp1(DVector A, DVector B, double x)
{
  double res;
  
  res = A(2) + (x-A(1))*(B(2)-A(2))/(B(1)-A(1));
  return res;
}


/*
  Constructs the Lagrange cubic polynomial interpolating points
  A,B,C and returns the functional value of this polynomial at point x.
*/

double interp2(DVector A, DVector B, DVector C, double x)
{
  double termA, termB, termC, res;
  
  termA = A(2) * (x-B(1))*(x-C(1))/((A(1)-B(1))*(A(1)-C(1)));
  termB = B(2) * (x-A(1))*(x-C(1))/((B(1)-A(1))*(B(1)-C(1)));
  termC = C(2) * (x-A(1))*(x-B(1))/((C(1)-A(1))*(C(1)-B(1)));

  res = termA + termB + termC;
  return res;
}
