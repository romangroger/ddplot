#include <math.h>

#include "ddplot.h"
#include "messages.h"
#include "neighlist.h"
#include "pltwin.h"
#include "darray.h"

DMatrix rdf_dist;  // radial distribution function (used in qsort)
IVector rdf_num;


/* 
  Calculate the radial distribution function - i.e. find the number of atoms which have a certain
  distance from a particular central atom. At the same time, find the atomic separations (ranges
  in the case of materials such as HEA) which correspond to the first, second, third nearest 
  neighbors.
*/

bool CalcRDF(PltWin *pw)
{
  double dx, dy, dz, dist2, rcut2, dist, dd;
  DMatrix rdf_mat, tmp, rtmp(6,1);
  DVector rbound(6);
  IVector nnear, rnum(6);
  IVector icell(3), icmin(3), icmax(3), idx;
  int i, j, d, icen, ineigh, iidx, jidx, kidx, ntake;
  bool written, disorder, take;
 
  if (VERBOSE) {
    printf("* calculating the radial distribution function:\n");
    printf("    cut-off radius for atomic neighbors:  %0.4lf\n", pw->rcut);
  }

  if (pw->RdfDist.len[1] == 2) {  // we have slight disorder
    pw->RdfNum.Free();
    pw->RdfDist.Free();
    pw->RdfNum.Allocate(5);  pw->RdfNum.EnlargeStep(5);
    pw->RdfDist.Allocate(5,1);  pw->RdfDist.EnlargeStep(5,0);
  }

  pw->RdfNNeigh = 0;
  pw->RdfNum = 0;
  rcut2 = pow(pw->rcut, 2);

  disorder = false;
  for (icen=1; icen<=pw->NInit; icen++) {
    // consider only atoms in the selected Z-layers
    if (!pw->zLayerSel(pw->zLayer(icen)))
      continue;

    // get the indices of the cell in which atom icen resides
    GetCell(pw, icen, icell);

    // we will search also the neighboring cells - get the indices
    for (d=1; d<=3; d++) {
      icmin(d) = icell(d) - 1;
      icmax(d) = icell(d) + 1;
      if (d < 3) {
	if (icmin(d) == 0) icmin(d) = 1;
	if (icmax(d) == pw->ncell(d)+1) icmax(d) = pw->ncell(d);
      }
    }

    // search through all neighboring cells and itself
    for (kidx=icmin(3); kidx<=icmax(3); kidx++) {
      for (jidx=icmin(2); jidx<=icmax(2); jidx++) {
	for (iidx=icmin(1); iidx<=icmax(1); iidx++) {
	  // get the head atom (one of possible neighbors of icen) for this cell
	  if (kidx == 0)
	    ineigh =  pw->head(iidx,jidx,pw->ncell(3));
	  else if (kidx == pw->ncell(3)+1)
	    ineigh =  pw->head(iidx,jidx,1);
	  else
	    ineigh = pw->head(iidx,jidx,kidx);

	  while (ineigh > 0) {
	    // skip itself
	    if (ineigh < icen || (ineigh == icen && kidx != 0 && kidx != pw->ncell(3)+1)) {
	      ineigh = pw->list(ineigh);
	      continue;
	    }

	    // consider only the neighboring atoms located in selected Z-layers
	    if (!pw->zLayerSel(pw->zLayer(ineigh))) {
	      ineigh = pw->list(ineigh);
	      continue;
	    }
	    
	    dx = pw->xyzInit(ineigh,1) - pw->xyzInit(icen,1);
	    if (fabs(dx) > pw->rcut) {
	      ineigh = pw->list(ineigh);
	      continue;
	    }
	    
	    dy = pw->xyzInit(ineigh,2) - pw->xyzInit(icen,2); 
	    if (fabs(dy) > pw->rcut) {
	      ineigh = pw->list(ineigh);
	      continue;
	    }

	    if (kidx == 0)
	      dz = pw->xyzInit(ineigh,3) - pw->period(3) - pw->xyzInit(icen,3);
	    else if (kidx == pw->ncell(3)+1)
	      dz = pw->xyzInit(ineigh,3) + pw->period(3) - pw->xyzInit(icen,3);
	    else
	      dz = pw->xyzInit(ineigh,3) - pw->xyzInit(icen,3); 
	    if (fabs(dz) > pw->rcut) {
	      ineigh = pw->list(ineigh);
	      continue;
	    }
	    
	    // calculate the distance between icen and ineigh
	    dist2 = pow(dx,2) + pow(dy,2) + pow(dz,2);
	    
	    if (dist2 <= rcut2) {
	      written = false;
	      dist = sqrt(dist2);      		
	      if (pw->RdfNNeigh > 0) {
		for (i=1; i<=pw->RdfNNeigh; i++) {
		  dd = fabs(dist - pw->RdfDist(i));
		  if (dd <= 1e-4) {
		    pw->RdfNum(i)++;
		    pw->RdfDist(i) = dist;
		    written = true;
		    break;
		  } else if (dd <= RTOLDISORDER)  {
		    disorder = true;
		  }
		}
	      }
	      	      
	      // a completely new interatomic distance
	      if (!written) {
		pw->RdfNNeigh++;
		pw->RdfNum(pw->RdfNNeigh) = 1;
		pw->RdfDist(pw->RdfNNeigh) = dist;
		if (VERBOSE)
		  printf("%d | icen=%d ineigh=%d | r=%0.3lf\n",
			 pw->RdfNNeigh, icen, ineigh, pw->RdfDist(pw->RdfNNeigh));
	      }
	    }

	    // take next atom in the same cell (-1 of no other exists)
	    ineigh = pw->list(ineigh);
	  };
	}
      }
    }
  }

  // no neighbors
  if (pw->RdfNNeigh==0)
    return true;
 
  // sort according to RdfDist in the ascending order -> after
  // sorting the qsort returns indices <idx> such that the array
  // RdfDist[idx[:]] is sorted
  rdf_dist.Allocate(pw->RdfNNeigh, 1);
  rdf_dist = pw->RdfDist;
  rdf_num.Allocate(pw->RdfNNeigh);
  rdf_num = pw->RdfNum;

  // we need a matrix for sorting below...
  rdf_mat.Allocate(pw->RdfNNeigh, 1);
  memcpy(rdf_mat.data, rdf_dist.data, pw->RdfNNeigh*sizeof(double));

  // get the indexes into the sorted array
  idx.Allocate(pw->RdfNNeigh);
  sort(rdf_mat, 1, pw->RdfNNeigh, 1, idx);

  // save the sorted arrays
  for (i=1; i<=pw->RdfNNeigh; i++) {
    pw->RdfDist(i) = rdf_dist(idx(i));
    pw->RdfNum(i) = rdf_num(idx(i));
  }
  rdf_dist.Free();
  rdf_num.Free();
  rdf_mat.Free();
 
  // if there are no well-defined distances for first, second, third, etc. neighbors (such as in HEAs), find the
  // ranges of atomic separations for these neighbors
  if (disorder) {
    //    printf("slight disorder found => binning separations of neighboring atoms\n");
    nnear.Allocate(pw->RdfNNeigh);
    nnear = 0;
    for (i=1; i<=pw->RdfNNeigh; i++) {      
      for (j=i+1; j<=pw->RdfNNeigh; j++) {
	dist = fabs(pw->RdfDist(i) - pw->RdfDist(j));	
	if (dist <= RTOLDISORDER) {
	  nnear(i) += 1;
	  nnear(j) += 1;
	}
      }
    }

    // find the minima in nnear and identify the boundaries between 1st, 2nd, ... nearest neighbors
    tmp.Allocate(pw->RdfNNeigh,1);
    for (i=1; i<=pw->RdfNNeigh; i++)
      tmp(i,1) = nnear(i);
    sort(tmp, 1, pw->RdfNNeigh, 1, idx);
    ntake = 0;
    for (i=1; i<=pw->RdfNNeigh; i++) {
      take = (idx(i) == 1 || idx(i) == pw->RdfNNeigh);
      if (!take && nnear(idx(i)-1) > nnear(idx(i)) && nnear(idx(i)+1) > nnear(idx(i)))  // minimum
	take = true;
      if (take) {
	ntake++;
	rbound(ntake) = pw->RdfDist(idx(i));
	if (ntake == 6)  // we need only five nearest neighbors, i.e. 6 bounding regions
	  break;
      }
    }
    tmp.Free();
    nnear.Free();

    // sort rbound
    idx.Free();
    idx.Allocate(6);
    rtmp.Allocate(6,1);
    memcpy(rtmp.data, rbound.data, 6*sizeof(double));    
    sort(rtmp, 1, 6, 1, idx);
    for (i=1; i<=6; i++) {
      rbound(i) = rtmp(idx(i));
    }
    rtmp.Free();
    
    // keep the ranges of separations for the 1st, 2nd, ... nearest neighbors
    rnum = 0;
    for (i=1; i<=pw->RdfNNeigh; i++) {
      if (pw->RdfDist(i) >= rbound(1) && pw->RdfDist(i) <= rbound(2)) rnum(1)++;
      if (pw->RdfDist(i) >= rbound(2) && pw->RdfDist(i) <= rbound(3)) rnum(2)++;
      if (pw->RdfDist(i) >= rbound(3) && pw->RdfDist(i) <= rbound(4)) rnum(3)++;
      if (pw->RdfDist(i) >= rbound(4) && pw->RdfDist(i) <= rbound(5)) rnum(4)++;
      if (pw->RdfDist(i) >= rbound(5) && pw->RdfDist(i) <= rbound(6)) rnum(5)++;
    }

    pw->RdfDist.Free();
    pw->RdfNum.Free();
    
    pw->RdfNNeigh = 5;
    
    pw->RdfDist.Allocate(5,2);
    pw->RdfNum.Allocate(5);    
    for (i=1; i<=5; i++) {
      pw->RdfDist(i,1) = rbound(i);
      pw->RdfDist(i,2) = rbound(i+1);
      pw->RdfNum(i) = rnum(i);      
    }
  }

  idx.Free();

  return true;
}


/*
  Calculate the radial distribution function for the structure pointed to by pw and for the range of
  atomic separations given in the arguments.
*/

void RDF(PltWin *pw, int which, double rmin, double rmax, int nbins, QVector<double> &rdfrad, 
	 QVector<double> &rdfg, double &gmax)
{
  QVector<int> rdfnum(nbins,0);
  double rx, ry, rz, r, rsq, rminsq, rmaxsq, rlen, dr, c, val;
  int icen, ineigh, ibin;

  rminsq = pow(rmin,2);
  rmaxsq = pow(rmax,2);
  rlen = rmax-rmin;
  dr = rlen/nbins;

  rdfrad.clear();
  rdfg.clear();
  rdfrad.push_back(rmin + dr/2.0);
  for (ibin=1; ibin<nbins; ibin++) 
    rdfrad.push_back(rdfrad[ibin-1] + dr);

  for (icen=1; icen<=pw->NInit; icen++) {
    if (!pw->zLayerSel(pw->zLayer(icen)))
      continue;
    
    for (ineigh=icen+1; ineigh<=pw->NInit; ineigh++) {
      if (!pw->zLayerSel(pw->zLayer(ineigh))) 
	continue;
      
      if (which == UNRELAXED) {
	rx = pw->xyzInit(ineigh,1) - pw->xyzInit(icen,1);
	ry = pw->xyzInit(ineigh,2) - pw->xyzInit(icen,2); 
	rz = pw->xyzInit(ineigh,3) - pw->xyzInit(icen,3); 	    
      } else {
	rx = pw->xyzRel(ineigh,1) - pw->xyzRel(icen,1);
	ry = pw->xyzRel(ineigh,2) - pw->xyzRel(icen,2); 
	rz = pw->xyzRel(ineigh,3) - pw->xyzRel(icen,3); 	    
      }
      
      rsq = pow(rx,2) + pow(ry,2) + pow(rz,2);
      if (rsq >= rminsq && rsq <= rmaxsq) {
	r = sqrt(rsq);
	ibin = floor((r-rmin)/dr);
	rdfnum[ibin]++;	
      }
    }
  }

  c = 4.0*M_PI*dr;
  gmax = 0.0;  
  for (ibin=0; ibin<nbins; ibin++) {
    rsq = pow(rdfrad[ibin],2);
    val = rdfnum[ibin]/(c*rsq);
    rdfg.push_back(val);   // we calculate g(r)*rho here
    if (val>gmax) gmax = val;
  }
}
