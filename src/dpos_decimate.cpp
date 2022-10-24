#include <QDialog>
#include <QPainter>
#include <string.h>

#include "ui_ddposburgcircuitresults.h"

#include "bvect.h"
#include "cutpoly.h"
#include "darray.h"
#include "dpospnresults.h"
#include "lagrpoly.h"
#include "misc.h"
#include "pltwin.h"


/*
  Find the polygonal domain that represents the boundary of the region in which the center of the
  dislocation resides. This search must be performed in the deformed (relaxed) configuration; if it
  is performed in the initial configuration (i.e. ideal lattice using xyzInit instead of xyzRel),
  the algorithm finds more than one position of the dislocation depending on the shape of the
  initial polygon. The reason why the search has to be performed in the deformed configuration is
  that the Burgers circuit is always drawn in the deformed state and then mapped back onto the
  undeformed state (meaning that one calculates differences in the lengths of edges of the polygon
  between the deformed and undeformed configuration).  */

void CalcDPosDecimate(PltWin *pw, bool showdlg, double &xdinit, double &ydinit, double &xdrel, double &ydrel)
{
  QPainter *p;
  QBrush *brush;
  QPen *pen;
  QString txt;
  DMatrix V0init, V0rel, Vinit, Vrel, V1init, V1rel, V2init, V2rel, V01init, V01rel, V02init, V02rel;
  DMatrix Pinit(2,2), Prel(2,2), Qinit(2,2), Qrel(2,2), Q0rel(2,2), du;
  DVector B1(3), B2(3), Bv(3), tPinit(2), tPrel(2), tP0rel(2), tVinit, tVrel, tV0init, tV0rel;
  double x1, y1, x2, y2, Blen1, Blen2, xmnrel, xmxrel, ymnrel, ymxrel, xdrel_tol, ydrel_tol;
  double xmninit, xmxinit, ymninit, ymxinit, xdinit_tol, ydinit_tol;
  IVector at1, at2, idx1init, idx1rel, idx2init, idx2rel, idx01init, idx01rel, idx02init, idx02rel, aidx;
  int i, j, k, a, iV1rel, iV2rel, iV01rel, iV02rel;
  int iV0init, iV0rel, iVinit, iVrel, i1, i2;
  bool xfound_rel, firstcut, ok;

  //if (pw->interactive)
  //  pw->setAttribute(Qt::WA_PaintOutsidePaintEvent, true);

  // get the user-defined initial polygon  **** CHECK WHY WE NEED +2 IN ALLOCATIONS ****
  aidx.Allocate(pw->NRel);

  iV0init = iVinit = pw->napicked;
  V0init.Allocate(iV0init,2);                          // coordinates of vertices of the given polygon
  Vinit.Allocate(iV0init,2);  Vinit.EnlargeStep(5,0);        // coordinates of vertices of the current polygon
  V1init.Allocate(iV0init+1,2); V1init.EnlargeStep(5,0);      // coordinates of vertices of subpolygon 1
  V2init.Allocate(iV0init+1,2); V2init.EnlargeStep(5,0);      // coordinates of vertices of subpolygon 2
  V01init.Allocate(iV0init+1,2);                         // coordinates of vertices in the Burgers circuit 1
  V02init.Allocate(iV0init+1,2);                         // coordinates of vertices in the Burgers circuit 2
  tVinit.Allocate(iV0init);   tVinit.EnlargeStep(5);
  tV0init.Allocate(iV0init);
  idx1init.Allocate(iV0init+1); idx1init.EnlargeStep(5);
  idx2init.Allocate(iV0init+1); idx2init.EnlargeStep(5);
  idx01init.Allocate(iV0init+1); idx01init.EnlargeStep(5);
  idx02init.Allocate(iV0init+1); idx02init.EnlargeStep(5);

  iV0rel = iVrel = pw->napicked;
  V0rel.Allocate(iV0rel,2);                          // coordinates of vertices of the given polygon
  Vrel.Allocate(iV0rel,2);  Vrel.EnlargeStep(5,0);        // coordinates of vertices of the current polygon
  V1rel.Allocate(iV0rel+1,2); V1rel.EnlargeStep(5,0);      // coordinates of vertices of subpolygon 1
  V2rel.Allocate(iV0rel+1,2); V2rel.EnlargeStep(5,0);      // coordinates of vertices of subpolygon 2
  V01rel.Allocate(iV0rel+1,2);                         // coordinates of vertices in the Burgers circuit 1
  V02rel.Allocate(iV0rel+1,2);                         // coordinates of vertices in the Burgers circuit 2
  tVrel.Allocate(iV0rel);   tVrel.EnlargeStep(5);
  tV0rel.Allocate(iV0rel);
  idx1rel.Allocate(iV0rel+1); idx1rel.EnlargeStep(5);
  idx2rel.Allocate(iV0rel+1); idx2rel.EnlargeStep(5);
  idx01rel.Allocate(iV0rel+1); idx01rel.EnlargeStep(5);
  idx02rel.Allocate(iV0rel+1); idx02rel.EnlargeStep(5);

  for (i=1; i<=iV0rel; i++) {
    a = pw->apicked(i);
    V0init(i,1) = pw->xyzInit(a,1);
    V0init(i,2) = pw->xyzInit(a,2);
    Vinit(i,1) = V0init(i,1);                          // vertices of the initial polygon
    Vinit(i,2) = V0init(i,2);
    V0rel(i,1) = pw->xyzRel(a,1);
    V0rel(i,2) = pw->xyzRel(a,2);
    Vrel(i,1) = V0rel(i,1);                          // vertices of the initial polygon
    Vrel(i,2) = V0rel(i,2);
  }

  // check if the initial polygons are convex
  ok = isconvex(iVrel, Vrel);
  if (!ok) {
    printf("The relaxed polygon is not convex.\n");
    exit(0);
  }

  ok = isconvex(iVinit, Vinit);
  if (!ok) {
    printf("The unrelaxed polygon is not convex.\n");
    exit(0);
  }

  // indices into the set of atoms (shuffled or not)
  if (pw->shuffleIdx)
    shuffle(pw->NRel, aidx);
  else {
    for (i=1; i<=pw->NRel; i++)
      aidx(i) = i;
  }

  // go over all pairs of atoms in the block (no double counting)
  firstcut = true;
  for (i1=1; i1<=pw->NRel; i1++) {            // pw->NRel = pw->NInit
    i = aidx(i1);
    Pinit(1,1) = pw->xyzInit(i,1);
    Pinit(1,2) = pw->xyzInit(i,2);
    Prel(1,1) = pw->xyzRel(i,1);
    Prel(1,2) = pw->xyzRel(i,2);

    //    r1 = sqrt(pow(Pinit(1,1)-0.0,2) + pow(Pinit(1,2)-0.4714,2));
    //    if (r1 > 5) continue;

    for (i2=1; i2<=pw->NRel; i2++) {
      j = aidx(i2);
      if (j<=i) continue;  // no double-counting and the same atom
      Pinit(2,1) = pw->xyzInit(j,1);
      Pinit(2,2) = pw->xyzInit(j,2);
      Prel(2,1) = pw->xyzRel(j,1);
      Prel(2,2) = pw->xyzRel(j,2);

      //      r2 = sqrt(pow(Pinit(2,1)-0.0,2) + pow(Pinit(2,2)-0.4714,2));
      //      if (r1 > 5) continue;

      // bisect the dislocation polygon in Vinit into its subpolygons, V1init and V2init
      //      xfound_init = LineCutPoly2D(iVinit, Vinit, Pinit, tVinit, tPinit, Qinit,
      //				  iV1init, V1init, iV2init, V2init, idx1init, idx2init);
      //      if (!xfound_init) continue;

      // bisect the dislocation polygon in Vrel into its subpolygons, V1rel and V2rel
      xfound_rel = LineCutPoly2D(iVrel, Vrel, Prel, tVrel, tPrel, Qrel,
                                 iV1rel, V1rel, iV2rel, V2rel, idx1rel, idx2rel);
      if (!xfound_rel) continue;
      if (firstcut) {           // firstcut=false is at the end (after we actually do some cutting)
        tV0rel = tVrel;
        tP0rel = tPrel;
        Q0rel = Qrel;
        iV01rel = iV1rel;
        iV02rel = iV2rel;
        V01rel = V1rel;
        V02rel = V2rel;
        idx01rel = idx1rel;
        idx02rel = idx2rel;
      } else {
        xfound_rel = LineCutPoly2D(iV0rel, V0rel, Prel, tV0rel, tP0rel, Q0rel,
                                   iV01rel, V01rel, iV02rel, V02rel, idx01rel, idx02rel);
        if (!xfound_rel) continue;
      }

      // for the calculation of the Burgers vector, take the atoms at P instead of the
      // intersections in Q
      at1.Allocate(iV01rel);
      du.Allocate(iV01rel,3);
      for (k=1; k<=iV01rel; k++) {
        if (idx01rel(k)>0)
          at1(k) = pw->apicked(idx01rel(k));
        else {
          if (idx01rel(k)==-1) at1(k) = i;
          if (idx01rel(k)==-2) at1(k) = j;
        }
      }
      BVectFromCircuit(pw, iV01rel, at1, B1, Blen1);
      du.Free();

      at2.Allocate(iV02rel);
      du.Allocate(iV02rel,3);
      for (k=1; k<=iV02rel; k++) {
        if (idx02rel(k)>0)
          at2(k) = pw->apicked(idx02rel(k));
        else {
          if (idx02rel(k)==-1) at2(k) = i;
          if (idx02rel(k)==-2) at2(k) = j;
        }
      }
      BVectFromCircuit(pw, iV02rel, at2, B2, Blen2);
      du.Free();

      if (pw->interactive) {
        printf("Burgers circuit 1:\n");
        printf("iV01 = %d\n",iV01rel);
        printf("coordinates od nodes of the Burgers circuit:\n");
        for (k=1; k<=iV01rel; k++) {
          printf("%d | %0.4lf %0.4lf\n", at1(k), V01rel(k,1), V01rel(k,2));
        }
        printf("B1 = %0.4lf %0.4lf %0.4lf\n",B1(1), B1(2), B1(3));
        printf("|B1| = %0.4lf\n",Blen1);
        printf("Burgers circuit 2:\n");
        printf("iV02 = %d\n",iV02rel);
        printf("coordinates od nodes of the Burgers circuit:\n");
        for (k=1; k<=iV02rel; k++) {
          printf("%d | %0.4lf %0.4lf\n", at2(k), V02rel(k,1), V02rel(k,2));
        }
        printf("B2 = %0.4lf %0.4lf %0.4lf\n",B2(1), B2(2), B2(3));
        printf("|B2| = %0.4lf\n",Blen2);

	p = new QPainter(pw);
	brush = new QBrush();
	pen = new QPen();

	pen->setColor(Qt::darkGray);
	pen->setWidth(8);
	p->setPen(*pen);
	x1 = pw->ZFact*Prel(1,1) - pw->xPan - (pw->ZFact-1)*pw->xyzCen(1);
	y1 = pw->ZFact*Prel(1,2) - pw->yPan - (pw->ZFact-1)*pw->xyzCen(2);
	x2 = pw->ZFact*Prel(2,1) - pw->xPan - (pw->ZFact-1)*pw->xyzCen(1);
	y2 = pw->ZFact*Prel(2,2) - pw->yPan - (pw->ZFact-1)*pw->xyzCen(2);
	pw->DrawLine(p, x1, y1, x2, y2);

	pen->setColor(Qt::green);
	pen->setWidth(6);
	p->setPen(*pen);
	for (k=1; k<iV1rel; k++) {
	  x1 = pw->ZFact*V1rel(k,1) - pw->xPan - (pw->ZFact-1)*pw->xyzCen(1);
	  y1 = pw->ZFact*V1rel(k,2) - pw->yPan - (pw->ZFact-1)*pw->xyzCen(2);
	  x2 = pw->ZFact*V1rel(k+1,1) - pw->xPan - (pw->ZFact-1)*pw->xyzCen(1);
	  y2 = pw->ZFact*V1rel(k+1,2) - pw->yPan - (pw->ZFact-1)*pw->xyzCen(2);
	  pw->DrawLine(p, x1, y1, x2, y2);
	}

	pen->setColor(Qt::darkGreen);
	pen->setWidth(2);
	p->setPen(*pen);
	for (k=1; k<iV01rel; k++) {
	  x1 = pw->ZFact*pw->xyzRel(at1(k),1) - pw->xPan - (pw->ZFact-1)*pw->xyzCen(1);
	  y1 = pw->ZFact*pw->xyzRel(at1(k),2) - pw->yPan - (pw->ZFact-1)*pw->xyzCen(2);
	  x2 = pw->ZFact*pw->xyzRel(at1(k+1),1) - pw->xPan - (pw->ZFact-1)*pw->xyzCen(1);
	  y2 = pw->ZFact*pw->xyzRel(at1(k+1),2) - pw->yPan - (pw->ZFact-1)*pw->xyzCen(2);
	  pw->DrawLine(p, x1, y1, x2, y2);
	}

	pen->setColor(Qt::cyan);
	pen->setWidth(6);
	p->setPen(*pen);
	for (k=1; k<iV2rel; k++) {
	  x1 = pw->ZFact*V2rel(k,1) - pw->xPan - (pw->ZFact-1)*pw->xyzCen(1);
	  y1 = pw->ZFact*V2rel(k,2) - pw->yPan - (pw->ZFact-1)*pw->xyzCen(2);
	  x2 = pw->ZFact*V2rel(k+1,1) - pw->xPan - (pw->ZFact-1)*pw->xyzCen(1);
	  y2 = pw->ZFact*V2rel(k+1,2) - pw->yPan - (pw->ZFact-1)*pw->xyzCen(2);
	  pw->DrawLine(p, x1, y1, x2, y2);
	}

	pen->setColor(Qt::blue);
	pen->setWidth(2);
	p->setPen(*pen);
	for (k=1; k<iV02rel; k++) {
	  x1 = pw->ZFact*pw->xyzRel(at2(k),1) - pw->xPan - (pw->ZFact-1)*pw->xyzCen(1);
	  y1 = pw->ZFact*pw->xyzRel(at2(k),2) - pw->yPan - (pw->ZFact-1)*pw->xyzCen(2);
	  x2 = pw->ZFact*pw->xyzRel(at2(k+1),1) - pw->xPan - (pw->ZFact-1)*pw->xyzCen(1);
	  y2 = pw->ZFact*pw->xyzRel(at2(k+1),2) - pw->yPan - (pw->ZFact-1)*pw->xyzCen(2);
	  pw->DrawLine(p, x1, y1, x2, y2);
	}
      }

      // Determine in which subpolygon the dislocation resides. We stop if the Burgers vectors
      // obtained from both circuits are nonzero or they are both zero. The former can happen, for
      // example, in fcc crystals, where the initial polygon encompasses both Shockley partial
      // dislocations. At some later time, it can happen that the polygon splits into two each of
      // which contains one partial.

      if ((Blen1 > 1e-4 && Blen2 > 1e-4) || (Blen1 < 1e-4 && Blen2 < 1e-4)) {
        printf("!!! Two or no dislocations present in both circuits !!!\n");
        getchar();
        exit(0);
      }

      if (Blen1 > 1e-4) {
        iVrel = iV1rel;
        Vrel = V1rel;
        //	if (xfound_init) {
        //	  iVinit = iV1init;
        //	  Vinit = V1init;
        //	} else {
          //	  printf("xfound_init is false.\n");
        //	}
        Bv = B1;
        if (fabs(Bv(3)-pw->Bvect(3)) > 1e-4) {
          printf("B = %0.4lf %0.4lf %0.4lf\n",Bv(1), Bv(2), Bv(3));
          //exit(0);
        }
        if (pw->interactive) printf("V1 is the new polygon.\n");
      } else {
        iVrel = iV2rel;
        Vrel = V2rel;
        //	if (xfound_init) {
        //	  iVinit = iV2init;
        //	  Vinit = V2init;
        //	} else {
          //	  printf("xfound_init is false.\n");
        //	}
        Bv = B2;
        if (fabs(Bv(3)-pw->Bvect(3)) > 1e-4) {
          printf("B = %0.4lf %0.4lf %0.4lf\n",Bv(1), Bv(2), Bv(3));
          //exit(0);
        }
        if (pw->interactive) printf("V2 is the new polygon.\n");
      }

      if (pw->interactive) {
        pen->setColor(Qt::red);
        pen->setWidth(2);
        p->setPen(*pen);
        for (k=1; k<iVrel; k++) {
          x1 = pw->ZFact*Vrel(k,1) - pw->xPan - (pw->ZFact-1)*pw->xyzCen(1);
          y1 = pw->ZFact*Vrel(k,2) - pw->yPan - (pw->ZFact-1)*pw->xyzCen(2);
          x2 = pw->ZFact*Vrel(k+1,1) - pw->xPan - (pw->ZFact-1)*pw->xyzCen(1);
          y2 = pw->ZFact*Vrel(k+1,2) - pw->yPan - (pw->ZFact-1)*pw->xyzCen(2);
          pw->DrawLine(p, x1, y1, x2, y2);
        }

	at1.Free();
	at2.Free();
	p->end();
	pw->repaint();
      }

      if (pw->interactive) getchar();
      if (firstcut) firstcut = false;
    }
  }

  // the polygon encompassing the center of the dislocation
  xdinit = ydinit = 0;
  xmninit = 1e10;  xmxinit = -1e10;
  ymninit = 1e10;  ymxinit = -1e10;
  pw->ndpoly_init = iVinit;
  pw->dpoly_init.Free();
  pw->dpoly_init.Allocate(iVinit,2);
  for (i=1; i<=iVinit; i++) {
    if (i<iVinit) {
      if (Vinit(i,1)<xmninit) xmninit = Vinit(i,1);
      if (Vinit(i,1)>xmxinit) xmxinit = Vinit(i,1);
      if (Vinit(i,2)<ymninit) ymninit = Vinit(i,2);
      if (Vinit(i,2)>ymxinit) ymxinit = Vinit(i,2);
      xdinit += Vinit(i,1);
      ydinit += Vinit(i,2);
    }
    pw->dpoly_init(i,1) = Vinit(i,1);
    pw->dpoly_init(i,2) = Vinit(i,2);
  }
  xdinit /= iVinit-1;
  ydinit /= iVinit-1;
  xdinit_tol = (xmxinit-xmninit)/2.0;
  ydinit_tol = (ymxinit-ymninit)/2.0;

  xdrel = ydrel = 0;
  xmnrel = 1e10;  xmxrel = -1e10;
  ymnrel = 1e10;  ymxrel = -1e10;
  pw->ndpoly_rel = iVrel;
  pw->dpoly_rel.Free();
  pw->dpoly_rel.Allocate(iVrel,2);
  for (i=1; i<=iVrel; i++) {
    if (i<iVrel) {  // the last point is the same as the first point
      if (Vrel(i,1)<xmnrel) xmnrel = Vrel(i,1);
      if (Vrel(i,1)>xmxrel) xmxrel = Vrel(i,1);
      if (Vrel(i,2)<ymnrel) ymnrel = Vrel(i,2);
      if (Vrel(i,2)>ymxrel) ymxrel = Vrel(i,2);
      xdrel += Vrel(i,1);
      ydrel += Vrel(i,2);
    }
    pw->dpoly_rel(i,1) = Vrel(i,1);
    pw->dpoly_rel(i,2) = Vrel(i,2);
  }
  xdrel /= iVrel-1;
  ydrel /= iVrel-1;
  xdrel_tol = (xmxrel-xmnrel)/2.0;
  ydrel_tol = (ymxrel-ymnrel)/2.0;

  if (pw->interactive) {
    printf("*** SEARCH FOR THE DISLOCATION POSITION IS COMPLETED ***\n");
    printf("Coordinates of the vertices of the polygon in the RELAXED configuration:\n");
    for (i=1; i<=iVrel; i++)
      printf("Vrel(%d) = %0.4lf %0.4lf\n", i, Vrel(i,1), Vrel(i,2));
    printf("Estimated position of the dislocation in the RELAXED configuration:\n");
    printf("xDrel = %0.4lf +/- %0.4lf\nyDrel = %0.4lf +/- %0.4lf\n", xdrel, xdrel_tol, ydrel, ydrel_tol);
    printf("----------------------------------------------------------------------------\n");
    printf("Coordinates of the vertices of the polygon in the INITIAL configuration:\n");
    for (i=1; i<=iVinit; i++)
      printf("Vinit(%d) = %0.4lf %0.4lf\n", i, Vinit(i,1), Vinit(i,2));
    printf("Estimated position of the dislocation in the INITIAL configuration:\n");
    printf("xDinit = %0.4lf +/- %0.4lf\nyDinit = %0.4lf +/- %0.4lf\n", xdinit, xdinit_tol, ydinit, ydinit_tol);
  }

  if (!pw->interactive) pw->repaint();

  // show the Qt dialog with the results
  if (showdlg) {
    Ui::DDPosBurgCircuitResults ui;
    QDialog *dialog = new QDialog;
    ui.setupUi(dialog);

    txt = QString::asprintf("%0.4lf", xdrel);
    ui.xdrel->setText(txt);
    txt = QString::asprintf("%0.4lf", ydrel);
    ui.ydrel->setText(txt);
    txt = QString::asprintf("%0.4lf", xdrel_tol);
    ui.xdrel_tol->setText(txt);
    txt = QString::asprintf("%0.4lf", ydrel_tol);
    ui.ydrel_tol->setText(txt);

    txt = QString::asprintf("%0.4lf", xdinit);
    ui.xdinit->setText(txt);
    txt = QString::asprintf("%0.4lf", ydinit);
    ui.ydinit->setText(txt);
    txt = QString::asprintf("%0.4lf", xdinit_tol);
    ui.xdinit_tol->setText(txt);
    txt = QString::asprintf("%0.4lf", ydinit_tol);
    ui.ydinit_tol->setText(txt);

    ui.coorNodes_rel->setRowCount(iVrel-1);
    for (i=1; i<iVrel; i++) {
      txt = QString::asprintf("%0.4lf", Vrel(i,1));
      QTableWidgetItem *witem1 = new QTableWidgetItem(txt);
      ui.coorNodes_rel->setItem(i-1, 0, witem1);
      txt = QString::asprintf("%0.4lf", Vrel(i,2));
      QTableWidgetItem *witem2 = new QTableWidgetItem(txt);
      ui.coorNodes_rel->setItem(i-1, 1, witem2);
    }

    ui.coorNodes_init->setRowCount(iVinit-1);
    for (i=1; i<iVinit; i++) {
      txt = QString::asprintf("%0.4lf", Vinit(i,1));
      QTableWidgetItem *witem1 = new QTableWidgetItem(txt);
      ui.coorNodes_init->setItem(i-1, 0, witem1);
      txt = QString::asprintf("%0.4lf", Vinit(i,2));
      QTableWidgetItem *witem2 = new QTableWidgetItem(txt);
      ui.coorNodes_init->setItem(i-1, 1, witem2);
    }

    for (i=1; i<=3; i++) {
      if (fabs(Bv(i))<1e-4) Bv(i) = 0;
    }

    txt = QString::asprintf("%0.4lf", Bv(1));
    ui.Bx->setText(txt);
    txt = QString::asprintf("%0.4lf", Bv(2));
    ui.By->setText(txt);
    txt = QString::asprintf("%0.4lf", Bv(3));
    ui.Bz->setText(txt);

    dialog->exec();
    ok = dialog->result()==QDialog::Accepted;
    if (ok) {
      pw->showDPoly = ui.showDPoly->isChecked();
      pw->repaint();
    }
  }

  aidx.Free();

  Vinit.Free();
  V0init.Free();
  V1init.Free();
  V2init.Free();
  V01init.Free();
  V02init.Free();
  tVinit.Free();
  tV0init.Free();
  idx1init.Free();
  idx2init.Free();
  idx01init.Free();
  idx02init.Free();

  Vrel.Free();
  V0rel.Free();
  V1rel.Free();
  V2rel.Free();
  V01rel.Free();
  V02rel.Free();
  tVrel.Free();
  tV0rel.Free();
  idx1rel.Free();
  idx2rel.Free();
  idx01rel.Free();
  idx02rel.Free();
}


