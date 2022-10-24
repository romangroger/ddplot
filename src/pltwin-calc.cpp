#include <math.h>

#include "messages.h"
#include "neighlist.h"
#include "pltwin.h"


/*
  Calculate the relative displacements between two neighboring atoms in the (xy) plane for further
  plotting of the edge components of the (relative) displacement field. The edge components of the
  displacement field are not scaled which is unlike the screw components that are always scaled by
  the length of the Burgers vector.

  Remember: This function must always be called prior to plotting the dislocation displacement map.
*/

void PltWin::calcEdgeRelDisp()
{
  double dzInit, s;
  int d, icen, ineigh, idx, dum;

  if (VERBOSE)
    printf("* calculating the edge-displacements scaling\n");

  for (icen=1; icen<=NInit; icen++) {
    idx = 1;

    while (NeighListInit(icen,idx)!=END_OF_LIST) {
      ineigh = NeighListInit(icen,idx);

      scaleArr(icen,idx,1) = aDisp(ineigh,1) - aDisp(icen,1);
      scaleArr(icen,idx,2) = aDisp(ineigh,2) - aDisp(icen,2);

      for (d=1; d<=2; d++) {
	if (Bvect(d)!=0) {
	  dum = trunc(scaleArr(icen,idx,d)/Bvect(d));
	  if (dum!=0) scaleArr(icen,idx,d) -= Bvect(d)*dum;  // max. length must be equal to the B.v.
	  if (abs(scaleArr(icen,idx,d))>Bvect(d)/2.0) {
	    s = scaleArr(icen,idx,d) / abs(scaleArr(icen,idx,d));
	    scaleArr(icen,idx,d) = -s*(Bvect(d)-abs(scaleArr(icen,idx,d)));
	  }
	}
      }

      /* This is a consistency check that has to be performed to make sure that e.g. the
	 total edge component of the Burgers vector is zero. For each pair of atoms we
	 check whether the distance of these atoms in the unrelaxed configuration in the
	 z-direction is period(3)/3, i.e. sqrt(3)/6 in our bcc block. If not, we need to
	 take the periodic image of one of these atoms, i.e. we correct the distance
	 between these atoms in the z-direction by -period(3). */

      if (corrEdgeCompBCC) {
	dzInit = xyzInit(ineigh,3) - xyzInit(icen,3);
	if (fabs(dzInit) > period(3)/3.0+0.01)              // this is specific to bcc
	  dzInit = dzInit - dzInit/fabs(dzInit)*period(3);
	
	if (dzInit < 0) {  // > 0 only reverses the sense of all arrows
	  scaleArr(icen,idx,1) = -scaleArr(icen,idx,1);
	  scaleArr(icen,idx,2) = -scaleArr(icen,idx,2);
	}
      }

      /* end of the correction step */

      idx++;
    }
  }
}


/*
  Calculate the relative displacements between two neighboring atoms in the z direction that
  corresponds to the screw displacements. All screw displacements are expressed in the unit of the
  magnitude of the Burgers vector, i.e.  scaleArr(:,:,3) are between -1 and +1 and can be used
  directly to scale the atom-to-atom distance to obtain the vector of an arrow.

  This is usually done as:  rdist*scaleArr(:,:,3) * DScaleFact (scaling of displacements)
                            ---------------------
           when scaleArr(:,:,3) is +/-1, the arrow is from atom to atom

  Remember: This function must always be called prior to plotting the dislocation displacement map.
*/

void PltWin::calcScrewRelDisp()
{
  double rdisp, ratio, s;
  int ineigh, icen, idx;

  if (VERBOSE)
    printf("* calculating the screw-displacements scaling\n");

  for (icen=1; icen<=NInit; icen++) {
    idx = 1;

    while (NeighListInit(icen,idx)!=END_OF_LIST) {
      ineigh = NeighListInit(icen,idx);

      rdisp = aDisp(ineigh,3) - aDisp(icen,3);   // relative displacement of the two atoms
      ratio = rdisp/Bvect(3);

      if (fabs(ratio)>1.0)                       // treatment if ratio>1
        ratio -= trunc(ratio);

      if (fabs(ratio)>0.5) {                     // correction to have ratio between +/-0.5
	s = ratio/fabs(ratio);
	ratio = -s*(1.0 - fabs(ratio));
      }

      // save the scaling factors for all pairs of atoms - the same structure as NeighListInit
      scaleArr(icen,idx,3) = 2.0*ratio;  // max. size of the arrow should be the full period

      idx++;
    }
  }
}


/*
  Calculate the relative displacements between two neighboring atoms in the direction given by the
  projection vector ProjVect. All projected displacements are expressed in the unit of the
  projection of the the Burgers vector along the same direction, i.e.  scaleArr(:,:,3) are between
  -1 and +1 and can be used directly to scale the atom-to-atom distance to obtain the vector of an
  arrow. These displacements are plotted from atom to atom as the screw displacements.

  Remember: This function must always be called prior to plotting the dislocation displacement
  map.  
*/

bool PltWin::calcProjRelDisp()
{
  DVector eproj(3), r(3);
  double rdisp, ratio, s, len, Bproj;
  int ineigh, icen, idx, d;

  if (VERBOSE)
    printf("* calculating the displacements scaling in the given direction\n");

  // project the Burgers vector in the direction ProjVector
  len = sqrt( pow(ProjVector(1),2)+pow(ProjVector(2),2)+pow(ProjVector(3),2) );
  eproj = ProjVector/len;
  Bproj = 0;
  for (d=1; d<=3; d++)
    Bproj += Bvect(d)*eproj(d);

  if (fabs(Bproj)<1e-6) {
    msgError(err_ProjVectNotValid);
    return false;
  }

  for (icen=1; icen<=NInit; icen++) {
    idx = 1;

    while (NeighListInit(icen,idx)!=END_OF_LIST) {
      ineigh = NeighListInit(icen,idx);

      for (d=1; d<=3; d++)
	r(d) = aDisp(ineigh,d) - aDisp(icen,d);   // relative displacement of the two atoms

      // project the relative displacements into the direction of the unit vector eproj (calculated above)
      rdisp = 0;
      for (d=1; d<=3; d++)
	rdisp += r(d)*eproj(d);  
      ratio = rdisp/Bproj;

      while (fabs(ratio)>1.0)                    // treatment if ratio>1
        ratio -= trunc(ratio);

      if (fabs(ratio)>0.5) {                     // correction to have ratio between +/-0.5
	s = ratio/fabs(ratio);
	ratio = -s*(1.0 - fabs(ratio));
      }

      // save the scaling factors for all pairs of atoms - the same structure as NeighListInit
      scaleArr(icen,idx,3) = 2.0*ratio;  // max. size of the arrow should be the full period

      idx++;
    }
  }
  return true;
}


/*
  Calculate the Nye tensor at every atom in the block and calculate the interpolations of the tensor 
  in the interstitial regions between the atoms. For details, see 
  [Hartley and Mishin, Mat. Sci. Eng. A400-401:18-21 (2005)].
*/

void PltWin::calcNyeTensor()
{
  DVector PInit(MAX_NEIGHBORS,3), QRel(MAX_NEIGHBORS,3);
  double dx, dy, dz, dist, rmax;
  int icen, ineigh, idx, nnP, nnQ;

  // calculate the neighbor list in the relaxed structure
  rmax = 0.5*(RdfDist(1,1)+RdfDist(2,1));  // consider "up to 1.5th" nearest neighbors
  RelNeighborList(this, rmax, NeighListRel, numNeighRel);

  // MAKE BOTH NEIGHBOR LISTS BIDIRECTIONAL, i.e. i<-j and i->j


  for (icen=1; icen<=NInit; icen++) {    
    // get nearest neighbors of atom icen in the unrelaxed structure
    nnP = 0;
    idx = 1;
    rmax = RdfDist(1,1);  // consider only the nearest neighbors

    while (NeighListInit(icen,idx)!=END_OF_LIST) {
      ineigh = NeighListInit(icen,idx);

      dx = xyzInit(ineigh,1) - xyzInit(icen,1);
      dy = xyzInit(ineigh,2) - xyzInit(icen,2); 
      dz = xyzInit(ineigh,3) - xyzInit(icen,3); 
      dist = sqrt(pow(dx,2) + pow(dy,2) + pow(dz,2));

      if (abs(dist-rmax)<1e-6) {  
	nnP++;
	PInit(nnP,1) = dx;
	PInit(nnP,2) = dy;
	PInit(nnP,3) = dz;
      }
      idx++;
    }

    // get all above-calculated neighbors of atom icen in the unrelaxed structure
    nnQ = 0;

    while (NeighListRel(icen,nnQ+1)!=END_OF_LIST) {
      nnQ++;
      ineigh = NeighListRel(icen,nnQ);
      QRel(nnQ,1) = xyzRel(ineigh,1) - xyzRel(icen,1);
      QRel(nnQ,2) = xyzRel(ineigh,2) - xyzRel(icen,2);
      QRel(nnQ,3) = xyzRel(ineigh,3) - xyzRel(icen,3);
    }

    // construct the lattice correspondence tensor for atom icen by means of the mean-squares
    // optimization (the system is overdetermined)
    
  }

  knownNyeTensor = true;
}


/*
  Calculates the difference between relative displacements plotted in two generally different
  widgets (p1 and p2). The difference is stored in array scaleArr that has the same structure as
  NeighListInit. The third index of scaleArr gives the x,y,z relative displacements between two
  neighbors whose indexes are given by the two indexes of scaleArr. Be careful - x and y are in the
  same units as the positions of atoms, while z is between +1 and -1 and is later used to scale the
  atom-to-atom vector to get the size of an arrow corresponding to a screw displacement. 

  The variable comp is either EDGE or SCREW depending on the component we want to compare.
*/

bool PltWin::CompareDisp(PltWin *p2, int comp)
{
  PltWin *p1 = this;
  int idx, icen, ineigh1, ineigh2;
  bool ok;

  // check if the neighbors lists of both plots are identical - this assures that we can compare
  // blocks for two different materials having the same crystal structure, only different
  // lattice parameter
  ok = (p1->NInit==p2->NInit);
  if (!ok)  return ok;

  for (icen=1; icen<=p1->NInit; icen++) {
    idx = 1;    

    while (p1->NeighListInit(icen,idx)!=END_OF_LIST && p2->NeighListInit(icen,idx)!=END_OF_LIST) {
      ineigh1 = p1->NeighListInit(icen,idx);  
      ineigh2 = p2->NeighListInit(icen,idx);

      ok = (ineigh1==ineigh2);
      if (!ok)  return false;  // different neighbors in the two lists      

      idx++;
    }

    // now, we must be at the end of both lists simultaneously
    if (p1->NeighListInit(icen,idx)!=END_OF_LIST || p2->NeighListInit(icen,idx)!=END_OF_LIST)
      return false;
  }


  // calculate the difference in relative displacements between the two atoms
  switch(comp) {
    case EDGE:
      p1->calcEdgeRelDisp();
      p2->calcEdgeRelDisp();

      for (icen=1; icen<=p1->NInit; icen++) {
	idx = 1;    	
	while (p1->NeighListInit(icen,idx)!=END_OF_LIST) {
	  scaleArr(icen,idx,1) = p1->scaleArr(icen,idx,1) - p2->scaleArr(icen,idx,1);
	  scaleArr(icen,idx,2) = p1->scaleArr(icen,idx,2) - p2->scaleArr(icen,idx,2);
	  idx++;
	}
      }

      break;

    // subtract abs values otherwise it would distinguish between the direction of arrow and
    // some symbols would be (-) if both arrows point in the negative direction
    case SCREW:
      p1->calcScrewRelDisp();
      p2->calcScrewRelDisp();

      for (icen=1; icen<=p1->NInit; icen++) {
	idx = 1;    	
	while (p1->NeighListInit(icen,idx)!=END_OF_LIST) {
	  scaleArr(icen,idx,3) = fabs(p1->scaleArr(icen,idx,3)) - fabs(p2->scaleArr(icen,idx,3));
	  idx++;
	}
      }
  }

  return true;  // ok, the two sets are compatible if we got here
}
