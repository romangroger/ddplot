#include <math.h>

#include "darray.h"


// Calculation of the distance between two atoms given by vectors atom1 and atom2
double vDistance( double *v1, double *v2, int nitems=3 )
{
  double dist;
  int d;

  dist = 0;
  for (d=0; d<nitems; d++)
    dist += pow(v1[d]-v2[d],2);
  dist = sqrt(dist);

  return dist;
}


double vDotProduct( double v1[], double v2[], int nitems=3 )
{
  double res;
  int n;

  res = 0;
  for (n=0; n<nitems; n++)
    res += v1[n]*v2[n];

  return res;
}


void dswap( double &val1, double &val2 )
{
  double keep;

  keep = val1;
  val1 = val2;
  val2 = keep; 
}


/*
  The Fisher-Yates algorithm for generating a random permutation of a set S of n indices. The
  indices into the set are returned in idx so that S(idx(1)) is the first element of the set,
  etc.  
*/

void shuffle(int n, IVector &idx)
{
  int i, j, dum;

  for (i=1; i<=n; i++)
    idx(i) = i;

  for (i=n; i>=2; i--) {
    j = 1 + round((i-2)*(float)rand()/RAND_MAX);   // j=1..i-1
    dum = idx(i);
    idx(i) = idx(j);
    idx(j) = dum;
  }
}


/* Calculates mod(a/b) */

double mod(double a, double b)
{
  double res;
  res = a - b*trunc(a/b);
  return(res);
}


/* Calculates the transformation matrix from csys1 to csys2 */

DMatrix mtransf(DMatrix &csys1, DMatrix &csys2)
{
  DMatrix cs1(3,3), cs2(3,3), mat(3,3);
  double len1, len2;
  int i, d, j;
  
  for (i=1; i<=3; i++) {
    len1 = sqrt(pow(csys1(i,1),2) + pow(csys1(i,2),2) + pow(csys1(i,3),2));
    len2 = sqrt(pow(csys2(i,1),2) + pow(csys2(i,2),2) + pow(csys2(i,3),2));
    for (d=1; d<=3; d++) {
      cs1(i,d) = csys1(i,d)/len1;
      cs2(i,d) = csys2(i,d)/len2;
    }
  }

  for (j=1; j<=3; j++) {
    for (i=1; i<=3; i++) {
      mat(i,j) = cs2(i,1)*cs1(j,1) + cs2(i,2)*cs1(j,2) + cs2(i,3)*cs1(j,3); 
    }
  }

  return(mat);
}


/* Matrix-vector multiplication */

DVector mvmult(DMatrix &m, DVector &v)
{
  DVector res(3);
  int i, j;
  
  for (i=1; i<=3; i++) {
    res(i) = 0.0;
    for (j=1; j<=3; j++) {
      res(i) += m(i,j)*v(j);
    }
  }

  return(res);
}
