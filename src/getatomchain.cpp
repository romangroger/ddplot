#include "pltwin.h"

/*
  Find all atoms that lie on the line given by the two atoms a1, a2
  when considering the positions of atoms in the perfect lattice.
*/

void getatomchain(PltWin *pw, int a1, int a2, int &nsel, IVector &asel)
{
  double tol=1e-4;
  double x0, y0, x1, y1, x2, y2, x01, y01, x12, y12, nondot, mag;
  int i;

  x1 = pw->xyzInit(a1,1);
  y1 = pw->xyzInit(a1,2);
  x2 = pw->xyzInit(a2,1);
  y2 = pw->xyzInit(a2,2);
  x12 = x2 - x1;
  y12 = y2 - y1;
  mag = sqrt(pow(x12,2)+pow(y12,2));
  x12 /= mag;       // (x12,y12) is a unit vector from a1 to a2
  y12 /= mag;

  nsel = 0;
  for (i=1; i<=pw->NInit; i++) {
    x0 = pw->xyzInit(i,1);
    y0 = pw->xyzInit(i,2);
    x01 = x1 - x0;
    y01 = y1 - y0;
    mag = sqrt(pow(x01,2)+pow(y01,2));
    if (mag != 0) {
      x01 /= mag;   // (x01,y01) is a unit vector from i to a1
      y01 /= mag;
      nondot = fabs(1.0 - fabs(x01*x12 + y01*y12));
    }
    if (mag == 0 || nondot <= tol) {  // keep atoms that are on the line a1...a2
      nsel++;
      asel(nsel) = i;
    }
  }
}













