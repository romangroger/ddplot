#include <qdialog.h>
#include <qdir.h>
#include <qstring.h>

#include "ui_dcoordsys.h"
#include "ui_dprojvector.h"
#include "ui_dtransform.h"

#include "appwin.h"
#include "calcs.h"
#include "messages.h"


bool AppWin::coordSys()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  int istart, iend, i;
  bool ok;

  Ui::DCoordSys ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);

  ui.x1->insert( QString::number( pw->csys(1,1) ));
  ui.x1->selectAll();
  ui.x2->insert( QString::number( pw->csys(1,2) ));
  ui.x3->insert( QString::number( pw->csys(1,3) ));
  ui.y1->insert( QString::number( pw->csys(2,1) ));
  ui.y2->insert( QString::number( pw->csys(2,2) ));
  ui.y3->insert( QString::number( pw->csys(2,3) ));
  ui.z1->insert( QString::number( pw->csys(3,1) ));
  ui.z2->insert( QString::number( pw->csys(3,2) ));
  ui.z3->insert( QString::number( pw->csys(3,3) ));

  dialog->exec();
  ok = dialog->result()==QDialog::Accepted;

  if (ok) {
    if (ui.applyHow->currentIndex()==0)
      istart = iend = tabWidget->currentIndex();
    else {
      istart = 0;
      iend = tabWidget->count()-1;
    }
    
    // apply the new magnification ratio
    for (i=istart; i<=iend; i++) {
      pw = plotWidget[i];
      if (!pw->isSelected)
	continue;

      pw->csys(1,1) = ui.x1->text().toDouble();
      pw->csys(1,2) = ui.x2->text().toDouble();
      pw->csys(1,3) = ui.x3->text().toDouble();
      pw->csys(2,1) = ui.y1->text().toDouble();
      pw->csys(2,2) = ui.y2->text().toDouble();
      pw->csys(2,3) = ui.y3->text().toDouble();
      pw->csys(3,1) = ui.z1->text().toDouble();
      pw->csys(3,2) = ui.z2->text().toDouble();
      pw->csys(3,3) = ui.z3->text().toDouble();
      pw->repaint();
    }
  }

  return ok;
}


void AppWin::projVector()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  DVector coord(3);
  int NPlots = tabWidget->count();
  int i,n;
  bool ok, okx, oky, okz, ok2;

  do {
    // dialog may not be WDestructiveClose, because it would delete all data inside exec()
    Ui::DProjVector ui;
    QDialog *dialog = new QDialog;
    ui.setupUi(dialog);

    ui.xcoord->insert( QString::number( pw->ProjVector(1) ));
    ui.xcoord->selectAll();
    ui.ycoord->insert( QString::number( pw->ProjVector(2) ));
    ui.zcoord->insert( QString::number( pw->ProjVector(3) ));

    dialog->exec();
    ok = dialog->result()==QDialog::Accepted;

    if (ok) {
      coord(1) = ui.xcoord->text().toDouble( &okx );
      coord(2) = ui.ycoord->text().toDouble( &oky );
      coord(3) = ui.zcoord->text().toDouble( &okz );

      if (okx && oky && okz) {
	for (n=0; n<NPlots; n++) {
	  pw = plotWidget[n];
	  if (!pw->isSelected) 
	    continue;

	  for (i=1; i<=3; i++)
	    pw->ProjVector(i) = coord(i);

	  ok2 = pw->calcProjRelDisp();
	  if (ok2) {
	    pw->DispComponent = PROJ;
	    repaintStatusBar();
	    pw->repaint();
	  }
	}
      } else
        msgError(err_ProjVectIsString);
    }
  } while ( ok && (!okx || !oky || !okz) );
}


// Reflect the block through the given plane
void AppWin::reflectBlock(QAction *action)
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  int maxis;
  int i, d, n;

  if (action==ui->actXZReflect) {
    maxis = 2;
    if (VERBOSE)
      printf("* the plot has been reflected through the XZ plane\n");
  } else if (action==ui->actYZReflect) {
    maxis = 1;
    if (VERBOSE)
      printf("* the plot has been reflected through the YZ plane\n");
  } else if (action==ui->actXYReflect) {
    maxis = 3;
    for (i=1; i<=pw->NumZLayers; i++)
      pw->zCoordLayer(i) *= -1;
    if (VERBOSE)
      printf("* the plot has been reflected through the XY plane\n");
  }

  for (n=1; n<=pw->NInit; n++)
    pw->xyzInit(n,maxis) *= -1; 

  for (n=1; n<=pw->NInert; n++)
    pw->xyzInert(n,maxis) *= -1; 
    
  for (n=1; n<=pw->NRel; n++)
    pw->xyzRel(n,maxis) *= -1;
    
  for (n=1; n<=pw->NRel; n++) {
    for (d=1; d<=3; d++)
      pw->aDisp(n,d) = pw->xyzRel(n,d) - pw->xyzInit(n,d);  
  }

  for (i=1; i<=3; i++)
    pw->csys(maxis,i) *= -1;

  // must re-calculate the scaling of arrows
  if (pw->DispComponent==EDGE)
    pw->calcEdgeRelDisp();
  else if (pw->DispComponent==SCREW)
    pw->calcScrewRelDisp();

  pw->InitBlockGeometry(false);
  pw->InitBlockANeighbors();
  pw->SetGeometry();
  pw->repaint();
}


// Rotate the block 180 deg about a specific axis
void AppWin::rotateBlock(QAction *action)
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  DMatrix mat(3,3), prod(3,3);
  DVector coor(3);
  int i, j, k, d, n;

  if (action==ui->actXRot180) {
      mat(1,1) =  1;  mat(1,2) =  0;  mat(1,3) =  0;
      mat(2,1) =  0;  mat(2,2) = -1;  mat(2,3) =  0;
      mat(3,1) =  0;  mat(3,2) =  0;  mat(3,3) = -1;
      for (i=1; i<=pw->NumZLayers; i++)
	pw->zCoordLayer(i) *= -1;
      if (VERBOSE)
	printf("* the plot has been rotated 180 deg about the X axis\n");
  } else if (action==ui->actYRot180) {
      mat(1,1) = -1;  mat(1,2) =  0;  mat(1,3) =  0;
      mat(2,1) =  0;  mat(2,2) =  1;  mat(2,3) =  0;
      mat(3,1) =  0;  mat(3,2) =  0;  mat(3,3) = -1;
      for (i=1; i<=pw->NumZLayers; i++)
	pw->zCoordLayer(i) *= -1;
      if (VERBOSE)
	printf("* the plot has been rotated 180 deg about the Y axis\n");
  } else if (action==ui->actZRot180) {
      mat(1,1) = -1;  mat(1,2) =  0;  mat(1,3) =  0;
      mat(2,1) =  0;  mat(2,2) = -1;  mat(2,3) =  0;
      mat(3,1) =  0;  mat(3,2) =  0;  mat(3,3) =  1;
      if (VERBOSE)
	printf("* the plot has been rotated 180 deg about the Z axis\n");
  } else if (action==ui->actXRot90) {
      mat(1,1) =  1;  mat(1,2) =  0;  mat(1,3) =  0;
      mat(2,1) =  0;  mat(2,2) =  0;  mat(2,3) = -1;
      mat(3,1) =  0;  mat(3,2) =  1;  mat(3,3) =  0;
      for (i=1; i<=pw->NumZLayers; i++)
	pw->zCoordLayer(i) *= -1;
      if (VERBOSE)
	printf("* the plot has been rotated 90 deg about the X axis\n");
  } else if (action==ui->actYRot90) {
      mat(1,1) =  0;  mat(1,2) =  0;  mat(1,3) =  1;
      mat(2,1) =  0;  mat(2,2) =  1;  mat(2,3) =  0;
      mat(3,1) = -1;  mat(3,2) =  0;  mat(3,3) =  0;
      for (i=1; i<=pw->NumZLayers; i++)
	pw->zCoordLayer(i) *= -1;
      if (VERBOSE)
	printf("* the plot has been rotated 90 deg about the Y axis\n");
  } else if (action==ui->actZRot90) {
      mat(1,1) =  0;  mat(1,2) = -1;  mat(1,3) =  0;
      mat(2,1) =  1;  mat(2,2) =  0;  mat(2,3) =  0;
      mat(3,1) =  0;  mat(3,2) =  0;  mat(3,3) =  1;
      if (VERBOSE)
	printf("* the plot has been rotated 90 deg about the Z axis\n");
  }

  // transformation of coordinates of atoms - unrelaxed
  for (n=1; n<=pw->NInit; n++) {
    for (i=1; i<=3; i++) {
      coor(i) = 0;
      for (j=1; j<=3; j++) {
	coor(i) += mat(i,j)*pw->xyzInit(n,j);
      }
    }
    
    for (i=1; i<=3; i++)
      pw->xyzInit(n,i) = coor(i);
  }

  // transformation of coordinates of atoms - inert
  for (n=1; n<=pw->NInert; n++) {
    for (i=1; i<=3; i++) {
      coor(i) = 0;
      for (j=1; j<=3; j++) {
	coor(i) += mat(i,j)*pw->xyzInert(n,j);
      }
    }
    
    for (i=1; i<=3; i++)
      pw->xyzInert(n,i) = coor(i);
  }
  
  // transformation of coordinates of atoms - relaxed
  for (n=1; n<=pw->NRel; n++) {
    for (i=1; i<=3; i++) {
      coor(i) = 0;
      for (j=1; j<=3; j++) {
	coor(i) += mat(i,j)*pw->xyzRel(n,j);
      }
    }
    
    for (i=1; i<=3; i++)
      pw->xyzRel(n,i) = coor(i);    
  }
  
  // recalculate the relative displacements
  for (n=1; n<=pw->NRel; n++) {
    for (d=1; d<=3; d++)
      pw->aDisp(n,d) = pw->xyzRel(n,d) - pw->xyzInit(n,d);  
  }

  // transformation of the coordinate system
  prod = 0;
  for (i=1; i<=3; i++)
    for (j=1; j<=3; j++)
      for (k=1; k<=3; k++)
	prod(i,j) += mat(i,k) * pw->csys(k,j);
  pw->csys = prod;

  // must re-calculate the scaling of arrows
  if (pw->DispComponent==EDGE)
    pw->calcEdgeRelDisp();
  else if (pw->DispComponent==SCREW)
    pw->calcScrewRelDisp();

  pw->InitBlockGeometry(false);
  pw->InitBlockANeighbors();
  pw->SetGeometry();
  pw->repaint();
}


void AppWin::transform()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  DMatrix newCSys(3,3), mat(3,3), prod(3,3);
  DVector coor(3);
  double sum;
  int res, i, j, n, istart, iend, iplt;
  bool needcsys;


  // Is the coordinate system of the block given ?
  needcsys = true;
  sum = 0;
  for (i=1; i<=3; i++) {
    for (j=1; j<=3; j++) {
      sum += fabs(pw->csys(i,j));
    }
  }
  needcsys = (sum==0);

  if (needcsys) {
    msgInfo(info_GiveCoordBlock);
    return;
  }

  Ui::DTransform ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);

  ui.x1->setText( QString::number( pw->csys(1,1), 'g', 4 ));
  ui.x2->setText( QString::number( pw->csys(1,2), 'g', 4 ));
  ui.x3->setText( QString::number( pw->csys(1,3), 'g', 4 ));
  ui.y1->setText( QString::number( pw->csys(2,1), 'g', 4 ));
  ui.y2->setText( QString::number( pw->csys(2,2), 'g', 4 ));
  ui.y3->setText( QString::number( pw->csys(2,3), 'g', 4 ));
  ui.z1->setText( QString::number( pw->csys(3,1), 'g', 4 ));
  ui.z2->setText( QString::number( pw->csys(3,2), 'g', 4 ));
  ui.z3->setText( QString::number( pw->csys(3,3), 'g', 4 ));
  
  dialog->exec();
  res = dialog->result();
  if (res!=QDialog::Accepted)
    return;

  // target coordinate system
  newCSys(1,1) = ui.X1->text().toDouble();
  newCSys(1,2) = ui.X2->text().toDouble();
  newCSys(1,3) = ui.X3->text().toDouble();
  newCSys(2,1) = ui.Y1->text().toDouble();
  newCSys(2,2) = ui.Y2->text().toDouble();
  newCSys(2,3) = ui.Y3->text().toDouble();
  newCSys(3,1) = ui.Z1->text().toDouble();
  newCSys(3,2) = ui.Z2->text().toDouble();
  newCSys(3,3) = ui.Z3->text().toDouble();
  
  if (VERBOSE) {
    printf("* new orientation of the block will be:\n");
    for (i=1; i<=3; i++)
      printf("    [  %8.4f  %8.4f  %8.4f  ]\n", newCSys(i,1), newCSys(i,2), newCSys(i,3));
  }

  // how to apply this settings?
  if (ui.applyHow->currentIndex()==0)
    istart = iend = tabWidget->currentIndex();
  else {
    istart = 0;
    iend = tabWidget->count()-1;
  }

  // transform the coordinates...
  for (iplt=istart; iplt<=iend; iplt++) {
    pw = plotWidget[iplt];
    if (!pw->isSelected)
      continue;

    // transformation matrix
    mtransf( pw->csys, newCSys, mat );

    // save the new coordinate system
    pw->csys = newCSys;

    // transformation of coordinates of atoms - unrelaxed
    for (n=1; n<=pw->NInit; n++) {
      for (i=1; i<=3; i++) {
	coor(i) = 0;
	for (j=1; j<=3; j++) {
	  coor(i) += mat(i,j)*pw->xyzInit(n,j);
	}
      }
      
      for (i=1; i<=3; i++)
	pw->xyzInit(n,i) = coor(i);
    }
    
    // transformation of coordinates of atoms - relaxed
    for (n=1; n<=pw->NRel; n++) {
      for (i=1; i<=3; i++) {
	coor(i) = 0;
	for (j=1; j<=3; j++) {
	  coor(i) += mat(i,j)*pw->xyzRel(n,j);
	}
      }
      
      for (i=1; i<=3; i++)
	pw->xyzRel(n,i) = coor(i);    
    }

    pw->InitBlockGeometry(false);
    pw->InitBlockANeighbors();
    pw->SetGeometry();
    pw->repaint();
  }
}
