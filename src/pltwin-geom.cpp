#include <math.h>
#include <qpainter.h>
#include <stdlib.h>
#include <QGraphicsColorizeEffect>

#include "colormap.h"
#include "ddplot.h"
#include "pltwin.h"


/*
  Plotting the EDGE COMPONENT of the displacements. The arrows correspond to the relative
  displacements of the two connected atoms in the plane of the figure. Their direction
  is the actual direction of the relative displacement on going from the perfect lattice
  into the distorted structure.
*/

void PltWin::plotEdgeComponent(QPainter *p)
{
  QString txt;
  DVector rdisp(3);
  DVector xy_from(2), xy_to(2);
  float ang;
  double xfrom, yfrom, xto, yto, xw, yw;
  double cen, len;
  int d, ineigh, icen, idx;
  bool plotted;

  if (!paintEPS) p->setFont(QFont("helvetica", 11));

  for (icen=1; icen<=NInit; icen++) {
    // plot only those atoms which belong to the active layers
    if (!zLayerSel(zLayer(icen)))
      continue;

    idx = 1;    

    while (NeighListInit(icen,idx)!=END_OF_LIST) {
      ineigh = NeighListInit(icen,idx);

      // plot only those atoms which belong to the active layers
      if (!zLayerSel(zLayer(ineigh))) {
	idx++;
	continue;
      }

      rdisp(1) = scaleArr(icen,idx,1)*DScaleFact;
      rdisp(2) = scaleArr(icen,idx,2)*DScaleFact;

      // calculation of the length of the arrow between icen and ineigh
      for (d=1; d<=2; d++) {
	if (AtomPos==UNRELAXED)
          cen = (xyzInit(icen,d)+xyzInit(ineigh,d))/2.0;         // center of the arrow
        else
          cen = (xyzRel(icen,d)+xyzRel(ineigh,d))/2.0;           // center of the arrow

        xy_from(d) = cen - rdisp(d)/2.0;
        xy_to(d) = cen + rdisp(d)/2.0;
      }
      //--------------------------------------------------------------------

      // plotting of the arrows
      xfrom = ZFact*xy_from(1) - xPan - (ZFact-1)*xyzCen(1);
      yfrom = ZFact*xy_from(2) - yPan - (ZFact-1)*xyzCen(2);
      xto = ZFact*xy_to(1) - xPan - (ZFact-1)*xyzCen(1);
      yto = ZFact*xy_to(2) - yPan - (ZFact-1)*xyzCen(2);

      if (xfrom==xto && yfrom==yto) 
	plotted = false;
      else
	plotted = DrawArrow(p, xfrom, yfrom, xto, yto);

      // annotations
      if (PrintMagDisp && plotted) {
	len = sqrt( pow(rdisp(1)/DScaleFact,2) + pow(rdisp(2)/DScaleFact,2) );
	txt = QString::asprintf("a/%0.2lf", 1.0/fabs(len));
	xw = (xfrom+xto)/2.0;
	yw = (yfrom+yto)/2.0;
	ang = atan2(yto-yfrom, xto-xfrom);
	if (fabs(ang)>M_PI/2.0) ang += M_PI;
	DrawText(p, xw, yw, txt, Qt::AlignHCenter, ang);       
      }

      idx++;
    }
  }
}


/*
  Plotting the arrows for the SCREW COMPONENT of relative displacements between atoms.
  Each arrow corresponds to the relative displacements of the two connected atoms in 
  the direction perpendicular to the plane of the screen, relative to the distance of
  the two atoms in the perfect lattice. If an arrow is longer than the Burgers vector,
  only its modulo is taken. If this is larger than 1/2 of the Burgers vector, the
  sense of the arrow is reversed and the length is proportional to the difference
  between the original size of the arrow and b/2.
*/

void PltWin::plotScrewComponent(QPainter *p)
{
  QString txt;
  DVector rdisp(3), xyz0(3), xyz1(3), rvect(3);
  DVector xyz_from(3), xyz_to(3), rscreen(3);
  float ang;
  double xfrom, yfrom, xto, yto, cen, xw, yw;
  int d, ineigh, icen, idx;
  bool plotted;

  if (!paintEPS) p->setFont(QFont("helvetica", 11));

  for (icen=1; icen<=NInit; icen++) {
    // plot only those atoms which belong to the active layers
    if (!zLayerSel(zLayer(icen)))
      continue;
    
    idx = 1;    

    while (NeighListInit(icen,idx)!=END_OF_LIST) {
      ineigh = NeighListInit(icen,idx);

      // plot only those atoms which belong to the active layers
      if (!zLayerSel(zLayer(ineigh))) {
	idx++;
	continue;
      }

      // calculation of the length of an arrow between icen and ineigh
      if (AtomPos==UNRELAXED) {
        for (d=1; d<=3; d++) {
          xyz0(d) = xyzInit(icen,d);
          xyz1(d) = xyzInit(ineigh,d); 
        }
      } else {
        for (d=1; d<=3; d++) {
          xyz0(d) = xyzRel(icen,d);
          xyz1(d) = xyzRel(ineigh,d);
        }
      }

      for (d=1; d<=3; d++) {
        rvect(d) = xyz1(d) - xyz0(d);
	rscreen(d) = scaleArr(icen,idx,3)*DScaleFact*rvect(d);   
      }

      // adjustment of arrows - they must lie between two atoms
      for (d=1; d<=3; d++) {
        cen = (xyz0(d)+xyz1(d))/2.0;
        xyz_from(d) = cen - rscreen(d)/2.0;
        xyz_to(d) = cen + rscreen(d)/2.0;
      }
      //----------------------------------------------

      // plotting of the arrows
      xfrom = ZFact*xyz_from(1) - xPan - (ZFact-1)*xyzCen(1);
      yfrom = ZFact*xyz_from(2) - yPan - (ZFact-1)*xyzCen(2);
      xto = ZFact*xyz_to(1) - xPan - (ZFact-1)*xyzCen(1);
      yto = ZFact*xyz_to(2) - yPan - (ZFact-1)*xyzCen(2);

      if (xfrom==xto && yfrom==yto)
	plotted = false;
      else
      	plotted = DrawArrow(p, xfrom, yfrom, xto, yto);

      // annotations
      if (PrintMagDisp && plotted) {
        txt = QString::asprintf("a/%0.2lf", 4.0/(fabs(scaleArr(icen,idx,3))));
	xw = (xfrom+xto)/2.0;
	yw = (yfrom+yto)/2.0;
	ang = atan2(yto-yfrom, xto-xfrom);
	if (fabs(ang)>M_PI/2.0) ang += M_PI;
	DrawText(p, xw, yw, txt, Qt::AlignHCenter, ang);
      }
 
      idx++;
    }
  }
}
      

/*
  Plotting the DIFFERENCE between the displacements in two plots. Prior to calling this function,
  it is necessary to call PltWin::compareDisp to calculate the difference between the displacement
  of an atom in the two selected plots. This is then stored for each individual atom in aDisp.
  Note, that in this case aDisp does not keep the displacements of atoms due to applied stress but
  instead the difference between these displacements in two different plots.
*/

void PltWin::plotDifference(QPainter *p)
{
  QString txt;
  DVector xycen(2), xy_from(2), xy_to(2);
  double rdist, xfrom, yfrom, xto, yto;
  int icen, ineigh, idx, d, symsize;

  if (!paintEPS) p->setFont(QFont("helvetica", 11));

  for (icen=1; icen<=NInit; icen++) {
    // plot only those atoms which belong to the active layers
    if (!zLayerSel(zLayer(icen)))
      continue;

    idx = 1;    

    while (NeighListInit(icen,idx)!=END_OF_LIST) {
      ineigh = NeighListInit(icen,idx);

      // plot only those atoms which belong to the active layers
      if (!zLayerSel(zLayer(ineigh))) {
	idx++;
	continue;
      }

      // we are plotting in the (xy) plane - don't take the z-coordinate for scaling
      rdist = sqrt(pow(xyzInit(icen,1)-xyzInit(ineigh,1),2) +
		   pow(xyzInit(icen,2)-xyzInit(ineigh,2),2));

      // for comparison of screw components
      switch(DispComponent) {
        case DIFF_SCREW:
	  symsize = trunc(scaleArr(icen,idx,3)*rdist * factor*DScaleFact*ZFact);
	  if (symsize==0) 
	    break;

	  if (!paintEPS) p->setFont( QFont("helvetica", abs(symsize)) );
	    
	  if (symsize>0) {
	    txt = "+";
	    p->setPen(QPen(Qt::red));
	  } else {
	    txt = '-';
	    p->setPen(QPen(Qt::blue));
	  }
	  
	  // center the sign
	  for (d=1; d<=2; d++)
	    xycen(d) = (xyzInit(icen,d) + xyzInit(ineigh,d))/2.0 - abs(symsize)/(3*ZFact*factor);
	    
	  // plotting either the plus or minus sign according to the magnitude in diff
	  DrawText(p, ZFact*xycen(1)-xPan-(ZFact-1)*xyzCen(1), 
		   ZFact*xycen(2)-yPan-(ZFact-1)*xyzCen(2), txt);
	  break;

        case DIFF_EDGE:
	  // center of the arrow
	  for (d=1; d<=2; d++) {
	    xycen(d) = (xyzInit(icen,d)+xyzInit(ineigh,d))/2.0;         
	    xy_from(d) = xycen(d) - DScaleFact*scaleArr(icen,idx,d)/2.0;
	    xy_to(d) = xycen(d) + DScaleFact*scaleArr(icen,idx,d)/2.0;
	  }

	  // plotting of the arrows
	  xfrom = ZFact*xy_from(1) - xPan - (ZFact-1)*xyzCen(1);
	  yfrom = ZFact*xy_from(2) - yPan - (ZFact-1)*xyzCen(2);
	  xto = ZFact*xy_to(1) - xPan - (ZFact-1)*xyzCen(1);
	  yto = ZFact*xy_to(2) - yPan - (ZFact-1)*xyzCen(2);
	  
	  DrawArrow(p, xfrom, yfrom, xto, yto);
      }

      idx++;
    }
  }
}


/*
  Plotting a contour plot that corresponds to the spatial variation of a given component of the
  dislocation density (Nye) tensor.
*/

void PltWin::plotNyeTensor( QPainter *p )
{

}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------


bool PltWin::DrawArrow(QPainter *p, double xw0, double yw0, double xw1, double yw1)
{
  QPolygon pts;
  const double h = 0.2*tan(arrAngle*M_PI/360.0);  
  double arrow[4][2] = { {0.0, 0.0}, {1.0, 0.0}, {0.8, h}, {0.8, -h} };
  //  const double h = 0.3*tan(arrAngle*M_PI/360.0);  // bigger arrowhead
  //  double arrow[4][2] = { {0.0, 0.0}, {1.0, 0.0}, {0.7, h}, {0.7, -h} };
  IMatrix xy(4,2);
  int i;
  double ang, xrot, yrot, size;

  size = sqrt(pow(xw1-xw0,2)+pow(yw1-yw0,2));
  if (factor*size<shortestArrow) 
    return false;

  // scaling and rotation
  ang = atan2(yw1-yw0,xw1-xw0);
  for (i=0; i<4; i++) {
    // scaling
    arrow[i][0] *= size;
    arrow[i][1] *= size;

    // rotation
    xrot = arrow[i][0]*cos(ang) - arrow[i][1]*sin(ang);
    yrot = arrow[i][0]*sin(ang) + arrow[i][1]*cos(ang);
    arrow[i][0] = xrot;
    arrow[i][1] = yrot;
  }

  for (i=1; i<=4; i++) {
    xyWorldToScreen(xw0+arrow[i-1][0], yw0+arrow[i-1][1], xy(i,1), xy(i,2));
  }
  
  if (paintEPS) {
    fprintf(feps, "newpath 0 0 0 setrgbcolor %d %d moveto %d %d lineto stroke\n", 
  	    xy(1,1), height()-xy(1,2), xy(2,1), height()-xy(2,2));
    fprintf(feps, "newpath 0 0 0 setrgbcolor %d %d moveto %d %d lineto %d %d lineto closepath fill stroke\n", 
	    xy(2,1), height()-xy(2,2), xy(3,1), height()-xy(3,2), xy(4,1), height()-xy(4,2));
  } else {
    QLineF line(xy(1,1), xy(1,2), xy(2,1), xy(2,2));
    p->drawLine(line);
    QPointF poly[3] = {
      QPointF(xy(2,1), xy(2,2)), 
      QPointF(xy(3,1), xy(3,2)), 
      QPointF(xy(4,1), xy(4,2)) };
    p->drawPolygon(poly, 3);
  }
  
  return true;
}


void PltWin::DrawAtom(QPainter *p, int atom, double xw, double yw, int dia)
{
  QColor cfill, cline;
  QPen pen;
  QBrush brush;
  int xs, ys, lw;
  int rad = int(dia/2.0);

  xyWorldToScreen(xw, yw, xs, ys);
  xs -= rad;
  ys -= rad;

  if (paintEPS) {
    (atom > 0) ? cfill = zColorLayer(zLayer(atom), 2) : cfill = Qt::lightGray;
    fprintf(feps, "newpath %0.1f %0.1f %0.1f setrgbcolor %d %d %d 0 360 arc closepath fill\n", 
    	    cfill.red()/255.0, cfill.green()/255.0, cfill.blue()/255.0, xs+rad, height()-ys-rad, rad);
    if (atom > 0) {
      cline = zColorLayer(zLayer(atom),1);
      lw = zLineThickLayer(zLayer(atom));
    } else {
      cline = Qt::black;
      lw = 1;
    }
    fprintf(feps, "newpath %0.1f %0.1f %0.1f setrgbcolor %d setlinewidth %d %d %d 0 360 arc closepath stroke\n", 
	    cline.red()/255.0, cline.green()/255.0, cline.blue()/255.0, lw, xs+rad, height()-ys-rad, rad);
  } else {
    if (ATOM_3DSPHERE) {
      QColor col;
      (atom > 0) ? col = p->brush().color() : col = Qt::lightGray;
      QRect rect(xs, ys, dia, dia);
      QRadialGradient radgrad(xs+dia/2, ys+dia/2, dia/2, xs+2*dia/3, ys+dia/3);
      radgrad.setColorAt(0, col.lighter(200));
      radgrad.setColorAt(0.6, col);
      radgrad.setColorAt(1, col.darker(200));
      pen = p->pen();
      brush = p->brush();
      p->setPen(Qt::NoPen);
      p->setBrush(radgrad);
      p->drawEllipse(rect);
      p->setPen(pen);
      p->setBrush(brush);

      // draw atoms using saved colored spheres
      /*      if (plotType == PLOT_ATOM_LAYERS) {
	(atom > 0) ? c = zLayer(atom)%NUM_BGCOLORS : c = 0;
      } else if (plotType == PLOT_ATOM_TYPES) {
	c = atomType(atom);
      }
      p->drawImage(QRect(xs, ys, dia, dia), sphere[c]);*/
    } else {
      p->drawEllipse(xs, ys, dia, dia);
    }
  }

  // if the real atomic number is given, keep its coordinates on the screen
  if (atom > 0) {
    atomScreen(atom,1) = xs + rad;
    atomScreen(atom,2) = ys + rad;
  }
}


void PltWin::DrawLine(QPainter *p, double xw0, double yw0, double xw1, double yw1)
{
  int xs0, ys0, xs1, ys1;

  xyWorldToScreen(xw0, yw0, xs0, ys0);
  xyWorldToScreen(xw1, yw1, xs1, ys1);
  if (paintEPS)
    fprintf(feps, "%d %d moveto %d %d lineto stroke\n", xs0, height()-ys0, xs1, height()-ys1);
  else
    p->drawLine(xs0, ys0, xs1, ys1);
}


void PltWin::DrawPlaneTraces(QPainter *p, double xw0, double yw0, double xw1, double yw1)
{
  int ang, circDia;
  int xs0, ys0, xs1, ys1;
  double rang;

  QPen *apen = new QPen();
  apen->setColor(Qt::darkGray);
  p->setPen(*apen);

  circDia = 10;
  xyWorldToScreen(xw0, yw0, xs0, ys0);
  xyWorldToScreen(xw1, yw1, xs1, ys1);
  p->setClipRect(xs0-2*circDia, ys1-2*circDia, xs1-xs0+4*circDia, ys0-ys1+4*circDia);

  for (ang=0; ang<360; ang+=PTAngle) {
    rang = ang*M_PI/180.0;
    xw1 = xCore+100.0*cos(rang);
    yw1 = yCore+100.0*sin(rang);
    DrawLine(p, xCore, yCore, xw1, yw1);
  }
  p->setClipping(false);
}


void PltWin::DrawText(QPainter *p, double xw, double yw, QString txt, Qt::AlignmentFlag align, float ang)
{
  int xs, ys;
  
  xyWorldToScreen(xw, yw, xs, ys);
  if (paintEPS) {
    if (ang == 0) {
      if (align == Qt::AlignLeft) 
	fprintf(feps, "newpath %d %d moveto (%s) show\n",
		xs, height()-ys, txt.toLatin1().data());
      else if (align == Qt::AlignHCenter)
	fprintf(feps, "newpath %d %d moveto (%s) dup stringwidth pop 2 div neg 0 rmoveto show\n",
		xs, height()-ys, txt.toLatin1().data());	       
    } else {
      if (align == Qt::AlignLeft)
	fprintf(feps, "gsave newpath %d %d moveto (%s) %0.1f rotate show grestore\n",
		xs, height()-ys, txt.toLatin1().data(), ang*180.0/M_PI);
      else if (align == Qt::AlignHCenter)
	fprintf(feps, "gsave newpath %d %d moveto (%s) %0.1f rotate dup stringwidth pop 2 div neg 0 rmoveto show grestore\n",
		xs, height()-ys, txt.toLatin1().data(), ang*180.0/M_PI);
    }
  } else {
    if (align == Qt::AlignLeft)
      p->drawText(xs, ys, txt);
    else {
      p->save();
      p->translate(xs, ys);
      p->rotate(-ang*180.0/M_PI);
      QRect rect(-50, 0, 100, 20);
      p->drawText(rect, align, txt);
      p->restore();
    }
  }
}


void PltWin::Pan(double xsteps, double ysteps)
{
  xPan += PAN_RATIO*xsteps*blSize(1);   
  yPan += PAN_RATIO*ysteps*blSize(2);   
  repaint();
}


void PltWin::SetGeometry()
{
  double xfact, yfact;

  xfact = (width()-2*xBorder)/blSize(1);
  yfact = (height()-2*yBorder)/blSize(2);
  factor = (xfact<yfact) ? xfact : yfact;

  xOffset = (int) floor( (width()-2*xBorder-factor*blSize(1))/2.0 );
  yOffset = (int) floor( (height()-2*yBorder-factor*blSize(2))/2.0 );
}


void PltWin::ShowActiveZLayers( QPainter *p )
{
  QString txt;
  double step, pos;
  int i, yheight;

  p->setPen( Qt::darkGray ); 
  p->setBrush( Qt::white );

  yheight = 12*(NumZLayers-1);
  if (yheight>height()-80)
    yheight = height()-80;

  p->drawRect( 10, height()-yheight-60, 130, yheight+40 );

  step = double(yheight)/(NumZLayers-1);
  for (i=1; i<=NumZLayers; i++ ) {
    pos = height()-40 - (i-1)*step;

    // active layers in green, inactive in black
    if (zLayerSel(i))
      p->setPen( Qt::green );
    else
      p->setPen( Qt::black );

    p->drawLine( 75, pos, 120, pos );

    txt = txt.setNum(zCoordLayer(i), 'f', 4);
    p->drawText( 30, pos+step/2.0, txt );
  }
}


void PltWin::ShowColorMap( QPainter *p )
{
  QString txt;
  float div;
  int x0, y0, xwidth, yheight, y, dy, dymin=20;
  int i, nbin, i1, i2;
  bool bin;

  p->setFont(QFont("helvetica", 13));
    
  xwidth = 40;
  yheight = 350;
  x0 = width() - 120;
  y0 = height()/2 - yheight/2;

  dy = yheight/cmap.count();
  y = y0;

  // need binning
  bin = (dy < dymin);
  if (bin) {
    nbin = round(yheight/dymin);
    dy = yheight/nbin;
    div = cmap.count()/nbin;
    colormap(nbin, cmap);
  }
  
  for (i=cmap.count()-1; i>=0; i--) {
    p->setPen( Qt::NoPen );
    p->setBrush( QColor(cmap[i]) );
    p->drawRect( x0, y, xwidth, dy );
    p->setPen( Qt::black );
    if (!bin)
      txt = QString::asprintf("%d", i);
    else {
      i1 = round(i*div);
      i2 = round((i+1)*div);
      txt = QString::asprintf("%d..%d", i1, i2);
    }
    p->drawText( x0+xwidth+5, y+dy/2+5, txt );
    y += dy;
  }

  if (plotType == PLOT_ATOM_NEIGHBORS)
    p->drawText( x0-50, y0-20, "Number of neighbors" );

  if (plotType == PLOT_ATOM_TYPES)
    p->drawText( x0-30, y0-20, "Atomic types" );
}


void PltWin::ShowCSys( QPainter *p )
{
  QString txt;
  int xwidth, yheight, xmin, ymin;

  xwidth = yheight = 150;
  xmin = width()-xwidth;
  ymin = height()-yheight-20;

  p->setPen( Qt::darkGray ); 
  p->setBrush( Qt::white );
  p->drawRect( xmin, ymin, xwidth, yheight );

  // outlining box
  p->setBrush( Qt::NoBrush );
  p->drawRect( xmin+60, ymin+30, xwidth-90, yheight-90 );
  p->drawRect( xmin+30, ymin+60, xwidth-90, yheight-90 );
  p->drawLine( xmin+30, ymin+60, xmin+60, ymin+30 );
  p->drawLine( xmin+30+xwidth-90, ymin+60, xmin+60+xwidth-90, ymin+30 );
  p->drawLine( xmin+30, ymin+60+yheight-90, xmin+60, ymin+30+yheight-90 );
  p->drawLine( xmin+30+xwidth-90, ymin+60+yheight-90, xmin+60+xwidth-90, ymin+30+yheight-90 );

  // red lines indicating axes
  p->setPen( QPen( Qt::red, 2 ) );
  p->drawLine( xmin+60, ymin+30, xmin+60, ymin+30+yheight-90 );
  p->drawLine( xmin+60, ymin+30+yheight-90, xmin+60+xwidth-90, ymin+30+yheight-90 );
  p->drawLine( xmin+60, ymin+30+yheight-90, xmin+30, ymin+60+yheight-90 );

  // x-label
  txt = QString::asprintf("[%d %d %d]", int(csys(1,1)), int(csys(1,2)), int(csys(1,3)));
  p->drawText( xmin+30+xwidth-90, ymin+30+yheight-90+13, txt);

  // y-label
  txt = QString::asprintf("[%d %d %d]", int(csys(2,1)), int(csys(2,2)), int(csys(2,3)));
  p->drawText( xmin+30, ymin+27, txt);

  // z-label
  txt = QString::asprintf("[%d %d %d]", int(csys(3,1)), int(csys(3,2)), int(csys(3,3)));
  p->drawText( xmin+15, ymin+60+yheight-90+13, txt);
}


void PltWin::Zoom( double zfact )
{
  if (ZFact*zfact<=500 && ZFact*zfact>=0.1) {
    ZFact *= zfact;
    repaint();
  }
}
