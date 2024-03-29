#include "ui_darrneighbors.h"
#include "ui_darrowstyle.h"
#include "ui_dscalearrow.h"
#include "ui_dztolerance.h"

#include "appwin.h"
#include "atomstyle.h"
#include "lookandfeel.h"
#include "messages.h"
#include "neighlist.h"
#include "pltwin.h"
#include "scalepos.h"


// Choose between which pairs of neighboring atoms the arrows should be plotted.
void AppWin::arrNeighbors()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  QString txt;
  int i, j, istart, iend;
  bool ok;

  Ui::DArrNeighbors ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);

  // set the neighbor distances
  if (pw->arrNeighNum>0 && pw->RdfNNeigh>0) {
    if (pw->RdfDist.len[1] == 1)
      txt.sprintf("R = %0.3f", pw->RdfDist(1));      
    else
      txt.sprintf("R = %0.3f - %0.3f", pw->RdfDist(1,1), pw->RdfDist(1,2));
    ui.Rdist1->setText(txt);
  }
  if (pw->arrNeighNum>0 && pw->RdfNNeigh>1) {
    if (pw->RdfDist.len[1] == 1)
      txt.sprintf("R = %0.3f", pw->RdfDist(2));
    else
      txt.sprintf("R = %0.3f - %0.3f", pw->RdfDist(2,1), pw->RdfDist(2,2));
    ui.Rdist2->setText(txt); 
  }
  if (pw->arrNeighNum>0 && pw->RdfNNeigh>2) {
    if (pw->RdfDist.len[1] == 1)
      txt.sprintf("R = %0.3f", pw->RdfDist(3));
    else
      txt.sprintf("R = %0.3f - %0.3f", pw->RdfDist(3,1), pw->RdfDist(3,2));
    ui.Rdist3->setText(txt); 
  }
  if (pw->arrNeighNum>0 && pw->RdfNNeigh>3) {
    if (pw->RdfDist.len[1] == 1)
      txt.sprintf("R = %0.3f", pw->RdfDist(4));
    else
      txt.sprintf("R = %0.3f - %0.3f", pw->RdfDist(4,1), pw->RdfDist(4,2));
    ui.Rdist4->setText(txt); 
  }
  if (pw->arrNeighNum>0 && pw->RdfNNeigh>4) {
    if (pw->RdfDist.len[1] == 1)
      txt.sprintf("R = %0.3f", pw->RdfDist(5));
    else
      txt.sprintf("R = %0.3f - %0.3f", pw->RdfDist(5,1), pw->RdfDist(5,2));
    ui.Rdist5->setText(txt); 
  }

  // set the checkboxes
  for (i=1; i<=pw->arrNeighNum; i++) {
    if (pw->arrNeighbors(i)==1)  ui.Rcheck1->setChecked(1);
    if (pw->arrNeighbors(i)==2)  ui.Rcheck2->setChecked(1);
    if (pw->arrNeighbors(i)==3)  ui.Rcheck3->setChecked(1);
    if (pw->arrNeighbors(i)==4)  ui.Rcheck4->setChecked(1);
    if (pw->arrNeighbors(i)==5)  ui.Rcheck5->setChecked(1);
  }

  // execute the dialog
  dialog->exec();
  ok = dialog->result()==QDialog::Accepted;

  // change settings, if needed
  if (ok) {
    if (ui.applyHow->currentIndex()==0)
      istart = iend = tabWidget->currentIndex();
    else {
      istart = 0;
      iend = tabWidget->count()-1;
    }

    for (i=istart; i<=iend; i++) {
      pw = plotWidget[i];
      if (!pw->isSelected)
	continue;

      pw->arrNeighNum = 0;
      if (ui.Rcheck1->isChecked()) {
	pw->arrNeighNum++;
	pw->arrNeighbors(pw->arrNeighNum) = 1;
      }
      if (ui.Rcheck2->isChecked()) {
	pw->arrNeighNum++;
	pw->arrNeighbors(pw->arrNeighNum) = 2;
      }
      if (ui.Rcheck3->isChecked()) {
	pw->arrNeighNum++;
	pw->arrNeighbors(pw->arrNeighNum) = 3;
      }
      if (ui.Rcheck4->isChecked()) {
	pw->arrNeighNum++;
	pw->arrNeighbors(pw->arrNeighNum) = 4;
      }
      if (ui.Rcheck5->isChecked()) {
	pw->arrNeighNum++;
	pw->arrNeighbors(pw->arrNeighNum) = 5;
      }

      if (VERBOSE) {
	if (pw->arrNeighNum>0) {
	  printf("* the arrows will be plotted between these nearest neighbors:\n");
	  printf("    ");
	  for (j=1; j<=pw->arrNeighNum; j++)
	    printf("%d ", pw->arrNeighbors(j));
	  printf("\n"); 
	}
      }

      // re-calculate the linked neighbor list
      if (pw->DispComponent!=NONE)
	InitNeighborList(pw, pw->NeighListInit, pw->numNeighInit);

      // must re-calculate the relative displacements - arrows plotted between different neighbors
      if (pw->DispComponent==EDGE)
	pw->calcEdgeRelDisp();
      else if (pw->DispComponent==SCREW)
	pw->calcScrewRelDisp();
    }

    // repaint only the visible widget
    plotWidget[ tabWidget->currentIndex() ]->repaint();
  }
}


void AppWin::arrowsPlot()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  int i,istart,iend;
  bool ok;

  Ui::DArrowStyle ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);

  ui.ithickness->setValue( pw->thicknessArrow );
  ui.ithickness->selectAll();
  ui.ishortest->setValue( pw->shortestArrow );

  dialog->exec();
  ok = dialog->result()==QDialog::Accepted;

  if (ok) {
    // to which plots should the settings be applied
    if (ui.applyHow->currentIndex()==0)
      istart = iend = tabWidget->currentIndex();
    else {
      istart = 0;
      iend = tabWidget->count()-1;
    }

    // apply the new arrow thickness
    for (i=istart; i<=iend; i++) {
      pw = plotWidget[i];
      if (!pw->isSelected)
	continue;

      pw->thicknessArrow = ui.ithickness->text().toInt();
      pw->shortestArrow = ui.ishortest->text().toInt();
      pw->repaint();
    }
  }
}


void AppWin::crystalPlot()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  int i, j, istart, iend;
  bool ok;

  // declaration
  AtomStyle *dialog = new AtomStyle(pw);
  dialog->setData();

  // execution
  dialog->exec();
  ok = dialog->result()==QDialog::Accepted;

  // change of settings
  if (ok) {
    if (dialog->applyHow->currentIndex()==0)
      istart = iend = tabWidget->currentIndex();
    else {
      istart = 0;
      iend = tabWidget->count()-1;
    }

    for (i=istart; i<=iend; i++) {
      pw = plotWidget[i];
      if (!pw->isSelected)
	continue;
      
      for (j=1; j<=pw->NumZLayers; j++) {
	pw->zDiamLayer(j) = dialog->atomTable->item(j-1,1)->text().toInt();
	pw->zLineThickLayer(j) = dialog->atomTable->item(j-1,2)->text().toInt();
	pw->zColorLayer(j,1) = dialog->zColor(j,1);
	pw->zColorLayer(j,2) = dialog->zColor(j,2);
      }
      pw->PrepareZColoredSpheres();
      pw->repaint();
    }
  }
}


void AppWin::lookAndFeel()
{
  QPalette pal;
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  int i;
  bool ok;

  LookAndFeel *dialog = new LookAndFeel(pw);
  dialog->antialiase->setChecked(ANTIALIASE);
  dialog->atom_3dsphere->setChecked(ATOM_3DSPHERE);
  pal.setColor(QPalette::Base, BACKGROUND_COLOR);
  dialog->bgcolor->setPalette(pal);
  dialog->bgcol = BACKGROUND_COLOR;
  
  dialog->exec();
  ok = dialog->result()==QDialog::Accepted;
  if (ok) {
    ANTIALIASE = dialog->antialiase->isChecked();
    ATOM_3DSPHERE = dialog->atom_3dsphere->isChecked();
    BACKGROUND_COLOR = dialog->bgcol;
    
    QPalette pal;
    pal.setColor(QPalette::Window, BACKGROUND_COLOR);
    for (i=0; i<=tabWidget->count()-1; i++)
      plotWidget[i]->setPalette(pal);    
    plotWidget[tabWidget->currentIndex()]->repaint();
  }
}


void AppWin::scaleArrow()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  double sfact;
  int i,istart,iend;
  bool ok,okk;

  Ui::DScaleArrow ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);

  ui.sfactor->setText( QString::number( pw->DScaleFact ));
  ui.sfactor->selectAll();
  do {
    dialog->exec();
    ok = dialog->result()==QDialog::Accepted;

    if (ok) {
      sfact = ui.sfactor->text().toDouble( &okk );
      if (okk) {
	// get the indexes for widgets to which this should be applied
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

	  pw->DScaleFact = sfact; 
	  pw->repaint();
	}
      } else
        msgError(err_ScaleArrowIsString);   
    }
  } while (ok && !okk);

  repaintStatusBar();
}


void AppWin::scalePos()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  double fact;
  int i, istart, iend;
  bool ok;

  ScalePos *dialog = new ScalePos(pw);
  dialog->magRatio->setText( QString::number( pw->factor ));
  dialog->magRatio->selectAll();

  dialog->exec();
  
  // which button has been clicked ?
  ok = dialog->result()==QDialog::Accepted;
  if (ok) {
    fact = dialog->magRatio->text().toDouble( &ok );
    if (!ok) {
      msgError(err_ScalePosIsString);   
      return;
    }

    // check if the magnification ratio should be applied to the current widget
    // or to all selected plots
    if (dialog->applyHow->currentIndex()==0)
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
      
      pw->factor = fact;
      pw->xOffset = (int) floor( (pw->width() - 2*pw->xBorder - pw->factor*pw->blSize(1))/2.0 );
      pw->yOffset = (int) floor( (pw->height() - 2*pw->yBorder - pw->factor*pw->blSize(2))/2.0 );
    
      pw->repaint();
    }
  }
}


void AppWin::setZTolerance()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  double zTol;
  int istart, iend, i;
  bool ok;
  
  Ui::DZTolerance ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);

  ui.zTolerance->setText( QString::number( pw->zTolerance ));

  dialog->exec();
  
  ok = dialog->result()==QDialog::Accepted;
  if (ok) {
    zTol = ui.zTolerance->text().toDouble( &ok );
    if (!ok) {
      msgError(err_ZToleranceIsString);   
      return;
    }

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
      
      pw->zTolerance = zTol;
      pw->InitBlockGeometry(false);
      pw->InitBlockANeighbors();
      pw->repaint();
    }
  }
}
