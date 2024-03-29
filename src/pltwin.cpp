#include <assert.h>
#include <math.h>
#include <QPainter>
#include <stdlib.h>

#include <QMouseEvent>

#include "appwin.h"
#include "calcs.h"
#include "colormap.h"
#include "cutpoly.h"
#include "bvect.h"
#include "darray.h"
#include "dpos_peierlsnabarro.h"
#include "messages.h"
#include "pltwin.h"
#include "ddplot.h"
#include "matinv.h"
#include "misc.h"
#include "neighlist.h"


PltWin::PltWin(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f)
{
  int d,i,j;

  xBorder = yBorder = 30;
  xOffset = yOffset = 0;
  arrAngle = 60.0;
  ZFact = ZOOM_FACT;
  xPan = yPan = 0;
  isSelected = true;
  DisloCenter = SHOW_DCENTER;
  PlaneTraces = PTRACES;
  PTAngle = PTRACES_ANGLE;

  tail = NULL;
  TailSize = 0;

  arrNeighNum = ARR_NEIGHNUM;
  arrNeighbors.Allocate(5);

  for (i=1; i<=arrNeighNum; i++)
    arrNeighbors(i) = ARR_NEIGHBORS[i-1];

  ProjVector.Allocate(3);
  for (d=1; d<=3; d++) 
    ProjVector(d) = PROJ_VECTOR[d-1];

  blSize.Allocate(3);
  xyzMin.Allocate(3);  xyzMax.Allocate(3);
  period.Allocate(3);
  xyzCen.Allocate(3);
  cellsize.Allocate(3);
  ncell.Allocate(3);

  zLayerSel.Allocate(60);  zLayerSel.EnlargeStep(5);
  zCoordLayer.Allocate(60);  zCoordLayer.EnlargeStep(5);
  zColorLayer.Allocate(60,2); zColorLayer.EnlargeStep(5,0);
  zLineThickLayer.Allocate(60);  zLineThickLayer.EnlargeStep(5);
  zDiamLayer.Allocate(60);  zDiamLayer.EnlargeStep(5);
  RdfNum.Allocate(5);  RdfNum.EnlargeStep(5);
  RdfDist.Allocate(5,1);  RdfDist.EnlargeStep(5,0);
  
  csys.Allocate(3,3);
  for (i=1; i<=3; i++) 
    for (j=1; j<=3; j++)
      csys(i,j) = CSYS[i-1][j-1];

  DScaleFact = DSCALE_FACT;
  shortestArrow = ARROW_SHORTEST;     // shortest arrow to be plotted
  thicknessArrow = ARROW_THICKNESS;
  plotType = PLOT_TYPE;
  DispComponent = DISP_COMPONENT;
  AtomPos = ATOMPOS;
  AtomDispScale = ATOMDISP_SCALE;
  PrintMagDisp = PRINT_MAG_DISP;
  AtomNumbers = ATOM_NUMBERS;

  NInert = 0;
  InertAtoms = false;
  isGBfile = false;
  showGB = SHOWGB;
  showZLayers = SHOW_ZLAYERS;
  showCSys = SHOW_CSYS;
  showNeighCells = SHOW_NEIGHCELLS;

  zTolerance = ZTOLERANCE;

  corrEdgeCompBCC = CORR_EDGECOMP_BCC;

  xCore = yCore = 0.0;

  rcut = 0;

  feps = NULL;
  paintEPS = false;

  Bvect.Allocate(3);
  
  napicked = 0;
  apicked = 0;
  clickAtoms = false;
  doAfterMouseClicks = DO_NOTHING;
  ndpoly = 0;
  nchain = 0;
  ndpath = 0;

  setGeometry(0, 23, parent->width(), parent->height()-23);

  connect(this, SIGNAL(signalDPosPeierlsNabarro_interm()), 
	  parentWidget()->parentWidget(), SLOT(calcDPosPeierlsNabarro_interm()));
  connect(this, SIGNAL(signalDPosPeierlsNabarro_end()), 
	  parentWidget()->parentWidget(), SLOT(calcDPosPeierlsNabarro_end()));
  connect(this, SIGNAL(signalDPosDecimate_end()), 
	  parentWidget()->parentWidget(), SLOT(calcDPosDecimate_end()));
  connect(this, SIGNAL(signalDefineBurgersCircuit_end()), 
	  parentWidget()->parentWidget(), SLOT(defineBurgersCircuit_end()));
  connect(this, SIGNAL(signalDefineCalcAtomDispl_end()), 
	  parentWidget()->parentWidget(), SLOT(defineCalcAtomDispl_end()));
}


PltWin::~PltWin()
{
  if (tail!=NULL) {
    free(tail);
    tail = NULL;
  }
}


PltWin &PltWin::operator=(const PltWin &p)
{
  FName = p.FName;
  zColorLayer = p.zColorLayer;

  NRel = p.NRel;
  NInit = p.NInit;
  NInert = p.NInert;

  xyzRel = p.xyzRel;
  xyzInit = p.xyzInit;
  xyzInert = p.xyzInert;
  aDisp = p.aDisp;
  atomType = p.atomType;

  csys = p.csys;
  blSize = p.blSize;
  xyzMin = p.xyzMin;
  xyzMax = p.xyzMax;
  period = p.period;
  latpar = p.latpar;

  TailSize = p.TailSize;
  if (tail!=NULL) free(tail); 
  if (p.TailSize>0) {
    tail = (char*) malloc(p.TailSize);
    *tail = *p.tail;
  }

  zCoordLayer = p.zCoordLayer;
  zLayer = p.zLayer;
  NumZLayers = p.NumZLayers;
  zLineThickLayer = p.zLineThickLayer;
  zDiamLayer = p.zDiamLayer;
  zLayerSel = p.zLayerSel;
  showZLayers = p.showZLayers;
  zTolerance = p.zTolerance;

  Bvect = p.Bvect;
  xCore = p.xCore;
  yCore = p.yCore;
  showNyeTensor = p.showNyeTensor;
  knownNyeTensor = p.knownNyeTensor;

  gbYcoor = p.gbYcoor;
  gbLayer = p.gbLayer;
  isGBfile = p.isGBfile;
  showGB = p.showGB;

  ProjVector = p.ProjVector;
  arrAngle = p.arrAngle; 
  ZFact = p.ZFact;
  xPan = p.xPan;
  yPan = p.yPan;
  DScaleFact = p.DScaleFact;
  AtomDispScale = p.AtomDispScale;
  factor = p.factor;
  atomScreen = p.atomScreen;
  xBorder = p.xBorder;
  yBorder = p.yBorder;
  xOffset = p.xOffset;
  yOffset = p.yOffset;
  xyzCen = p.xyzCen;
  aorder = p.aorder;
  shortestArrow = p.shortestArrow;
  thicknessArrow = p.thicknessArrow;
  plotType = p.plotType;
  DispComponent = p.DispComponent;
  AtomPos = p.AtomPos;
  PTAngle = p.PTAngle;
  PrintMagDisp = p.PrintMagDisp;
  AtomNumbers = p.AtomNumbers;
  isSelected = p.isSelected;
  DisloCenter = p.DisloCenter;
  PlaneTraces = p.PlaneTraces;
  InertAtoms = p.InertAtoms;
  showCSys = p.showCSys;
  corrEdgeCompBCC = p.corrEdgeCompBCC;
  antialiase = p.antialiase;

  if (p.feps != NULL) {
    feps = new FILE();
    *feps = *p.feps;
  }
  paintEPS = p.paintEPS;

  RdfDist = p.RdfDist;
  arrNeighNum = p.arrNeighNum;
  arrNeighbors = p.arrNeighbors;
  RdfNum = p.RdfNum;
  RdfNNeigh = p.RdfNNeigh;

  rcut = p.rcut;
  cellsize = p.cellsize;
  ncell = p.ncell;
  list = p.list;
  head = p.head;
  NeighListInit = p.NeighListInit;
  NeighListRel = p.NeighListRel;
  numNeighInit = p.numNeighInit;
  numNeighRel = p.numNeighRel;
  scaleArr = p.scaleArr;
  showNeighCells = p.showNeighCells;

  sphere = p.sphere;

  return *this;
}


void PltWin::mousePressEvent(QMouseEvent * event)
{
  DVector dpos(2), ubat;
  IVector at;
  double mindsq, dsq;
  int x, y, n, apick;

  if (event->button() == Qt::LeftButton && clickAtoms) {
    if (NInit == 0) return;
    x = event->x();
    y = event->y();
    apick = 0;
    mindsq = 1e20;
    for (n=1; n<=NInit; n++) {
      if (!zLayerSel(zLayer(n))) continue;
      dsq = pow(atomScreen(n,1)-x,2) + pow(atomScreen(n,2)-y,2);
      if (dsq < mindsq) {
	apick = n;
	mindsq = dsq;
      }
    }
    napicked++;
    apicked(napicked) = apick;
    repaint();

    if (napicked==2 && doAfterMouseClicks == CALC_DPOS_PEIERLSNABARRO)
      emit signalDPosPeierlsNabarro_interm();
  }

  if (event->button() == Qt::RightButton && clickAtoms) {
    if (napicked > 0) {
      apicked(napicked) = 0;
      napicked--;
      repaint();
    }
  }
}


void PltWin::mouseDoubleClickEvent( QMouseEvent * event )
{
  if (event->button() == Qt::LeftButton && clickAtoms) {
    switch(doAfterMouseClicks) {
      case CALC_BURG_VECTOR:
	if (napicked > 2)
	  emit signalDefineBurgersCircuit_end();
	break;
	
      case CALC_DPOS_DECIMATE:
        if (napicked >= 2)
          emit signalDPosDecimate_end();
        break;

      case CALC_DPOS_PEIERLSNABARRO:
	if (nchain >= 4) 
	  emit signalDPosPeierlsNabarro_end();
	break;
	
      case CALC_ATOM_DISPL:
	if (napicked >= 2)
	  emit signalDefineCalcAtomDispl_end();
	break;
    }

    clickAtoms = false;  
  }
}


void PltWin::paintEvent(QPaintEvent *)
{
  QPainter *p = new QPainter(this);
  drawIt(p);
  p->end();
  free(p);
}


void PltWin::resizeEvent(QResizeEvent *)
{
  SetGeometry();
}


void PltWin::drawIt(QPainter *p)
{
  QString txt;
  double x, y, x0, y0, x1, y1, x2, y2, xprev, yprev, shift;
  int n, layer, lay, prev_layer, circDia, i, j, at, ichain, a, xs, ys;
  int nmax;
  bool dum;

  QPen *pen = new QPen();
  QBrush *brush = new QBrush();
  QBrush *brush_unrld = new QBrush();

  // antialiasing of primitives if ANTIALIASE is set to true
  if (!paintEPS) p->setRenderHint(QPainter::Antialiasing, ANTIALIASE);

  // plot the grain boundary line
  if (isGBfile && showGB) {
    x0 = ZFact*xyzMin(1) - xPan - (ZFact-1)*xyzCen(1);
    x1 = ZFact*xyzMax(1) - xPan - (ZFact-1)*xyzCen(1);
    y  = ZFact*gbYcoor - yPan - (ZFact-1)*xyzCen(2);
    DrawLine(p, x0, y, x1, y);
  }

  if (AtomPos==COMPOSITE) {
    if (!paintEPS) {
      brush_unrld->setColor( Qt::lightGray );
      brush_unrld->setStyle( Qt::SolidPattern );
    }
  }

  // plotting of atomic arrangement in the initial configuration
  prev_layer = -1;
  x0 = y0 = x1 = y1 = 0;

  // need to construct the neighbor list in the relaxed configuration
  if (plotType == PLOT_ATOM_NEIGHBORS) {    
    if (AtomPos == UNRELAXED && NeighListInit.data == NULL)
      InitNeighborList(this, NeighListInit, numNeighInit);
    
    if ((AtomPos == RELAXED || AtomPos == COMPOSITE) && NeighListRel.data == NULL)
      RelNeighborList(this, rcut, NeighListRel, numNeighRel);

    if (AtomPos == UNRELAXED) {
      nmax = -1;
      for (at=1; at<=NInit; at++) {
	n = numNeighInit(at);
	if (n > nmax) nmax = n;
      }
      colormap(nmax+1, cmap);
    }

    if (AtomPos == RELAXED) {
      nmax = -1;
      for (at=1; at<=NRel; at++) {
	n = numNeighRel(at);
	if (n > nmax) nmax = n;
      }
      colormap(nmax+1, cmap);
    }
  }

  if (plotType == PLOT_ATOM_TYPES) {
    nmax = -1;
    for (at=1; at<=NInit; at++) {
      n = atomType(at);
      if (n > nmax) nmax = n;
    }
    colormap(nmax, cmap);
  }
  
  //
  //  Plot the atomic configuration such that the atoms in the front (in the top layer) are plotted
  //  at the end.
  //

  for (i=1; i<=NInit; i++) {
    at = aorder(i);
    if (!zLayerSel(zLayer(at)))
      continue;    

    layer = zLayer(at);
    circDia = zDiamLayer(layer);
    
    switch(plotType) {
      case PLOT_ATOM_LAYERS:
	layer = zLayer(at);

	if (!paintEPS && layer != prev_layer) {
	  pen->setColor(zColorLayer(layer, 1));
	  pen->setWidth(zLineThickLayer(layer));
	  p->setPen(*pen);
	  
	  brush->setColor(zColorLayer(layer, 2));
	  brush->setStyle(Qt::SolidPattern);
	  p->setBrush(*brush);
	}
	break;
	
      case PLOT_ATOM_TYPES:
	lay = layer;
	layer = atomType(at);
	
	if (!paintEPS && layer != prev_layer) {
	  pen->setColor(Qt::black);
	  pen->setWidth(zLineThickLayer(lay));
	  p->setPen(*pen);
	  
	  if (layer == 0) {
	    brush->setColor(Qt::lightGray);
	  } else {
	    brush->setColor(cmap[layer-1]);
	  }
	  brush->setStyle(Qt::SolidPattern);
	  p->setBrush(*brush);
	}
	break;

      case PLOT_ATOM_NEIGHBORS:
	lay = layer;
	if (AtomPos == UNRELAXED)
	  layer = numNeighInit(at);
	else if (AtomPos == RELAXED || AtomPos == COMPOSITE)	  
	  layer = numNeighRel(at);

	if (!paintEPS && layer != prev_layer) {
	  pen->setColor(Qt::black);
	  pen->setWidth(zLineThickLayer(lay));
	  p->setPen(*pen);
	  
	  if (layer == 0)
	    brush->setColor(Qt::lightGray);
	  else {
	    brush->setColor(QColor(cmap[layer]));
	  }
	  brush->setStyle(Qt::SolidPattern);
	  p->setBrush(*brush);
	}
	break;
    }
    
    if (layer != prev_layer) prev_layer = layer;

    switch(AtomPos) {
      case UNRELAXED:
        x = ZFact*xyzInit(at,1) - xPan - (ZFact-1)*xyzCen(1);
	y = ZFact*xyzInit(at,2) - yPan - (ZFact-1)*xyzCen(2);
	DrawAtom(p, at, x, y, circDia);	
        break;

      case RELAXED:
	x = ZFact*(xyzInit(at,1) + AtomDispScale*aDisp(at,1)) - xPan - (ZFact-1)*xyzCen(1);
        y = ZFact*(xyzInit(at,2) + AtomDispScale*aDisp(at,2)) - yPan - (ZFact-1)*xyzCen(2);
        DrawAtom(p, at, x, y, circDia);
	break;

      case COMPOSITE:
	if (!paintEPS) { p->setBrush( *brush_unrld ); }
	x = ZFact*xyzInit(at,1) - xPan - (ZFact-1)*xyzCen(1);
	y = ZFact*xyzInit(at,2) - yPan - (ZFact-1)*xyzCen(2);
        DrawAtom(p, -1, x, y, circDia);
        if (!paintEPS) { p->setBrush( *brush ); }
	x = ZFact*(xyzInit(at,1) + AtomDispScale*aDisp(at,1)) - xPan - (ZFact-1)*xyzCen(1);
	y = ZFact*(xyzInit(at,2) + AtomDispScale*aDisp(at,2)) - yPan - (ZFact-1)*xyzCen(2); 
        DrawAtom(p, at, x, y, circDia);
	break;
    }

    if (x < x0) x0 = x;
    if (x > x1) x1 = x;
    if (y < y0) y0 = y;
    if (y > y1) y1 = y;

    // atom numbers
    if (AtomNumbers) {
      txt = QString::asprintf("%d", at);
      shift = circDia/(2.0*factor);
      DrawText(p, x+shift, y+shift, txt);
    }
  }

  // plot the inert atoms
  if (InertAtoms) {
    if (!paintEPS) {
      pen->setColor(Qt::black);
      pen->setWidth(zLineThickLayer(layer));
      p->setPen(*pen);
      
      brush->setColor(Qt::black);
      brush->setStyle(Qt::NoBrush);
      p->setBrush( *brush );
    }
            
    for (at=1; at<=NInert; at++) {
      // plot only those atoms which belong to the selected (active) layers
      //      if (!zLayerSel(zLayer(n)))
      //	continue;
      x = ZFact*xyzInert(at,1) - xPan - (ZFact-1)*xyzCen(1);
      y = ZFact*xyzInert(at,2) - yPan - (ZFact-1)*xyzCen(2);
      DrawAtom(p, 0, x, y, circDia);
    }    
  }

  // highlight the atoms picked
  if (!paintEPS && napicked > 0) {
    pen->setColor(Qt::green);
    pen->setWidth(2);
    p->setPen(*pen);    
    brush->setStyle(Qt::NoBrush);
    p->setBrush(*brush);
    
    dum = ATOM_3DSPHERE;
    ATOM_3DSPHERE = false;
    for (n=1; n<=napicked; n++) {
      at = apicked(n);
      x = ZFact*xyzInit(at,1) - xPan - (ZFact-1)*xyzCen(1);
      y = ZFact*xyzInit(at,2) - yPan - (ZFact-1)*xyzCen(2);
      DrawAtom(p, at, x, y, circDia+4);
      if (n > 1) DrawLine(p, xprev, yprev, x, y);
      xprev = x;
      yprev = y;
    }
    ATOM_3DSPHERE = dum;
  }

  // highlight the atoms in selected chains
  if (!paintEPS && nchain > 0) {
    pen->setColor(Qt::green);
    pen->setWidth(2);
    p->setPen(*pen);

    for (ichain=1; ichain<=nchain; ichain++) {
      for (i=1; i<=nachain(ichain); i++) {
	a = achain(ichain,i);
	x = ZFact*xyzInit(a,1) - xPan - (ZFact-1)*xyzCen(1);
	y = ZFact*xyzInit(a,2) - yPan - (ZFact-1)*xyzCen(2);	
	if (i>1) DrawLine(p, x0, y0, x, y);       
	x0 = x;
	y0 = y;

      }
    }

    if (!paintEPS && nchain >= 2) {
      QPen *pen2 = new QPen();
      pen2->setColor(Qt::green);
      pen2->setStyle(Qt::DashLine);
      p->setPen(*pen2);

      for (i=1; i<nchain; i+=2) {
	x1 = ZFact*dposchain(i,1) - xPan - (ZFact-1)*xyzCen(1);
	y1 = ZFact*dposchain(i,2) - yPan - (ZFact-1)*xyzCen(2);
	x2 = ZFact*dposchain(i+1,1) - xPan - (ZFact-1)*xyzCen(1);
	y2 = ZFact*dposchain(i+1,2) - yPan - (ZFact-1)*xyzCen(2);
	DrawLine(p, x1, y1, x2, y2);
      }
    }
  }

  // plotting the coordinate system centered at the initial position of
  // the dislocation line
  if (PlaneTraces) DrawPlaneTraces(p, x0, y0, x1, y1);

  // plotting of arrows
  if (arrNeighNum > 0) {
    if (!paintEPS) {
      pen->setColor(Qt::black);
      pen->setWidth(thicknessArrow);
      p->setPen(*pen); 
      p->setBrush(Qt::black);
    }
    
    switch(DispComponent) {
      case EDGE:
	plotEdgeComponent(p);
	break;
	
      case SCREW:
	plotScrewComponent(p);
	break;
      
      case PROJ:
	plotScrewComponent(p);   // plotted the same way as screw components
	break;
      
      case DIFF_EDGE:
      case DIFF_SCREW:
	plotDifference(p);
	break;
    }
  }
  
  // position of dislocation center
  if (DisloCenter) {
    DrawLine(p, xCore, y0, xCore, y1);    
    DrawLine(p, x0, yCore, x1, yCore);    
  }

  // small inset to show the active Z-layers
  if (!paintEPS && showZLayers)
    ShowActiveZLayers(p);

  // show color map
  if (!paintEPS && (plotType == PLOT_ATOM_TYPES || plotType == PLOT_ATOM_NEIGHBORS))
    ShowColorMap(p);

  // lines showing the division of the block into cells (for the linked neighbor list)
  if (showNeighCells) {
    if (!paintEPS) {
      pen->setColor(Qt::black);
      p->setPen(*pen);
    }

    for (i=0; i<=ncell(1); i++) {
      x = ZFact*(xyzMin(1)+i*cellsize(1)) - xPan - (ZFact-1)*xyzCen(1);
      y0 = ZFact*xyzMin(2) - yPan - (ZFact-1)*xyzCen(2);
      y1 = ZFact*xyzMax(2) - yPan - (ZFact-1)*xyzCen(2);
      DrawLine(p, x, y0, x, y1);    
      
      for (j=0; j<=ncell(2); j++) {
	y = ZFact*(xyzMin(2)+j*cellsize(2)) - yPan - (ZFact-1)*xyzCen(2);
	x0 = ZFact*xyzMin(1) - xPan - (ZFact-1)*xyzCen(1);
	x1 = ZFact*xyzMax(1) - xPan - (ZFact-1)*xyzCen(1);
	DrawLine(p, x0, y, x1, y);
      }
    }
  }

  // coordinate system of the block
  if (!paintEPS && showCSys)
    ShowCSys(p);

  // show the polygon that encompasses the dislocation center
  if (!paintEPS && ndpoly > 0) {
    pen->setColor(Qt::green);
    p->setPen(*pen);

    for (n=1; n<=ndpoly; n++) {
      x1 = ZFact*dpoly(n,1) - xPan - (ZFact-1)*xyzCen(1);
      y1 = ZFact*dpoly(n,2) - yPan - (ZFact-1)*xyzCen(2);
      if (n<ndpoly) {
	x2 = ZFact*dpoly(n+1,1) - xPan - (ZFact-1)*xyzCen(1);
	y2 = ZFact*dpoly(n+1,2) - yPan - (ZFact-1)*xyzCen(2);
      } else {
	x2 = ZFact*dpoly(1,1) - xPan - (ZFact-1)*xyzCen(1);
	y2 = ZFact*dpoly(1,2) - yPan - (ZFact-1)*xyzCen(2);
      }
      DrawLine(p, x1, y1, x2, y2);
    }
  }

  if (!paintEPS && ndpath > 0) {
    pen->setColor(Qt::green);
    p->setPen(*pen);
    brush->setColor(Qt::green);
    brush->setStyle(Qt::NoBrush);
    p->setBrush(*brush);

    for (n=1; n<ndpath; n++) {
      x1 = ZFact*dpath(n,1) - xPan - (ZFact-1)*xyzCen(1);
      y1 = ZFact*dpath(n,2) - yPan - (ZFact-1)*xyzCen(2);
      x2 = ZFact*dpath(n+1,1) - xPan - (ZFact-1)*xyzCen(1);
      y2 = ZFact*dpath(n+1,2) - yPan - (ZFact-1)*xyzCen(2);
      DrawLine(p, x1, y1, x2, y2);
      if (n==1) {
	xyWorldToScreen(x1, y1, xs, ys);
	xs -= 2;
	ys -= 2;
	p->drawEllipse(xs, ys, 4, 4);
      }
      xyWorldToScreen(x2, y2, xs, ys);
      xs -= 2;
      ys -= 2;
      p->drawEllipse(xs, ys, 4, 4);
    }
  }
}


void PltWin::printIt(QPrinter *printer)
{
  QPainter *prn = new QPainter(printer);
  drawIt(prn);
  prn->end();
  free(prn);
}

