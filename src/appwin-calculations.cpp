#include <QDialog>
#include <QDir>
#include <QTabWidget>

#include "ui_dburgvect.h"
#include "ui_dcalcbvector.h"
#include "ui_dcomparedisp.h"
#include "ui_ddefinebcircuit.h"
#include "ui_ddpospnresults.h"
#include "ui_dfinddposdecimate.h"
#include "ui_dfinddpospeierlsnabarro.h"
#include "ui_dsetupprobeatomdispl.h"

#include "appwin.h"
#include "bvect.h"
#include "dpos_peierlsnabarro.h"
#include "dpos_decimate.h"
#include "dpospnresults.h"
#include "messages.h"
#include "pltwin.h"
#include "probeatomdispl.h"
#include "rdfplot.h"


void AppWin::compareDisp()
{
  QString item,cstr;
  int i, p1, p2, comp, idx;
  bool ok, comparable;

  Ui::DCompareDisp ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);

  // set up the data
  for (i=0; i<tabWidget->count(); i++) {
    item = plotWidget[i]->FName;
    
    // only the file name is printed
    item = QDir::current().relativeFilePath(item);
    ui.plot1->addItem(item);
    ui.plot2->addItem(item);
  }

  dialog->exec();
  ok = dialog->result()==QDialog::Accepted;

  if (ok) {
    p1 = ui.plot1->currentRow();
    p2 = ui.plot2->currentRow();
    if (p1==p2) {
      msgInfo(info_CompareSamePlots);
      return;
    }

    comp = EDGE * ui.compEdge->isChecked() + SCREW * ui.compScrew->isChecked();
    if (comp==EDGE) 
      cstr = QString(" (edge)");
    else 
      cstr = QString(" (screw)");

    // create a new widget
    PltWin *pw = new PltWin(tabWidget);
    *pw = *plotWidget[p1];
    
    // set the plot-specific parameters
    if (comp==EDGE)
      pw->DispComponent = DIFF_EDGE;  
    else if (comp==SCREW)
      pw->DispComponent = DIFF_SCREW;     
    pw->FName = tabWidget->tabText(p1) + " (-) " + tabWidget->tabText(p2) + cstr.toLatin1().data();

    comparable = pw->CompareDisp(plotWidget[p2], comp);
    if (!comparable) {
      delete(pw);
      msgError(err_CannotComparePlots);
      return;
    }

    plotWidget.append(pw);
    tabWidget->addTab(pw, pw->FName);
    tabWidget->setCurrentIndex(tabWidget->count()-1);
    tabWidget->show();

    idx = tabWidget->currentIndex();
    this->ui->actFirstPlot->setEnabled(idx > 0);
    this->ui->actPrevPlot->setEnabled(idx > 0);
    this->ui->actNextPlot->setEnabled(idx < tabWidget->count()-1);
    this->ui->actLastPlot->setEnabled(idx < tabWidget->count()-1);

    repaintStatusBar();
  }
}


void AppWin::burgVect()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  QString str;
  int i, istart, iend;
  bool ok;

  Ui::DBurgVect ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);

  str = QString::asprintf("%0.4lf", pw->Bvect(1));
  ui.xcoord->setText(str);
  ui.xcoord->selectAll();
  str = QString::asprintf("%0.4lf", pw->Bvect(2));
  ui.ycoord->setText(str);
  str = QString::asprintf("%0.4lf", pw->Bvect(3));
  ui.zcoord->setText(str);

  dialog->exec();
  ok = dialog->result()==QDialog::Accepted;

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

      pw->Bvect(1) = ui.xcoord->text().toDouble();
      pw->Bvect(2) = ui.ycoord->text().toDouble();
      pw->Bvect(3) = ui.zcoord->text().toDouble();

      // must re-calculate the relative displacements - arrows plotted between different neighbors
      if (pw->DispComponent==EDGE)
	pw->calcEdgeRelDisp();
      else if (pw->DispComponent==SCREW)
	pw->calcScrewRelDisp();

      pw->repaint();
    }
  }
}


void AppWin::calcRDF()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];

  RdfPlot *dialog = new RdfPlot(0, pw, 0.0, 5.0, 100);
  dialog->exec();
  delete(dialog);
}


void AppWin::defineBurgersCircuit()
{
  PltWin *pw = plotWidget[tabWidget->currentIndex()];
  int i;
  bool ok;

  Ui::DDefineBCircuit ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);
  dialog->exec();
  ok = dialog->result()==QDialog::Accepted;
  if (ok) {
    for (i=0; i<tabWidget->count(); i++) {
      plotWidget[i]->ndpoly = 0;
      plotWidget[i]->nchain = 0;
      plotWidget[i]->ndpath = 0;
    }
    pw->napicked = 0;
    pw->apicked.Allocate(10);
    pw->apicked.EnlargeStep(5);
    pw->clickAtoms = true;
    pw->doAfterMouseClicks = CALC_BURG_VECTOR;
    pw->repaint();
  }
}


void AppWin::defineBurgersCircuit_end()
{
  PltWin *pw = plotWidget[tabWidget->currentIndex()];
  QString txt;
  DVector B(3);
  double Blen;
  bool ok;

  pw->doAfterMouseClicks = DO_NOTHING;
    
  BVectFromCircuit(pw, pw->napicked, pw->apicked, B, Blen);

  Ui::DCalcBVector ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);
	
  txt = QString::asprintf("%d", pw->napicked-1);
  ui.numNodes->setText(txt); 
  
  txt = QString::asprintf("%0.6lf", B(1));
  ui.Bx->setText(txt); 
  txt = QString::asprintf("%0.6lf", B(2));
  ui.By->setText(txt); 
  txt = QString::asprintf("%0.6lf", B(3));
  ui.Bz->setText(txt); 
  txt = QString::asprintf("%0.6lf", Blen);
  ui.Blen->setText(txt); 
  
  dialog->exec();
  ok = dialog->result()==QDialog::Accepted;
}


void AppWin::calcDPosPeierlsNabarro_interm()
{
  int icurr = tabWidget->currentIndex();
  PltWin *pwthis = plotWidget[icurr];
  PltWin *pw;
  DVector ubat, dpos(2);
  IVector at;
  double ubmin, ubmax;
  int i, istart, iend, nat, j;
  bool ok;

  //  if (pwthis->nchain==0) {
  //    f = fopen("ub_pos.dat", "w+");
  //    fclose(f);
  //  }

  if (pwthis->applyHow==0)
    istart = iend = icurr;
  else {
    istart = 0;
    iend = tabWidget->count()-1;
  }

  at.Allocate(30); at.EnlargeStep(10);
  ubat.Allocate(30); ubat.EnlargeStep(10);  

  for (i=istart; i<=iend; i++) {
    pw = plotWidget[i];
    if (!pw->isSelected) continue;

    if (i != icurr) {
      pw->applyHow = pwthis->applyHow;
      pw->napicked = pwthis->napicked;
      pw->apicked = pwthis->apicked;
    }

    ok = findDPosPeierlsNabarro(pw, dpos, nat, at, ubat, ubmin, ubmax);
    if (!ok) {
      msgError(err_DPosPeierlsNabarro);
      at.Free();
      ubat.Free();
      pw->napicked = 0;
      pwthis->repaint();
      return;
    }

    if (pw->nchain == 0) {
      pw->dposchain.Allocate(6,2); pw->dposchain.EnlargeStep(2,0);
      pw->nachain.Allocate(6); pw->nachain.EnlargeStep(2);
      pw->achain.Allocate(6,30); pw->achain.EnlargeStep(2,10);
      pw->ubchain.Allocate(6,30); pw->ubchain.EnlargeStep(2,10);
      pw->ubchain_min.Allocate(6); pw->ubchain_min.EnlargeStep(2);
      pw->ubchain_max.Allocate(6); pw->ubchain_max.EnlargeStep(2);
    }

    pw->nchain++;
    pw->dposchain(pw->nchain,1) = dpos(1);
    pw->dposchain(pw->nchain,2) = dpos(2);
    pw->nachain(pw->nchain) = nat;
    pw->ubchain_min(pw->nchain) = ubmin;
    pw->ubchain_max(pw->nchain) = ubmax;
    for (j=1; j<=nat; j++) {
      pw->achain(pw->nchain,j) = at(j);
      pw->ubchain(pw->nchain,j) = ubat(j);
    }

    pw->napicked = 0;
  }

  at.Free();
  ubat.Free();
  pwthis->repaint();
}


void AppWin::calcDPosPeierlsNabarro_end()
{
  int icurr = tabWidget->currentIndex();
  PltWin *pwthis = plotWidget[icurr];
  PltWin *pw;
  int i, ii, j, istart, iend, n;
  bool ok;

  pwthis->doAfterMouseClicks = DO_NOTHING;

  if (pwthis->applyHow==0)
    istart = iend = icurr;
  else {
    istart = 0;
    iend = tabWidget->count()-1;
  }

  // open the dialog window with the results and let the user save the data 
  DPosPNResults *dialog = new DPosPNResults;

  for (i=istart; i<=iend; i++) {
    dialog->txtXY = dialog->txtDPos = "";
    pw = plotWidget[i];
    if (pw->isSelected) {
      CalcDPosPeierlsNabarro(pw, dialog->txtXY, dialog->txtDPos);
      dialog->textXY->append(dialog->txtXY);
      dialog->textDPos->append(dialog->txtDPos);
      if (i<iend)
	dialog->textXY->append("");
    }
  }

  for (i=istart; i<=iend; i++) {
    n = iend-istart+1;
    plotWidget[i]->ndpath = n;
    plotWidget[i]->dpath.Allocate(n,2);
    for (j=1; j<=n; j++) {
      ii = istart+j-1;
      plotWidget[i]->dpath(j,1) = plotWidget[ii]->xdavg;
      plotWidget[i]->dpath(j,2) = plotWidget[ii]->ydavg; 
    }
  }
  
  dialog->exec();
  ok = dialog->result()==QDialog::Accepted;
}


void AppWin::calcDPosDecimate_end()
{
  PltWin *pw = plotWidget[tabWidget->currentIndex()];
  QString txt;
  DMatrix du;
  DVector B(3);
  FILE *fdpoly_init, *fdpoly_rel, *fdpos_init, *fdpos_rel;
  double Blen, xdinit, ydinit, xdrel, ydrel;
  int i, j, a;
  
  du.Allocate(pw->napicked-1,3);
  BVectFromCircuit(pw, pw->napicked, pw->apicked, B, Blen);
  du.Free();

  if (fabs(Blen) < 1e-4) {
    printf("No dislocation is present in the given circuit !\n");
    printf("%0.4lf\n",Blen);
    pw->napicked = 0;
    pw->clickAtoms = false;
    return;
  }
  
  // initialize the random number generator
  //  srand(time(NULL));
  srand(1);  // for testing - get the same sequence of dislocation polygons
  
  fdpoly_init = fopen("dpoly_unrel.dat", "w+");
  fdpoly_rel = fopen("dpoly_rel.dat", "w+");
  fdpos_init = fopen("dpos_unrel.dat", "w+");
  fdpos_rel = fopen("dpos_rel.dat", "w+");
  
  for (i=1; i<=pw->napicked; i++) {
    a = pw->apicked(i);
    fprintf(fdpoly_init, "%10.4lf %10.4lf\n", pw->xyzInit(a,1), pw->xyzInit(a,2));
    fprintf(fdpoly_rel, "%10.4lf %10.4lf\n", pw->xyzRel(a,1), pw->xyzRel(a,2));
  }
  fprintf(fdpoly_init, "\n");
  fprintf(fdpoly_rel, "\n");
  
  for (i=0; i<pw->ncalcdpos; i++) {
    CalcDPosDecimate(pw, !pw->shuffleIdx, xdinit, ydinit, xdrel, ydrel);
    printf("%2d | %7.4lf %7.4lf | %7.4lf %7.4lf\n", i+1, xdinit, ydinit, xdrel, ydrel);
    
    for (j=1; j<=pw->ndpoly_init; j++) {
      fprintf(fdpoly_init, "%10.4lf %10.4lf\n", pw->dpoly_init(j,1), pw->dpoly_init(j,2));
    }
    fprintf(fdpoly_init, "\n");
    
    for (j=1; j<=pw->ndpoly_rel; j++) {
      fprintf(fdpoly_rel, "%10.4lf %10.4lf\n", pw->dpoly_rel(j,1), pw->dpoly_rel(j,2));
    }
    fprintf(fdpoly_rel, "\n");
    
    fprintf(fdpos_init, "%10.4lf %10.4lf\n", xdinit, ydinit);
    fprintf(fdpos_rel, "%10.4lf %10.4lf\n", xdrel, ydrel);
    
    fflush(fdpoly_init);
    fflush(fdpoly_rel);
    fflush(fdpos_init);
    fflush(fdpos_rel);
  }
  
  fclose(fdpos_rel);
  fclose(fdpos_init);
  fclose(fdpoly_rel);
  fclose(fdpoly_init);
}


void AppWin::dlgDPosDecimate()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  bool ok;

  Ui::DFindDPosDecimate ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);

  dialog->exec();
  ok = dialog->result()==QDialog::Accepted;
  if (ok) {
    pw->napicked = 0;
    pw->apicked.Allocate(10);
    pw->apicked.EnlargeStep(5);
    pw->interactive = ui.interactive->isChecked();
    pw->shuffleIdx = ui.shuffleIdx->isChecked();
    if (pw->shuffleIdx)
      pw->ncalcdpos = ui.ncalcdpos->value();
    else
      pw->ncalcdpos = 1;
    pw->clickAtoms = true;
    pw->doAfterMouseClicks = CALC_DPOS_DECIMATE;
  }
}


void AppWin::dlgDPosPeierlsNabarro()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  int i;
  bool ok;

  Ui::DFindDPosPeierlsNabarro ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);
  
  dialog->exec();
  ok = dialog->result()==QDialog::Accepted;
  if (ok) {
    for (i=0; i<tabWidget->count(); i++) {
      plotWidget[i]->ndpoly = 0;
      plotWidget[i]->nchain = 0;
      plotWidget[i]->ndpath = 0;
    }
    pw->applyHow = ui.applyHow->currentIndex();
    pw->napicked = 0;
    pw->apicked.Allocate(10);
    pw->apicked.EnlargeStep(5);
    pw->clickAtoms = true;
    pw->doAfterMouseClicks = CALC_DPOS_PEIERLSNABARRO;
    pw->repaint();
  }
}


/* 
  Select atoms by clicking on them with mouse and save their positions and relative displacements into
  a file for further processing in an external program.
*/

void AppWin::probeAtomDispl()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  int i;
  bool ok;
  
  Ui::DSetupProbeAtomDispl ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);

  dialog->exec();
  ok = dialog->result()==QDialog::Accepted;
  if (ok) {
    for (i=0; i<tabWidget->count(); i++) {
      plotWidget[i]->ndpoly = 0;
      plotWidget[i]->nchain = 0;
      plotWidget[i]->ndpath = 0;
    }
    pw->napicked = 0;
    pw->apicked.Allocate(10);
    pw->apicked.EnlargeStep(5);
    pw->clickAtoms = true;
    pw->doAfterMouseClicks = CALC_ATOM_DISPL;
    pw->repaint();
  }
}


void AppWin::defineCalcAtomDispl_end()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  pw->doAfterMouseClicks = DO_NOTHING;
  ProbeAtomDispl *dialog = new ProbeAtomDispl(0, pw);  
  dialog->exec();
  delete(dialog);
}
