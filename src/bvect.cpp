#include "bvect.h"
#include "darray.h"
#include "pltwin.h"


/*
  Calculate the Burgers vector of the dislocation enclosed by the Burgers circuit that passes
  throught the atoms in at. It keeps track of the periodic slab in the z-direction to account
  for the periodicity in this direction.
*/

void BVectFromCircuit(PltWin *pw, int iV, IVector at, DVector &B, double &Blen)
{
  DVector ur(3), rr(3);
  double d2best, dx, dy, dz, d2, s, zslab, sbest;
  int i, j, ai1, ai2, ar1, ar2, ai2best, ai1save;

  zslab = 0;
  for (i=1; i<iV; i++) {
    ar1 = at(i);
    ar2 = at(i+1);
    if (i==1) {
      ai1 = ai1save = ar1;
      for (j=1; j<=3; j++)
	rr(j) = pw->xyzRel(ar1,j);
    }
    for (j=1; j<=3; j++) {
      ur(j) = pw->xyzRel(ar2,j) - pw->xyzRel(ar1,j);
      rr(j) += ur(j);
    }

    d2best = 1e20;
    ai2best = 0;
    sbest = 0;
    for (ai2=1; ai2<=pw->NInit; ai2++) {
      if (!pw->zLayerSel(pw->zLayer(ai2))) continue;
      dx = pw->xyzInit(ai2,1) - (pw->xyzInit(ai1,1)+ur(1));
      dy = pw->xyzInit(ai2,2) - (pw->xyzInit(ai1,2)+ur(2));
      dz = pw->xyzInit(ai2,3) - (pw->xyzInit(ai1,3)+ur(3));
      if (abs(dz)>=pw->period(3)/2.0) {
	s = dz/fabs(dz);
	dz -= s*pw->period(3);
      } else
	s = 0;
      d2 = pow(dx,2) + pow(dy,2) + pow(dz,2);
      if (d2 < d2best) {
	d2best = d2;
	ai2best = ai2;
	sbest = s;
      }
    }
    zslab += sbest;
    ai1 = ai2best;
  }

  B(1) = pw->xyzInit(ai1save,1) - pw->xyzInit(ai2best,1);
  B(2) = pw->xyzInit(ai1save,2) - pw->xyzInit(ai2best,2);
  B(3) = pw->xyzInit(ai1save,3) - (zslab*pw->period(3) + pw->xyzInit(ai2best,3));
  Blen = sqrt(pow(B(1),2) + pow(B(2),2) + pow(B(3),2));
}
