#include <qdialog.h>
#include <qdir.h>

#include "ui_ddefdislopos.h"
#include "ui_drelaxinfo.h"
#include "ui_dshowplanes.h"

#include "appwin.h"
#include "pltwin.h"
#include "selectplots.h"


void AppWin::defDisloPos()
{
  PltWin *pw;
  int res, i, istart, iend;
  
  Ui::DDefDisloPos ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);

  pw = plotWidget[tabWidget->currentIndex()];
  ui.x->setText(QString::number(pw->xCore));
  ui.y->setText(QString::number(pw->yCore));
  dialog->exec();
  
  res = dialog->result();  
  if (res==QDialog::Accepted) {
    if (ui.applyHow->currentIndex()==0)
      istart = iend = tabWidget->currentIndex();
    else {
      istart = 0;
      iend = tabWidget->count()-1;
    }    
    for (i=istart; i<=iend; i++) {
      pw = plotWidget[i];
      if (!pw->isSelected) continue;
      pw->xCore = ui.x->text().toDouble();
      pw->yCore = ui.y->text().toDouble();
      pw->repaint();
    }
  }
}


void AppWin::disloCenter()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];

  SHOW_DCENTER = !SHOW_DCENTER;
  ui->actDCenter->setChecked(SHOW_DCENTER);
  pw->DisloCenter = SHOW_DCENTER; 
  pw->repaint();
  repaintStatusBar();
}


void AppWin::plotAtomLayers()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];

  pw->plotType = PLOT_ATOM_LAYERS;
  pw->repaint();

  ui->actDAtLayers->setChecked(true);
  ui->actDAtTypes->setChecked(false);
  ui->actDAtNeighbors->setChecked(false);
}


void AppWin::plotAtomTypes()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  
  pw->plotType = PLOT_ATOM_TYPES;
  pw->repaint();

  ui->actDAtLayers->setChecked(false);
  ui->actDAtTypes->setChecked(true);
  ui->actDAtNeighbors->setChecked(false);
}


void AppWin::plotAtomNeighbors()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  
  pw->plotType = PLOT_ATOM_NEIGHBORS;
  pw->repaint();

  ui->actDAtLayers->setChecked(false);
  ui->actDAtTypes->setChecked(false);
  ui->actDAtNeighbors->setChecked(true);
}


void AppWin::plotEdgeComponent()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];

  pw->DispComponent = EDGE;
  pw->InitBlockANeighbors();
  pw->calcEdgeRelDisp();
  pw->repaint();
  repaintStatusBar();

  ui->actNoComp->setChecked(false);
  ui->actEdgeComp->setChecked(true);
  ui->actScrewComp->setChecked(false);
}


void AppWin::plotNoComponent()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];

  pw->DispComponent = NONE;
  pw->repaint();
  repaintStatusBar();

  ui->actNoComp->setChecked(true);
  ui->actEdgeComp->setChecked(false);
  ui->actScrewComp->setChecked(false);
}

void AppWin::plotScrewComponent()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];

  pw->DispComponent = SCREW;
  pw->InitBlockANeighbors();
  pw->calcScrewRelDisp();
  pw->repaint();
  repaintStatusBar();

  ui->actNoComp->setChecked(false);
  ui->actEdgeComp->setChecked(false);
  ui->actScrewComp->setChecked(true);
}


void AppWin::plotUnrelaxed()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];

  pw->AtomPos = UNRELAXED;
  pw->repaint();
  repaintStatusBar();

  ui->actUnrelView->setChecked(true);
  ui->actRelView->setChecked(false);
  ui->actCompView->setChecked(false);
}


void AppWin::plotRelaxed()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];

  pw->AtomPos = RELAXED;
  pw->repaint();
  repaintStatusBar();

  ui->actUnrelView->setChecked(false);
  ui->actRelView->setChecked(true);
  ui->actCompView->setChecked(false);
}


void AppWin::plotComposite()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];

  pw->AtomPos = COMPOSITE;
  pw->repaint();
  repaintStatusBar();

  ui->actUnrelView->setChecked(false);
  ui->actRelView->setChecked(false);
  ui->actCompView->setChecked(true);
}


void AppWin::plotInertAtoms()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];

  pw->InertAtoms = !pw->InertAtoms;
  pw->InitBlockGeometry(true);
  pw->SetGeometry();
  pw->repaint();

  ui->actInertAtoms->setChecked(pw->InertAtoms);
}


void AppWin::printMagDisp()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];

  pw->PrintMagDisp = abs(pw->PrintMagDisp-1); 
  pw->repaint();

  ui->actMagDisp->setChecked(pw->PrintMagDisp);
}


void AppWin::printAtomNum()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];

  pw->AtomNumbers = abs(pw->AtomNumbers-1); 
  pw->repaint();

  ui->actNumAtom->setChecked(pw->AtomNumbers);
}


void AppWin::relaxInfo()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];

  Ui::DRelaxInfo ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);

  if (pw->tail!=NULL) {
    ui.textArea->append( pw->tail );
  }

  dialog->exec();
}


void AppWin::selectFirstPlot()
{
  int idx;

  idx = tabWidget->currentIndex();

  if (idx > 0) {
    idx = 0;
    tabWidget->setCurrentIndex(idx);
    ui->actFirstPlot->setEnabled(idx > 0);
    ui->actPrevPlot->setEnabled(idx > 0);
    ui->actNextPlot->setEnabled(idx < tabWidget->count()-1);
    ui->actLastPlot->setEnabled(idx < tabWidget->count()-1);
  }
}


void AppWin::selectLastPlot()
{
  int idx;

  idx = tabWidget->currentIndex();

  if (idx < tabWidget->count()-1) {
    idx = tabWidget->count()-1;
    tabWidget->setCurrentIndex(idx);
    ui->actFirstPlot->setEnabled(idx > 0);
    ui->actPrevPlot->setEnabled(idx > 0);
    ui->actNextPlot->setEnabled(idx < tabWidget->count()-1);
    ui->actLastPlot->setEnabled(idx < tabWidget->count()-1);
  }
}


void AppWin::selectNextPlot()
{
  int idx;

  idx = tabWidget->currentIndex();

  if (idx < tabWidget->count()-1) {
    idx++;
    tabWidget->setCurrentIndex(idx);
    ui->actFirstPlot->setEnabled(idx > 0);
    ui->actPrevPlot->setEnabled(idx > 0);
    ui->actNextPlot->setEnabled(idx < tabWidget->count()-1);
    ui->actLastPlot->setEnabled(idx < tabWidget->count()-1);
  }
}


void AppWin::selectPrevPlot()
{
  int idx;

  idx = tabWidget->currentIndex();

  if (idx > 0) {
    idx--;
    tabWidget->setCurrentIndex(idx);
    ui->actFirstPlot->setEnabled(idx > 0);
    ui->actPrevPlot->setEnabled(idx > 0);
    ui->actNextPlot->setEnabled(idx < tabWidget->count()-1);
    ui->actLastPlot->setEnabled(idx < tabWidget->count()-1);
  }
}


void AppWin::showGb()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  
  pw->showGB = !pw->showGB;
  ui->actShowGb->setChecked(pw->showGB);
  pw->repaint();
}


void AppWin::showCells()
{
  int i;

  SHOW_NEIGHCELLS = !SHOW_NEIGHCELLS;
  ui->actShowCells->setChecked(SHOW_NEIGHCELLS);

  // apply to all selected plots
  for (i=0; i<tabWidget->count(); i++) {
    if (plotWidget[i]->isSelected)
      plotWidget[i]->showNeighCells = !plotWidget[i]->showNeighCells;
  }
  
  plotWidget[ tabWidget->currentIndex() ]->repaint();
}


void AppWin::showCSys()
{
  int i;

  SHOW_CSYS = !SHOW_CSYS;
  ui->actShowCSys->setChecked(SHOW_CSYS);

  // apply to all selected plots
  for (i=0; i<tabWidget->count(); i++) {
    if (plotWidget[i]->isSelected)
      plotWidget[i]->showCSys = !plotWidget[i]->showCSys;
  }
  
  plotWidget[ tabWidget->currentIndex() ]->repaint();
}


void AppWin::showPlanes()
{
  int i,astep;
  bool ok;

  Ui::DShowPlanes ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);

  ui.angle->setValue( PTRACES_ANGLE );
  ui.angle->selectAll();
  dialog->exec();
  ok = dialog->result()==QDialog::Accepted;

  if (!ok)
    return;

  astep = ui.angle->text().toDouble( &ok );
  PTRACES_ANGLE = astep;

  for (i=0; i<tabWidget->count(); i++) {
    if (!plotWidget[i]->isSelected) 
      continue;  
    
    if (astep>0) {
      plotWidget[i]->PlaneTraces = true;
      plotWidget[i]->PTAngle = astep;
    } else {
      plotWidget[i]->PlaneTraces = false;
    }
  }

  // only the current widget needs to be repainted
  plotWidget[ tabWidget->currentIndex() ]->repaint();
}


void AppWin::selectPlots()
{
  QString item;
  int res;
  int i;
  bool selFlag;

  SelectPlots *dialog = new SelectPlots();

  for (i=0; i<tabWidget->count(); i++) {
    item = plotWidget[i]->FName;

    // only the file name is printed
    item = QDir::current().relativeFilePath(item);
 
    selFlag = plotWidget[i]->isSelected;

    dialog->plotFiles->addItem(item);
    dialog->plotFiles->item(i)->setSelected(selFlag);
  }

  // get the user's response
  dialog->exec();
  res = dialog->result();
  
  // clicked OK
  if (res==QDialog::Accepted) {
    for (i=0; i<dialog->plotFiles->count(); i++) {
      selFlag = dialog->plotFiles->item(i)->isSelected();
      plotWidget[i]->isSelected = selFlag;
      tabWidget->setTabEnabled(i, selFlag);
    }
	   
    tabWidget->repaint();
    repaintStatusBar();
  }
}
