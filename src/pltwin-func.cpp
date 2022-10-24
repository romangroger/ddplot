#include <math.h>
#include <qlineedit.h>
#include <qpainter.h>
#include <stdlib.h>

#include "ui_dnewcutoff.h"

#include "calcs.h"
#include "messages.h"
#include "misc.h"
#include "neighlist.h"
#include "pltwin.h"


// Finds the atom in the topmost layer that is nearest to the position [x,y] on the screen
int PltWin::FindNearestAtom( int x, int y )
{
  double dist, mindist;
  int n, atom;

  atom = -1;
  for (n=1; n<=NInit; n++) {
    dist = sqrt( pow(atomScreen(n,1)-x,2) + pow(atomScreen(n,2)-y,2) );

    if (dist<mindist || atom<0) {
      mindist = dist;
      atom = n;
    }
  }

  return atom;
}


/* Calculation of the atomic displacements */

bool PltWin::InitBlockGeometry(bool toggle_inert)
{ 
  QString stat;
  IVector isorted;
  DMatrix dum;
  double zlast, dmin, dmax, disp, zcoord;
  int i, n, d, layer, fgidx, bgidx, thidx, diaidx;
  bool ok;

  xyzMin = INF;
  xyzMax = -INF;

  // atom #1 is considered above
  for (n=1; n<=NInit; n++) {
    for (d=1; d<=3; d++) {
      if (xyzInit(n,d) < xyzMin(d))  xyzMin(d) = xyzInit(n,d);
      if (xyzInit(n,d) > xyzMax(d))  xyzMax(d) = xyzInit(n,d);
    }
  }

  if (InertAtoms) {
    for (n=1; n<=NInert; n++) {
      for (d=1; d<=3; d++) {
	if (xyzInert(n,d) < xyzMin(d))  xyzMin(d) = xyzInert(n,d);
	if (xyzInert(n,d) > xyzMax(d))  xyzMax(d) = xyzInert(n,d);
      }
    }
  }

  for (d=1; d<=3; d++) {
    blSize(d) = xyzMax(d) - xyzMin(d);
    xyzCen(d) = (int) trunc((xyzMin(d)+xyzMax(d))/2.0);
  }

  if (VERBOSE) {
    printf("* properties of the loaded block:\n");
    printf("    - number of atoms in the perfect lattice = %d\n", NInit);
    printf("    - number of atoms in the relaxed configuration = %d\n", NRel);
    printf("    - number of inert atoms = %d\n", NInert);
    printf("    - position of the dislocation core = [ %0.4lf, %0.4lf ]\n", xCore, yCore);

    printf("* geometry of the block of atoms (unrelaxed coordinates):\n");
    printf("    xmin = %8.4lf,  xmax = %8.4lf,  xsize = %8.4lf\n", 
	   xyzMin(1), xyzMax(1), blSize(1));
    printf("    ymin = %8.4lf,  ymax = %8.4lf,  ysize = %8.4lf\n", 
	   xyzMin(2), xyzMax(2), blSize(2));
    printf("    zmin = %8.4lf,  zmax = %8.4lf,  zsize = %8.4lf\n", 
	   xyzMin(3), xyzMax(3), blSize(3));    
  }

  // only inert atoms were changed - no need to do the stuff below
  if (toggle_inert) return true;

  // create a vector of indices that determines the order in which the atoms will be plotted (layer
  // 1 first, last layer last) so that the atoms in +zmax are visible
  aorder.Allocate(NInit);
  sort(xyzInit, 1, NInit, 3, aorder);  // aorder are indices into a sorted array xyzInit

  // layers and their appearance
  layer = 0;
  zlast = -INF;
  zLayer.Allocate(NInit);

  for (n=1; n<=NInit; n++) {
    zcoord = xyzInit(aorder(n), 3);
    if (zcoord > zlast+zTolerance) {
      layer++;     
      zlast = zcoord;
      zCoordLayer(layer) = zlast;

      // calculation of the color, thickness and diameter
      fgidx = CalcIndex(layer-1, NUM_FGCOLORS);
      bgidx = CalcIndex(layer-1, NUM_BGCOLORS);
      thidx = CalcIndex(layer-1, NUM_THICKNESS);
      diaidx = CalcIndex(layer-1, NUM_DIAM);

      zColorLayer(layer,1) = ATOM_FGCOLOR[fgidx];
      zColorLayer(layer,2) = ATOM_BGCOLOR[bgidx];
      zLineThickLayer(layer) = ATOM_THICKNESS[thidx];
      zDiamLayer(layer) = ATOM_DIAM[diaidx];
    }
    zLayer(aorder(n)) = layer;
  } 
  NumZLayers = layer;

  // prepare colored spheres that correspond to those of Z layers
  PrepareZColoredSpheres();

  // all layers are selected by default -> i.e. atoms in all layers are considered
  // for plotting arrows
  zLayerSel.Allocate(NumZLayers);
  zLayerSel = 1;  

  if (VERBOSE) {
    printf("* list of layers, their positions along z, visibility, line thickness, diameter and colors:\n");
    printf("  (differential displacements will be calculated only between atoms in visible layers)\n");

    for (i=1; i<=NumZLayers; i++) {
      if (zLayerSel(i))
	stat = "visible";
      else
	stat = "hidden";

      printf("    %3d  %8.4lf  %s  %d  %d  %s  %s\n", i, zCoordLayer(i), stat.toLatin1().data(), 
	     zLineThickLayer(i), zDiamLayer(i), 
	     zColorLayer(i,1).name().toLatin1().data(), zColorLayer(i,2).name().toLatin1().data());
    }
  }

  // calculation of the displacement vectors
  dmin = INF;
  dmax = -INF;
  
  aDisp.Allocate(NRel,3);
  
  for (n=1; n<=NRel; n++) {
    for (d=1; d<=3; d++)
      aDisp(n,d) = xyzRel(n,d) - xyzInit(n,d);    
    
    if (VERBOSE) {
      disp = sqrt( pow(aDisp(n,1),2) + pow(aDisp(n,2),2) + pow(aDisp(n,3),2) );
      if (disp<dmin)  dmin = disp;
      if (disp>dmax)  dmax = disp;
    }
  }
  
  if (VERBOSE) {
    printf("* calculation of displacements induced by the applied stress:\n"); 
    printf("    dmin = %8.4lf,  dmax = %8.4lf\n", dmin, dmax);
  }

  if (DispComponent != NONE) 
    ok = InitBlockANeighbors();
  else 
    ok = true;

  return ok;
}


/* Generate a linked neighbor list between atoms in the unrelaxed configuration that will be used to
  plot arrows between atoms. This may take a long time. If an error occurs when calling
  InitNeighborList, it means that an atom has too many neighbors that they cannot be stored in the
  allocated array -> invoke the dialog for the user to input a smaller cut-off*/

bool PltWin::InitBlockANeighbors()
{
  double rtry, rsave;
  bool ok;

  // construct the linked neighbor list
  ok = InitNeighborList(this, NeighListInit, numNeighInit);
  if (ok)
    return true;
  
  // if the error occurred, allow the user to input NeighRCut manually for this plot
  if (!isGBfile) 
    rtry = vlength(period);
  else
    rtry = blSize(1)/10.0;  // this is only a guess - can be changed as we like
  
  Ui::DNewCutOff ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);
  
  do {
    ui.rCurrent->setText( QString::number(rcut, 'g', 4));
    ui.rTry->setText( QString::number(rtry, 'g', 4));
    dialog->exec();
    
    if (dialog->result()==QDialog::Accepted) {
      rcut = ui.rNew->text().toDouble();
      rsave = NEIGHRCUT;
      NEIGHRCUT = -1;  // use the given rcut
      
      ok = InitNeighborList(this, NeighListInit, numNeighInit);
      
      NEIGHRCUT = rsave;
    }
  } while (dialog->result()==QDialog::Accepted && !ok);

  return true;
}
