#include "darray.h"
#include "matinv.h"

/*
  Inverse of a square matrix M with n rows and columns using the LU
  decomposition.

  We are given a system of linear algebraic equations [A]{x}={b} with full
  matrix [A] and unknown elements of the vector {x}.
  The matrix [A] is decomposed into a product of a lower triangular matrix [L] 
  and an upper triangular matrix [U] such that [A]=[L][U].
  Two systems are to be solved: [L][U]{x}={b} and then [L]([U]{x})={b} in two
  steps:
    1. solve [L]{y}={b} for unknown {y}
    2. solve [U]{x}={y} for known {y} and unknown {x}
*/

void matinv(int n, DMatrix a, DMatrix &inva)
{
  DMatrix aa;
  DVector b;
  IVector indx;
  int i, j, d;

  aa.Allocate(n,n);
  b.Allocate(n);
  indx.Allocate(n);

  // be careful not to change the matrix 'a' outside
  aa = a;

  // set up identity matrix
  inva = 0.0;
  for (i=1; i<=n; i++)
    inva(i,i) = 1.0;

  // decompose the matrix
  ludcmp(aa, n, indx, d);

  // find inverse by columns
  for (j=1; j<=n; j++) {
    for (i=1; i<=n; i++)
      b(i) = inva(i,j);
    lubksb(aa, n, indx, b);
    for (i=1; i<=n; i++)
      inva(i,j) = b(i);
  }

  aa.Free();
  b.Free();
  indx.Free();
}


/*
  Solving a set of linear algebraic equations f(x1,x2,...,xN)=0 using the
  LU decomposition. Described in the Numerical Recipes.
*/

void ludcmp(DMatrix &a, int n, IVector &indx, int &d)
{
  double tiny=1e-20;

  DVector b, vv;
  double aamax, absAij, sum, dum;
  int i, j, k, imax;

  b.Allocate(n);
  vv.Allocate(n);

  d = 1;

  // get the maximum element in each row of matrix a
  for (i=1; i<=n; i++) {
    aamax = 0.0;
    for (j=1; j<=n; j++) {
      absAij = a(i,j);
      if (absAij < 0) absAij = -absAij;
      if (absAij > aamax)  
	aamax = absAij;
    }
     
    if (aamax == 0) {
      printf("ludcmp ::  Singular matrix of the system.\n");
      printf("%0.4lf %0.4lf %0.4lf\n", a(1,1), a(1,2), a(1,3));
      printf("%0.4lf %0.4lf %0.4lf\n", a(2,1), a(2,2), a(2,3));
      printf("%0.4lf %0.4lf %0.4lf\n", a(3,1), a(3,2), a(3,3));
      exit(0);
    }
     
    vv(i) = 1.0/aamax;
  }

  for (j=1; j<=n; j++) {                 // Crout's method
    for (i=1; i<=j-1; i++) {             // calculate the upper triangular matrix
      sum = a(i,j);
      for (k=1; k<=i-1; k++)
	sum = sum - a(i,k)*a(k,j);
      a(i,j) = sum;
    }

    aamax = 0.0;
    for (i=j; i<=n; i++) {
      sum = a(i,j);
      for (k=1; k<=j-1; k++) 
	sum = sum - a(i,k)*a(k,j);       
      a(i,j) = sum;
      dum = vv(i)*abs(sum);             // if this element better than the lastly chosen one ?
      if (dum >= aamax) {
	imax = i;
	aamax = dum;
      }
    }

    if (j != imax) {                    // swap the rows ?
      for (k=1; k<=n; k++) {                 
	dum = a(imax,k);
	a(imax,k) = a(j,k);
	a(j,k) = dum;
      }
      d = -d;                           // ... and change the parity of d
      vv(imax) = vv(j);                 // change the scaling factor
    }
     
    indx(j) = imax;
    if (a(j,j) == 0)  a(j,j) = tiny;    // if the pivot is zero => singular matrix
     
    if (j != n) {                    // divide by the pivot
      dum = 1.0/a(j,j);
      for (i=j+1; i<=n; i++)
	a(i,j) = a(i,j)*dum;
    }
  }

  b.Free();
  vv.Free();
}


/*
  Solution of the system [a]{x}={b}. Here, the matrix [a] is substituted by 
  the decomposed matrix assembled by the algorithm above. The vector <indx>
  contains the permutation vector and the vector {b} is the real right-hand
  side vector.
*/

void lubksb(DMatrix a, int n, IVector indx, DVector &b)
{
  double sum;
  int i, ii, j, ll;

  ii = 0;                                   
  for (i=1; i<=n; i++) {                     // forward substitution.
    ll = indx(i);
    sum = b(ll);
    b(ll) = b(i);

    if (ii != 0) {
      for (j=ii; j<=i-1; j++)
	sum = sum - a(i,j)*b(j);
    } else if (sum != 0) {
      ii = i;                               // from now on, make the summations above
    }

    b(i) = sum;
  }

  for (i=n; i>=1; i--) {                    // back substitution
    sum = b(i);
    for (j=i+1; j<=n; j++)
      sum = sum - a(i,j)*b(j);
    b(i) = sum/a(i,i);
  }
}
