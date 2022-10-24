#include <qtablewidget.h>

#include "ui_dcoordatoms.h"

#include "appwin.h"
#include "neighlist.h"
#include "pltwin.h"
#include "selectzlayers.h"


void AppWin::coordAtoms()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  QTableWidgetItem witem;
  QString str;
  int i, d;
  bool ok;

  Ui::DCoordAtoms ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);
  ui.coordTable->setRowCount(pw->NInit);

  for (i=1; i<=pw->NInit; i++) {
    for (d=1; d<=3; d++) {
      str = QString::asprintf("%0.4f", pw->xyzInit(i,d));
      QTableWidgetItem *witem1 = new QTableWidgetItem(str);
      ui.coordTable->setItem(i-1, d-1, witem1);
      str = QString::asprintf("%0.4f", pw->xyzRel(i,d));
      QTableWidgetItem *witem2 = new QTableWidgetItem(str);
      ui.coordTable->setItem(i-1, d+2, witem2);
    }
  }

  dialog->exec();
  ok = dialog->result()==QDialog::Accepted;
}


void AppWin::moveZLayersDown()
{
  PltWin *pw = plotWidget[tabWidget->currentIndex()];
  int i;

  if (!pw->zLayerSel(1)) { 
    for (i=1; i<pw->NumZLayers; i++)
      pw->zLayerSel(i) = pw->zLayerSel(i+1);
    pw->zLayerSel(pw->NumZLayers) = false;

    // Construct the neighbor list in the unrelaxed configuration
    if (pw->DispComponent!=NONE)
      InitNeighborList(pw, pw->NeighListInit, pw->numNeighInit);
    
    // must re-calculate the relative displacements - arrows plotted between different neighbors
    if (pw->DispComponent==EDGE)
      pw->calcEdgeRelDisp();
    else if (pw->DispComponent==SCREW) {
      pw->calcScrewRelDisp();
    }
    
    pw->repaint();   
    ui->actZLayDown->setEnabled(!pw->zLayerSel(1));
    ui->actZLayUp->setEnabled(!pw->zLayerSel(pw->NumZLayers));
  }
}


void AppWin::moveZLayersUp()
{
  PltWin *pw = plotWidget[tabWidget->currentIndex()];
  int i;
  
  if (!pw->zLayerSel(pw->NumZLayers)) { 
    for (i=pw->NumZLayers; i>1; i--)
      pw->zLayerSel(i) = pw->zLayerSel(i-1);
    pw->zLayerSel(1) = false;

    // Construct the neighbor list in the unrelaxed configuration
    if (pw->DispComponent!=NONE)
      InitNeighborList(pw, pw->NeighListInit, pw->numNeighInit);
    
    // must re-calculate the relative displacements - arrows plotted between different neighbors
    if (pw->DispComponent==EDGE)
      pw->calcEdgeRelDisp();
    else if (pw->DispComponent==SCREW) {
      pw->calcScrewRelDisp();
    }

    pw->repaint();
    ui->actZLayDown->setEnabled(!pw->zLayerSel(1));
    ui->actZLayUp->setEnabled(!pw->zLayerSel(pw->NumZLayers));
  }
}


void AppWin::showZLayers()
{
  int i;

  SHOW_ZLAYERS = !SHOW_ZLAYERS;
  ui->actShowZLay->setChecked(SHOW_ZLAYERS);

  // apply to all selected plots
  for (i=0; i<tabWidget->count(); i++) {
    if (plotWidget[i]->isSelected)
      plotWidget[i]->showZLayers = !plotWidget[i]->showZLayers;
  }
  
  plotWidget[ tabWidget->currentIndex() ]->repaint();
}


void AppWin::selectZLayers()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  QString item, stat;
  int res, istart, iend, iplt;
  int i, ii;
  bool selFlag;

  SelectZLayers *dialog = new SelectZLayers;

  // set up the data
  ii = 0;
  for (i=pw->NumZLayers-1; i>=0; i--) {
    item = QString::number( pw->zCoordLayer(i+1), 'f', 4 );
    selFlag = pw->zLayerSel(i+1);

    dialog->zLayers->addItem(item);
    dialog->zLayers->item(ii)->setSelected(selFlag);
    ii++;
  }

  // get the user's response
  dialog->exec();
  res = dialog->result();

  // clicked OK
  if (res==QDialog::Accepted) {
    if (dialog->applyHow->currentIndex()==0)
      istart = iend = tabWidget->currentIndex();
    else {
      istart = 0;
      iend = tabWidget->count()-1;
    }
    
    for (iplt=istart; iplt<=iend; iplt++) {
      PltWin *pw = plotWidget[iplt];
      
      ii = 0;
      for (i=pw->NumZLayers-1; i>=0; i--) {
        selFlag = dialog->zLayers->item(ii)->isSelected();
	pw->zLayerSel(i+1) = selFlag;
	ii++;
      }
      
      if (VERBOSE) {
	if (iend>istart)
	  printf("* working on plot #%d:  %s ...\n",iplt+1,pw->FName.toLatin1().data());
	printf("* list of z-layers, their positions along the z-axis and visibility:\n");
	printf("  (differential displacements will be calculated only between atoms in visible layers)\n");
	
	for (i=pw->NumZLayers-1; i>=0; i--) {
	  if (pw->zLayerSel(i))
	    stat = "visible";
	  else
	    stat = "hidden";
	  
	  printf("    %3d  %8.4lf  %s\n", 
		 i, pw->zCoordLayer(i), stat.toLatin1().data());
	}
      }
      
      // Construct the neighbor list in the unrelaxed configuration
      if (pw->DispComponent!=NONE)
	InitNeighborList(pw, pw->NeighListInit, pw->numNeighInit);
      
      // must re-calculate the relative displacements - arrows plotted between different neighbors
      if (pw->DispComponent==EDGE)
	pw->calcEdgeRelDisp();
      else if (pw->DispComponent==SCREW) {
	pw->calcScrewRelDisp();
      }
      
      pw->repaint();
    }
  }

  ui->actZLayDown->setEnabled(!pw->zLayerSel(1));
  ui->actZLayUp->setEnabled(!pw->zLayerSel(pw->NumZLayers));
}
