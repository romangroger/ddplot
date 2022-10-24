#include "cutpoly.h"

/*
  This function finds the intersection segment of a line with a two-dimensional polygon. See the
  explanation of this algorithm at
  http://softsurfer.com/Archive/algorithm_0111/algorithm_0111.htm#intersect2D_SegPoly(). It returns
  true if the polygon is crossed exactly twice by the line segment P1-P2.x. The polygon in V is defined
  by a set of points iV that go around the polygon in the counterclockwise direction !
*/

bool LineCutPoly2D(int iV, DMatrix V, DMatrix P, DVector &tV, DVector &tP, DMatrix &Q, 
		   int &iV1, DMatrix &V1, int &iV2, DMatrix &V2, IVector &idx1, IVector &idx2)
{
  const double NaN=1e10, tol_zero=1e-6, tol_coincide=1e-4;

  DVector nV(2), eV(2), eP(2), Qdum(2), eV1(2), eV2(2), e1(2), e2(2), n2(2);
  IVector ix(2);
  double num, denom, tPdum, tPenter, tPleave, dot, tV1, tV2;
  int i, nxfound, i1, i2;
  bool xfound, skip, swap12=false;

  iV1 = iV2 = 0;

  // find the intersection of the cutting line with the actual polygon which contains in its
  // interior the position of the dislocation
  tPenter = -NaN;
  tPleave = NaN;
  eP(1) = P(2,1) - P(1,1);
  eP(2) = P(2,2) - P(1,2);
  for (i=1; i<iV; i++) {
    eV(1) = V(i+1,1) - V(i,1);       // direction vector along the segment of the polygon
    eV(2) = V(i+1,2) - V(i,2);
    nV(1) = eV(2);                   // normal vector of the segment of the counterclockwise polygon
    nV(2) = -eV(1);  
    num = (V(i,1)-P(1,1))*nV(1) + (V(i,2)-P(1,2))*nV(2);
    denom = eP(1)*nV(1) + eP(2)*nV(2);
    if (fabs(denom)<tol_zero) {  // the cutting line is parallel to the edge direction of this segment -> no intersection
      // checking if P1-P2 coincides with Vi-Vi+1 - this is important to avoid having a subdomain that is a line
      e1(1) = V(i,1)-P(1,1);  e1(2) = V(i,2)-P(1,2);
      e2(1) = V(i,1)-P(2,1);  e2(2) = V(i,2)-P(2,2);
      n2(1) = e2(2); n2(2) = -e2(1);
      dot = e1(1)*n2(1) + e1(2)*n2(2);
      if (fabs(dot)<tol_coincide) {  // vector Vi-P1 is parallel to Vi-P2 -> P1-P2 coincides with Vi-Vi+1 -> no bisection
	return(false);
      } else {
	tV(i) = NaN;
	continue;
      }
    }

    // parameter of the cutting line defining the intersection point
    tPdum = num/denom;                  
    if (tPdum<0 || tPdum>1) {        // the intersection point is outside of the line segment P(1,:)-P(2,:)
      tV(i) = NaN;
      continue;      
    }
    if (denom > 0 && tPdum < tPleave) tPleave = tPdum;
    if (denom < 0 && tPdum > tPenter) tPenter = tPdum;

    // parameter of the edge of the polygon defining the intersection with the cutting line
    Qdum(1) = P(1,1) + tPdum*eP(1);        // coordinates of the two points on the line that intersects the polygon
    Qdum(2) = P(1,2) + tPdum*eP(2);
    if (fabs(V(i+1,1)-V(i,1)) < tol_zero)            
      tV(i) = (Qdum(2)-V(i,2))/(V(i+1,2)-V(i,2));  
    else if (fabs(V(i+1,2)-V(i,2)) < tol_zero) 
      tV(i) = (Qdum(1)-V(i,1))/(V(i+1,1)-V(i,1));  
    else {
      tV1 = (Qdum(1)-V(i,1))/(V(i+1,1)-V(i,1));  
      tV2 = (Qdum(2)-V(i,2))/(V(i+1,2)-V(i,2));  
      if (fabs(tV1-tV2) > tol_zero) {
	printf("!!! the parameters tV are different when calculated from x and y components !!!\n");
	return(false);
      }
      tV(i) = tV1;
    }
  }

  tP(1) = tPenter;
  tP(2) = tPleave;

  // check that (1) tP(1) and tP(2) are between 0 and 1, i.e. the intersection is between the points
  // P1 and P2, and (2) that exactly two values of tV are between 0 and 1, i.e. there are exactly
  // two intersections with the polygon
  xfound = (tP(2)-tP(1)>tol_zero && tP(1)>=0 && tP(2)<=1);  // without tol, there could be only one intersection point 
  if (xfound) {
    nxfound = 0;
    for (i=1; i<iV; i++) {
      xfound = (tV(i)>=0 && tV(i)<1);   // this avoids double counting of the nodes of the polygon
      if (xfound) {
	nxfound++;
	if (nxfound < 3) ix(nxfound) = i;
      }
    }
    xfound = (nxfound==2);
  } 
  if (!xfound) return(false);

  //----------------------------------------------------------------------------------------------------
  // We have now exactly two intersection points with the polygon
  //----------------------------------------------------------------------------------------------------

  // Q(1,:)-Q(2,:) is the line segment that cuts the polygon in half
  for (i=1; i<=2; i++) {
    Q(1,i) = P(1,i) + tP(1)*eP(i);
    Q(2,i) = P(1,i) + tP(2)*eP(i);
  }

  // cut the polygon into two subpolygons that are defined by the coordinates of vertices 
  // stored in V1 and V2
  skip = false;
  idx1 = 0;
  idx2 = 0;
  for (i=1; i<iV; i++) {
    if (!skip) {              // start building the subpolygon V1
      iV1++;
      V1(iV1,1) = V(i,1);
      V1(iV1,2) = V(i,2);
      idx1(iV1) = i;
      if (i==ix(1) || i==ix(2)) {
	if (tV(i)>0) {
	  iV1++;
	  V1(iV1,1) = V(i,1) + tV(i)*(V(i+1,1)-V(i,1));
	  V1(iV1,2) = V(i,2) + tV(i)*(V(i+1,2)-V(i,2));
	  if (fabs(V1(iV1,1)-Q(1,1))<tol_zero && fabs(V1(iV1,2)-Q(1,2))<tol_zero) 
	    idx1(iV1) = -1;   // this intersection point is Q1
	  else if (fabs(V1(iV1,1)-Q(2,1))<tol_zero && fabs(V1(iV1,2)-Q(2,2))<tol_zero) 
	    idx1(iV1) = -2;   // this intersection point is Q2
	  else {
	    printf("Unhandled exception in subroutine cutpoly - position #1.\n");
	    return(false);
	  }
	}
	if (tV(i)==0) {
	  if (fabs(V1(iV1,1)-Q(1,1))<tol_zero && fabs(V1(iV1,2)-Q(1,2))<tol_zero) 
	    idx1(iV1) = -1;   // this intersection point is Q1
	  else if (fabs(V1(iV1,1)-Q(2,1))<tol_zero && fabs(V1(iV1,2)-Q(2,2))<tol_zero) 
	    idx1(iV1) = -2;   // this intersection point is Q2
	  else {
	    printf("Unhandled exception in subroutine cutpoly - position #4.\n");
	    return(false);
	  }
	}
	iV2++;
	V2(iV2,1) = V1(iV1,1);
	V2(iV2,2) = V1(iV1,2);
	idx2(iV2) = idx1(iV1);
	skip = !skip;         // now continue building the subpolygon V2
      }
    } else {
      iV2++;
      V2(iV2,1) = V(i,1);
      V2(iV2,2) = V(i,2);
      idx2(iV2) = i;
      if (i==ix(1) || i==ix(2)) {
	if (tV(i)>0) {
	  iV2++;
	  V2(iV2,1) = V(i,1) + tV(i)*(V(i+1,1)-V(i,1));
	  V2(iV2,2) = V(i,2) + tV(i)*(V(i+1,2)-V(i,2));
	  if (fabs(V2(iV2,1)-Q(1,1))<tol_zero && fabs(V2(iV2,2)-Q(1,2))<tol_zero) 
	    idx2(iV2) = -1;   // this intersection point is Q1
	  else if (fabs(V2(iV2,1)-Q(2,1))<tol_zero && fabs(V2(iV2,2)-Q(2,2))<tol_zero) 
	    idx2(iV2) = -2;   // this intersection point is Q2
	  else {
	    printf("Unhandled exception in subroutine cutpoly - position #2.\n");
	    return(false);
	  }
	}
	if (tV(i)==0) {
	  if (fabs(V2(iV2,1)-Q(1,1))<tol_zero && fabs(V2(iV2,2)-Q(1,2))<tol_zero) 
	    idx2(iV2) = -1;   // this intersection point is Q1
	  else if (fabs(V2(iV2,1)-Q(2,1))<tol_zero && fabs(V2(iV2,2)-Q(2,2))<tol_zero) 
	    idx2(iV2) = -2;   // this intersection point is Q2
	  else {
	    printf("Unhandled exception in subroutine cutpoly - position #5.\n");
	    return(false);
	  }
	}
	iV1++;
	V1(iV1,1) = V2(iV2,1);
	V1(iV1,2) = V2(iV2,2);
	idx1(iV1) = idx2(iV2);
	skip = !skip;         // finished building the subpolygon V2, continue with V1
      }
    }
  }

  // close the two subpolygons V1 and V2
  iV1++;
  V1(iV1,1) = V1(1,1);
  V1(iV1,2) = V1(1,2);
  idx1(iV1) = idx1(1);
  iV2++;
  V2(iV2,1) = V2(1,1);
  V2(iV2,2) = V2(1,2);
  idx2(iV2) = idx2(1);

  // The intersections of the cutting line with the polygon are assigned by negative values of idx1
  // and idx2. Here, we check whether the subpolygon V1 is that which contains the oriented line
  // P1-P2. If not, V1 will be swapped with V2 below.
  for (i=1; i<iV1; i++) {
    if (idx1(i)<0) {
      if (idx1(i+1)<0) {
	i1 = i;
	i2 = i+1;
      } else {
	i2 = i;
	i1 = i-1;
	if (i1==0) i1 = iV1-1;
	if (idx1(i1)>0) {
	  printf("Unhandled exception in subroutine cutpoly - position #3.\n");
	  return(false);
	}
      }
      swap12 = (idx1(i1)==-2 && idx1(i2)==-1);
      break;
    }
  }

  if (swap12) {
    DMatrix Vdum = V1; 
    V1 = V2; 
    V2 = Vdum; 
    Vdum.Free();

    IVector idxdum = idx1;
    idx1 = idx2;
    idx2 = idxdum;
    idxdum.Free();

    int iVdum = iV1;
    iV1 = iV2; 
    iV2 = iVdum;
  }

  return(true);
}


/*
  A polygon is a set of points in a list where the consecutive points form the boundary. To decide
  whether the polygon is convex, calculate for each consecutive pair of edges of the polygon (each
  triplet of points), the z-component of the cross product of the vectors defined by the edges
  pointing towards the points in increasing order. The polygon is convex if the z-components of the
  cross products are all either positive or negative. Otherwise the polygon is nonconvex.x 

  Source: From http://stackoverflow.com/questions/471962/how-do-determine-if-a-polygon-is-complex-convex-nonconvex:
*/

bool isconvex(int nv, DMatrix v)
{
  double dx1, dy1, dx2, dy2, cross, csave;
  int iv, iv1, iv2;

  for (iv=1; iv<nv; iv++) {  // the last vertex (iv=nv) is the same as iv=1
    iv1 = iv+1;
    if (iv1 >= nv) iv1 = iv1 - (nv-1);
    iv2 = iv+2;
    if (iv2 >= nv) iv2 = iv2 - (nv-1);
    dx1 = v(iv1,1) - v(iv,1);
    dy1 = v(iv1,2) - v(iv,2);
    dx2 = v(iv2,1) - v(iv1,1);
    dy2 = v(iv2,2) - v(iv1,2);
    cross = dx1*dy2 - dy1*dx2;
    if (iv == 1)
      csave = cross;
    else if (csave*cross < 0)
      return(false);
  }
  return(true);
}
