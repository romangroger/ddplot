#include <qbrush.h>
#include <qdialog.h>
#include <qpen.h>
#include <qstring.h>
#include <qtablewidget.h>

#include "ui_ddpospnresults.h"

#include "dpospnresults.h"
#include "lagrpoly.h"
#include "getatomchain.h"
#include "messages.h"
#include "pltwin.h"

/*
  The procedure findDPosPeierlsNabarro (see below) is used to identify the dislocation position in a
  chain of atoms and their corresponding relative displacements in the direction of the Burgers
  vector. If this is done for the two chains of atoms above and below the slip plane, we obtain two
  positions of the dislocation - one in the chain above and the other in the chain below. These two
  points define a line segment. If this procedure is repeated for at least two different
  orientations of the possible slip plane (for screw dislocations in bcc crystals, these are {110}
  planes), we can identify the dislocation position as the intersection of these two lines. This
  calculation is done in this procedure. If more than two line segments are available (i.e. more
  than two slip planes exist), this procedure calculates the intersection between each pair of line
  segments. In general, this provides a polygon whose corners are the intersections of the individual
  pairs of line segments identified above. The position of the dislocation can then be associated
  with the geometrical center, although this choice is not always the best one. 

  This is called after we double-click on the last atom.
*/

void CalcDPosPeierlsNabarro(PltWin *pw, QString &txtXY, QString &txtDPos)
{
  QString txt;
  char buf[80];
  double x1, y1, x2, y2, x, yi, yj;
  int i, imax, nln, j, k, n;

  // determine the parameters a, b in the equations of type y=a*x+b that connect the calculated
  // positions of atoms in each pair of parallel chains of atoms
  imax = trunc(pw->nchain/2.0);
  DVector aln(imax), bln(imax);
  nln = 0;
  for (i=1; i<=pw->nchain; i+=2) {
    x1 = pw->dposchain(i,1);
    y1 = pw->dposchain(i,2);
    x2 = pw->dposchain(i+1,1);
    y2 = pw->dposchain(i+1,2);
    nln++;
    aln(nln) = (y2-y1)/(x2-x1);
    bln(nln) = y1 - aln(nln)*x1;
  }

  n = nln*(nln-1)/2;
  pw->dpoly.Free();
  pw->dpoly.Allocate(n,2);

  // get the intersection points of each pair of lines
  pw->xdavg = pw->ydavg = 0.0;
  k = 0;
  for (i=1; i<=nln; i++) {
    for (j=i+1; j<=nln; j++) {
      if (aln(i)==aln(j)) {
	msgError(err_DPosParallelLines);
	return;
      }
      x = -(bln(i)-bln(j))/(aln(i)-aln(j));
      yi = aln(i)*x+bln(i);
      yj = aln(j)*x+bln(j);
      if (fabs(yi-yj) > 1e-4) {
	msgError(err_DPosNoIntersection);
	return;
      }
      k = k + 1;
      pw->dpoly(k,1) = x;
      pw->dpoly(k,2) = yi;   // yi=yj (see above)
      pw->xdavg += x;
      pw->ydavg += yi;  // yi=yj (see above)
    }
  }

  // average the positions of the dislocations stored in (xx,yy) to get the estimate for the
  // dislocation position
  pw->xdavg /= double(k);
  pw->ydavg /= double(k);

  // save a polygon with vertices corresponding to the positions of the dislocation
  pw->ndpoly = k;
  for (i=1; i<=k; i++) {
    sprintf(buf, "%8.4f %8.4f\n", pw->dpoly(i,1), pw->dpoly(i,1));
    txtXY.append(buf);
  }
  sprintf(buf, "%8.4f %8.4f", pw->dpoly(1,1), pw->dpoly(1,2));
  txtXY.append(buf);
  sprintf(buf, "%8.4f %8.4f", pw->xdavg, pw->ydavg);
  txtDPos.append(buf);
}


/*
  For a given pair of atoms, find all atoms that lie along the line represented by these atoms (in
  the unrelaxed configuration). Calculate the displacements of these atoms in the direction of the
  Burgers vector and FIND THE POSITION ALONG THIS CHAIN AT WHICH THE DISPLACEMENT IS EQUAL TO THE
  MEAN, i.e. the position of the inflection point. This is the estimated position of the dislocation
  in this chain (returned as dpos). The other coordinate of the dislocation position cannot be
  determined at this point but is calculated in the procedure CalcDPosPeierlsNabarro above.

  This is called after each even atom selected.
 */

bool findDPosPeierlsNabarro(PltWin *pw, DVector &dpos, int &nachain, IVector &achain, DVector &ubchain,
			    double &ubmin, double &ubmax)
{
  DMatrix mpos;
  DVector v(2), w(2), unitb(3), pos;
  IVector idx, adum;
  FILE *f;
  double b, bhalf, ub0, ubmid, ub, dub1, dub2, d, mag, dub;
  int a1, a2, i, a, ii;
  bool ok;

  b = sqrt(pow(pw->Bvect(1),2) + pow(pw->Bvect(2),2) + pow(pw->Bvect(3),2));
  bhalf = b/2.0;
  unitb(1) = pw->Bvect(1)/b;
  unitb(2) = pw->Bvect(2)/b;
  unitb(3) = pw->Bvect(3)/b;

  // find the atoms along the line defined by the atoms picked
  a1 = pw->apicked(1);
  a2 = pw->apicked(2);
  getatomchain(pw, a1, a2, nachain, achain);
  mpos.Allocate(nachain,1); 
  pos.Allocate(nachain); 
  idx.Allocate(nachain);
  adum.Allocate(nachain);
  v(1) = pw->xyzInit(a2,1) - pw->xyzInit(a1,1);
  v(2) = pw->xyzInit(a2,2) - pw->xyzInit(a1,2);
  mag = sqrt(pow(v(1),2)+pow(v(2),2));
  v /= mag;
  for (i=1; i<=nachain; i++) {
    a = achain(i);
    w(1) = pw->xyzInit(a,1) - pw->xyzInit(a1,1);
    w(2) = pw->xyzInit(a,2) - pw->xyzInit(a1,2);
    mpos(i,1) = w(1)*v(1) + w(2)*v(2);  // position of the atom along the chain
  }
  sort(mpos, 1, nachain, 1, idx);
  for (i=1; i<=nachain; i++) {
    ii = idx(i);
    adum(i) = achain(ii);
    pos(i) = mpos(ii,1);
  }
  achain = adum;
  mpos.Free();
  idx.Free();
  adum.Free();

  // get the relative displacements of atoms in the chain in the direction of the Burgers vector
  for (i=1; i<=nachain; i++) {
    a = achain(i);
    ub = (pw->xyzRel(a,1)-pw->xyzInit(a,1))*unitb(1) +
      (pw->xyzRel(a,2)-pw->xyzInit(a,2))*unitb(2) + 
      (pw->xyzRel(a,3)-pw->xyzInit(a,3))*unitb(3);
    if (i == 1) ub0 = ub;
    ub -= ub0;
    if (ub > bhalf) ub -= b;
    if (ub <= -bhalf) ub += b;
    if (i > 1) {
      dub = ub - ubchain(i-1);
      if (fabs(dub) > bhalf) ub -= b*ub/fabs(ub);
    }
    ubchain(i) = ub;  // this is in (-b/2; b/2>
  }

  // get the minimum and maximum ub displacement along the chain
  ubmin = INF;
  ubmax = -INF;
  for (i=1; i<=nachain; i++) {
    ub = ubchain(i);
    if (ub < ubmin) ubmin = ub;
    if (ub > ubmax) ubmax = ub;
  }

  // save the profile ub(pos) for debugging
  f = fopen("ub_pos.dat", "a+");
  for (i=1; i<=nachain; i++) {
    fprintf(f, "%0.4f %0.4f\n", pos(i), ubchain(i));
  }
  fprintf(f, "\n");
  fclose(f);

  // For the chain of atoms, find the two atoms whose displacements are closest to 1/2 of the minimum
  // plus the maximum displacement of atoms in this chain. The position of the dislocation
  // is then obtained by linear interpolation.
  ubmid = (ubmin + ubmax)/2.0;
  ok = false;
  dpos = INF;
  for (i=1; i<nachain; i++) {
    dub1 = ubchain(i) - ubmid;
    dub2 = ubchain(i+1) - ubmid;
    if (dub1*dub2 < 0) {
      d = (ubmid-ubchain(i))/(ubchain(i+1)-ubchain(i));  // fractional coordinate <0,1>
      a1 = achain(i);
      a2 = achain(i+1);
      dpos(1) = pw->xyzInit(a1,1) + d*(pw->xyzInit(a2,1)-pw->xyzInit(a1,1));
      dpos(2) = pw->xyzInit(a1,2) + d*(pw->xyzInit(a2,2)-pw->xyzInit(a1,2));
      ok = true;
      break;
    }
  }
  pos.Free();
  return(ok);
}
